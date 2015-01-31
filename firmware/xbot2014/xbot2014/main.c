#include "main.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

// Initial value for tmr0 giving us a 10uS delay (ACK timer)
const uint8_t TMR0_INITIAL =  (uint8_t)(-(F_CPU / 100000));

// Packets containing data to be sent to PSX (last byte is first byte of header)
uint8_t packet_header[3] =
{ 0x41, 0x5A, 0xFF };

uint8_t packet_pressure[7] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x5A, 0xFF };

uint8_t packet_poll_default[19] =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };

uint8_t packet_check[7] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };

uint8_t packet_status[7] =
{ 0x03, 0x02, 0x00, 0x02, 0x01, 0x00, 0xFF };

uint16_t packet_const0[7] =
{ 0x0000, 0x0000, 0x0000, 0x0002, 0x0000, 0x140A, 0xFF };

uint8_t packet_const1[7] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0xFF };

uint16_t packet_const2[7] =
{ 0x0000, 0x0000, 0x0000, 0x0604, 0x0000, 0x0000, 0xFF};

uint8_t packet_vibrate[7] =
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

uint8_t packet_byteset[7] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A, 0xFF };

// PSX will fill these with rumble data
uint8_t rumble0_val = 0x00;
uint8_t rumble1_val = 0x00;

// Encodes digital/analog, and number of poll bytes to send
uint8_t devicemode = PSX_MODE_DIGITAL;

// Serial data will fill this buffer until PSX is ready for it
uint8_t controller_buffer[CONTROLLER_BUFSIZE][19];

// Serial data is written to this index in controller_buffer
uint8_t write_to = 0;

inline void psx_setup(void)
{
    // Set up MISO as output, others as inputs
    PSX_DDR = 0;
    PSX_DDR |= (1<<PSX_MISO);
    
    // Initialize SPI data register
    SPDR = 0xFF;
    
    // Initialize controller buffer
    for (int i = 0; i < CONTROLLER_BUFSIZE; ++i)
        memcpy(controller_buffer[i], packet_poll_default, sizeof(packet_poll_default));
    
    // Set up SPI
    SPCR = 0;
    SPCR |= (1<<SPE);  // Enable SPI
    SPCR |= (1<<SPIE); // Enable SPI interrupts
    SPCR |= (1<<DORD); // Data order reversed
    SPCR |= (1<<CPOL); // Read on rising edge
    SPCR |= (1<<CPHA); // Clock polarity inverted
    
    // Enable timer interrupts
    TIMSK0 = (1<<TOIE0);
}

inline void psx_ack(void)
{
    // Set ACK low (by making it an output)
    PSX_DDR |= (1<<PSX_ACK);
    
    // Set timer to overflow in 10uS
    TCNT0 = TMR0_INITIAL;
    
    // Start timer with no prescaler
    TCCR0B = (1<<CS00);
}

int main(void)
{
    CPU_PRESCALE(CPU_16MHz);
    LED_CONFIG;
    psx_setup();
    
    // Enable interrupts
    sei();
    
    while(1)
    {
        _delay_ms(1000.0);
        controller_buffer[write_to][0] = 0xEF;
        cli();
        write_to = (write_to + 1) % CONTROLLER_BUFSIZE;
        sei();
        
        _delay_ms(250.0);
        controller_buffer[write_to][0] = 0xFF;
        cli();
        write_to = (write_to + 1) % CONTROLLER_BUFSIZE;
        sei();
    }
    
    return 0;
}

// SPI tranfer interrupt
ISR(SPI_STC_vect)
{
    // Keep track of which packet we are sending, and how many bytes are in it
    static uint8_t state = PSX_STATE_HEADER;
    static uint8_t length_to_send = 2;
    
    // Represents index to SEND (1 more than index to receive)
    static uint8_t bytenum = 0;
    
    // Determines whether to send first or second byte of const values
    static uint8_t const_num = 0x00;
    
    // Controller data to send (send empty data until we hear from PC)
    static uint8_t *packet_poll = packet_poll_default;
    
    // Index in controller buffer to send from
    static uint8_t read_from = 0;
    
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
                    psx_ack();
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    packet_header[0] = devicemode; // Exit config mode
                    length_to_send = (packet_header[0] & 0x0F) << 1;
                    
                    // If controller instructions are pending, go to next one. Otherwise, resend
                    // the previous one. Only send if PSX is listening for full pressure info
                    if (read_from != write_to && devicemode == PSX_MODE_PRESSURES)
                    {
                        packet_poll = controller_buffer[read_from];
                        read_from = (read_from + 1) % CONTROLLER_BUFSIZE;
                    }
                }
                else if (command == PSX_STATE_CONFIG)
                {
                    psx_ack();
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    length_to_send = (packet_header[0] & 0x0F) << 1;
                }
                else if (packet_header[0] == PSX_MODE_CONFIG)
                {
                    psx_ack();
                    SPDR = packet_header[1];
                    state = command;
                    bytenum = 0;
                    length_to_send = 6;
                }
                else // Got a config-only command while not in config mode
                {
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
            if (packet_vibrate[bytenum] == 0x00)
                rumble0_val = command;
            else if (packet_vibrate[bytenum] == 0x01)
                rumble1_val = command;
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
            SPDR = 0xFF;
            state = PSX_STATE_HEADER;
            bytenum = 0;
            length_to_send = 2;
            return;
    }
    
    // Move to next byte
    // Don't ack for last byte in packet
    bytenum++;
    if (bytenum <= length_to_send)
        psx_ack();
    else
    {
        state = PSX_STATE_HEADER;
        bytenum = 0;
        length_to_send = 2;
    }
}

// ACK timer interrupt
ISR(TIMER0_OVF_vect)
{
    // Turn off timer
    TCCR0B = 0;
    
    // Set ACK high (by making it an input)
    PSX_DDR &= ~(1<<PSX_ACK);
}