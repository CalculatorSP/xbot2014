/***********************************************************************
 *
 * psx.c
 *
 * API for PSX Controller Emulation
 *
 * Copyright (c) 2014,2015 John Miller
 *
 ***********************************************************************/

#define PSX_PRIVATE_INCLUDE
#include "psx.h"
#include "pipe.h"
#include "usb_serial.h"


/***********************************************************************
 *
 *  Configurable Options
 *
 ***********************************************************************/

// Number of controller instructions to buffer
#define CONTROLLER_BUFSIZE  (100)

// Port/pin setup (change this if your hardware configuration is different)
#define PSX_PORT    (PORTB)
#define PSX_DDR     (DDRB)

#define PSX_SS   (PB0)
#define PSX_CLK  (PB1)
#define PSX_MOSI (PB2)
#define PSX_MISO (PB3)
#define PSX_ACK  (PB7)

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define LED_ON		(PORTD |= (1<<6))


/***********************************************************************
 *
 *  Private Variables - Data Packets, etc
 *
 ***********************************************************************/

// Internal packets, automatically configured
static uint8_t packet_header[3] =
{ 0x41, 0x5A, 0xFF };

static uint8_t packet_pressure[7] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x5A, 0xFF };

static uint8_t packet_check[7] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };

static const uint8_t packet_poll_default[19] =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };

static uint8_t packet_status[7] =
{ 0x03, 0x02, 0x00, 0x02, 0x01, 0x00, 0xFF };

static uint16_t packet_const0[7] =
{ 0x0000, 0x0000, 0x0000, 0x0002, 0x0000, 0x140A, 0xFF };

static uint8_t packet_const1[7] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF };

static uint16_t packet_const2[7] =
{ 0x0000, 0x0000, 0x0000, 0x0604, 0x0000, 0x0000, 0xFF};

static uint8_t packet_vibrate[7] =
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

static uint8_t packet_byteset[7] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A, 0xFF };

// Encodes current device mode (analog/digital/pressures)
static volatile uint8_t devicemode = PSX_MODE_DIGITAL;

// Current intensity of rumble motors
//static volatile uint8_t rumble0_val = 0x00;
//static volatile uint8_t rumble1_val = 0x00;

// Track whether polling data is new
//static volatile uint8_t new_data = 0;

// Keep track of which packet we are sending, and how many bytes are in it
static uint8_t state = PSX_STATE_HEADER;
static uint8_t length_to_send = 2;

// Represents index to SEND (1 more than index to receive)
static uint8_t bytenum = 0;

// Determines whether to send first or second byte of const values
static uint8_t const_num = 0x00;

// Controller data to send (send empty data until we hear from PC)
static uint8_t packet_poll[20];

// Buffer for controller instructions (to go in pipe)
static uint8_t controller_buffer[CONTROLLER_BUFSIZE * 20];

// FIFO pipe for pending instructions
static pipe_t controller_pipe;


/***********************************************************************
 *
 *  Public Functions - API for commmunicating with PSX
 *
 ***********************************************************************/

void psx_setup(void)
{
    // Ensure we don't get an SPI interrupt before we are ready to handle it
    cli();
    
    // Set up MISO as output, others as inputs
    PSX_DDR = 0;
    PSX_DDR |= (1<<PSX_MISO);
    
    // Initialize SPI data register
    SPDR = 0xFF;
    
    // Set up SPI
    SPCR = 0;
    SPCR |= (1<<SPE);  // Enable SPI
    SPCR |= (1<<SPIE); // Enable SPI interrupts
    SPCR |= (1<<DORD); // Data order reversed
    SPCR |= (1<<CPOL); // Read on rising edge
    SPCR |= (1<<CPHA); // Clock polarity inverted
    
    // Initialize pipe
    pipe_init(&controller_pipe, controller_buffer, 20, CONTROLLER_BUFSIZE);
    
    // Set initial instruction
    memcpy(packet_poll, packet_poll_default, 19);
    packet_poll[19] = 3;
    
    // Enable global interrupts
    sei();
}

uint8_t psx_deposit(const uint8_t *instr)
{
    if (!pipe_write(&controller_pipe, instr))
        return 0;
    
    return 1;
}

//const uint8_t *psx_get_default(void)
//{
//    return packet_poll_default;
//}

//uint8_t psx_get_devicemode(void)
//{
//    uint8_t retval;
//    
//    // Ensure atomicity to avoid conflicts
//    SPCR &= ~(1<<SPIE);
//    retval = devicemode;
//    SPCR |= (1<<SPIE);
//    
//    return retval;
//}

//uint8_t psx_get_rumble(uint16_t *rum)
//{
//    uint8_t retval;
//    
//    // Ensure atomicity to avoid conflicts
//    SPCR &= ~(1<<SPIE);
//    
//    if (rum != NULL)
//        *rum = (((uint16_t)rumble1_val)<<8) | (uint16_t)rumble0_val;
//    
//    if (new_data)
//    {
//        new_data = 0;
//        retval = 1;
//    }
//    else
//        retval = 0;
//    
//    SPCR |= (1<<SPIE);
//    
//    return retval;
//}


/***********************************************************************
 *
 *  Private Functions
 *
 ***********************************************************************/

static inline void psx_ack(void)
{
    // Set ACK low (by making it an output)
    PSX_DDR |= (1<<PSX_ACK);
    
    // Wait 10 us
    _delay_us(10.0);
    
    // Set ACK high (by making it an input)
    PSX_DDR &= ~(1<<PSX_ACK);
}

// Handle one byte of communication with PSX
static inline void handle_next_spi_byte(void)
{
    // Read data from console
    uint8_t command = SPDR;
    
    switch (state)
    {
        // First 3 bytes of packet
        case (PSX_STATE_HEADER):
            if (bytenum == 0)
            {
                if (command == PSX_CMD_HEADER)
                {
                    SPDR = packet_header[0];
                    break;
                }
                else // Not a start of packet
                {
                    //LED_ON;
                    SPDR = 0xFF;
                    state = PSX_STATE_HEADER;
                    bytenum = 0;
                    length_to_send = 2;
                    return;
                }
            }
            else if (bytenum == 1)
            {
                if (command == PSX_STATE_POLL)
                {
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    packet_header[0] = devicemode; // Exit config mode
                    length_to_send = (packet_header[0] & 0x0F) << 1;
                    psx_ack();
                }
                else if (command == PSX_STATE_CONFIG)
                {
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    length_to_send = (packet_header[0] & 0x0F) << 1;
                    psx_ack();
                }
                else if (packet_header[0] == PSX_MODE_CONFIG)
                {
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    length_to_send = 6;
                    psx_ack();
                    
                }
                else // Got a config-only command while not in config mode
                {
                    //LED_ON;
                    SPDR = 0xFF;
                    state = PSX_STATE_HEADER;
                    bytenum = 0;
                    length_to_send = 2;
                }
                return;
            }
            break;
            
        // Initialize pressure sensor
        case (PSX_STATE_PRESSURE):
            SPDR = packet_pressure[bytenum];
            break;
            
        // Find out what buttons are included in poll responses
        case (PSX_STATE_CHECK):
            SPDR = packet_check[bytenum];
            break;
            
        // Main polling command
        case (PSX_STATE_POLL):
            SPDR = packet_poll[bytenum];
//            if (bytenum > 0 && bytenum < sizeof(packet_vibrate))
//            {
//                if (packet_vibrate[bytenum-1] == 0x00)
//                    rumble0_val = command;
//                else if (packet_vibrate[bytenum-1] == 0x01)
//                    rumble1_val = command;
//            }
            if (bytenum == length_to_send)
            {
                //new_data = 1;
                
                // If controller instructions are pending, go to next one. Otherwise, resend
                // the previous one.
                if (packet_poll[19] > 1)
                    --packet_poll[19];
                else
                    pipe_read(&controller_pipe, packet_poll);
//                {
////                    for (uint8_t i = 0; i < sizeof(packet_poll); ++i)
////                    {
////                        usb_serial_putchar(packet_poll[i]/16 + 'A');
////                        usb_serial_putchar((packet_poll[i]&15) + 'a');
////                    }
////                    usb_serial_putchar('\n');
//                }
            }
            break;
            
        // Enter/Exit Config Mode, also poll all button states, joysticks, and pressures
        case (PSX_STATE_CONFIG):
            SPDR = packet_poll[bytenum];
            if (bytenum == 1)
            {
                if (command == PSX_CMD_ENTER_CONFIG)
                    packet_header[0] = PSX_MODE_CONFIG;
                else if (command == PSX_CMD_EXIT_CONFIG)
                    packet_header[0] = devicemode;
            }
            break;
            
        // Switch modes between digital and analog
        case (PSX_STATE_MODE):
            SPDR = 0x00;
            if (bytenum == 1)
            {
                if (command == PSX_CMD_DIGITAL)
                {
                    devicemode = PSX_MODE_DIGITAL;
                    packet_status[2] = 0x00;
                }
                else if (command == PSX_CMD_ANALOG)
                {
                    devicemode = PSX_MODE_ANALOG;
                    packet_status[2] = 0x01;
                }
            }
            break;
            
        // Get more status info
        case (PSX_STATE_STATUS):
            SPDR = packet_status[bytenum];
            break;
            
        // Read an unknown constant value from controller
        case (PSX_STATE_CONST0):
            if (const_num == 0x00)
                SPDR = (uint8_t)(packet_const0[bytenum] & 0x00FF);
            else if (const_num == 0x01)
                SPDR = (uint8_t)(packet_const0[bytenum] >> 8);
            if (bytenum == 1)
                const_num = command;
            break;
            
        // Read an unknown constant value from controller
        case (PSX_STATE_CONST1):
            SPDR = packet_const1[bytenum];
            break;
            
        // Read an unknown constant value from controller
        case (PSX_STATE_CONST2):
            if (const_num == 0x00)
                SPDR = (uint8_t)(packet_const2[bytenum] & 0x00FF);
            else if (const_num == 0x01)
                SPDR = (uint8_t)(packet_const2[bytenum] >> 8);
            if (bytenum == 1)
                const_num = command;
            break;
            
        // Map bytes in the poll command to actuate the vibration motors
        case (PSX_STATE_VIBRATE):
            SPDR = packet_vibrate[bytenum];
            if (bytenum > 0)
                packet_vibrate[bytenum-1] = command;
            break;
            
        // Add or remove analog response bytes from the main polling command
        case (PSX_STATE_BYTESET):
            SPDR = packet_byteset[bytenum];
            if (bytenum > 0)
                packet_check[bytenum-1] = command;
            if (bytenum == 1)
                devicemode = PSX_MODE_PRESSURES;
            break;
            
        // Bad state, just reset
        default:
            //LED_ON;
            SPDR = 0xFF;
            state = PSX_STATE_HEADER;
            bytenum = 0;
            length_to_send = 2;
            return;
    }
    
    // Move to next byte
    // Don't ack for last byte in packet
    ++bytenum;
    if (bytenum <= length_to_send)
        psx_ack();
    else
    {
        state = PSX_STATE_HEADER;
        bytenum = 0;
        length_to_send = 2;
    }
}

// SPI transfer interrupt
ISR(SPI_STC_vect)
{
    // Do the communication
    handle_next_spi_byte();
}


