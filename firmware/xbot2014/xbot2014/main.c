#include "main.h"

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Initial value for tmr0 giving us a 10uS delay (ACK timer)
const uint8_t TMR0_INITIAL =  (uint8_t)(-(F_CPU / 100000));

uint8_t packet_header[3] =
{ 0xFF, 0x41, 0x5A };

uint8_t packet_pressure[6] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x5A };

uint8_t packet_check[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t packet_poll[18] =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

uint8_t packet_status[6] =
{ 0x03, 0x02, 0x00, 0x02, 0x01, 0x00 };

uint16_t packet_const0[6] =
{ 0x0000, 0x0000, 0x0000, 0x0002, 0x0000, 0x140A };

uint8_t packet_const1[6] =
{ 0x00, 0x00, 0x02, 0x00, 0x00, 0x00 };

uint16_t packet_const2[6] =
{ 0x0000, 0x0000, 0x0000, 0x0604, 0x0000, 0x0000 };

uint8_t packet_vibrate[6] =
{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

uint8_t packet_byteset[6] =
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x5A };

uint8_t rumble0_val = 0x00;
uint8_t rumble1_val = 0x00;
uint8_t devicemode = 0x41;

bool locked = false;

inline void psx_setup(void)
{
    // Set up MISO as output, others as inputs
    PSX_DDR = 0;
    PSX_DDR |= (1<<PSX_MISO);
    
    // Initialize SPI data register
    SPDR = packet_header[0];
    
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
    psx_setup();
    
    // Enable interrupts
    sei();
    
    while(1)
    {
    }
    
    return 0;
}

// SPI tranfer interrupt
ISR(SPI_STC_vect)
{
    static uint8_t state = PSX_STATE_HEADER;
    static uint8_t length_to_send = 3;
    
    // Represents index to SEND (1 more than index to receive)
    static uint8_t bytenum = 1;
    
    // Determines whether to send first or second byte of const values
    static uint8_t const_num = 0x00;
    
    // Read data from console
    uint8_t command = SPDR;
    
    switch (state)
    {
        // First 3 bytes of packet
        case (PSX_STATE_HEADER):
            if (bytenum == 1)
            {
                if (command == PSX_CMD_HEADER)
                {
                    SPDR = packet_header[1];
                    break;
                }
                else // Not a start of packet
                {
                    SPDR = packet_header[0];
                    return;
                }
            }
            else if (bytenum == 2)
            {
                if (command == PSX_STATE_POLL)
                {
                    psx_ack();
                    SPDR = packet_header[2];
                    state = command;
                    bytenum = 0;
                    packet_header[1] = devicemode; // Exit config mode
                    length_to_send = (packet_header[1] & 0x0F) << 1;
                }
                else if (command == PSX_STATE_CONFIG)
                {
                    psx_ack();
                    SPDR = packet_header[2];
                    state = command;
                    bytenum = 0;
                    length_to_send = (packet_header[1] & 0x0F) << 1;
                }
                else if (packet_header[1] == PSX_MODE_CONFIG)
                {
                    psx_ack();
                    SPDR = packet_header[2];
                    state = command;
                    bytenum = 0;
                    length_to_send = 6;
                }
                else // Got a config-only command while not in config mode
                {
                    SPDR = packet_header[0];
                    state = PSX_STATE_HEADER;
                    bytenum = 1;
                    length_to_send = 3;
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
                    packet_header[1] = PSX_MODE_CONFIG;
                else if (command == PSX_CMD_EXIT_CONFIG)
                    packet_header[1] = devicemode;
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
                    packet_status[2] == 0x01;
                }
            }
            if (bytenum == 2)
                locked = (command == PSX_CMD_LOCKED);
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
            packet_vibrate[bytenum] = command;
            break;
            
        // Add or remove analog response bytes from the main polling command
        case (PSX_STATE_BYTESET):
            SPDR = packet_byteset[bytenum];
            packet_check[bytenum] = command;
            if (bytenum == 1)
                devicemode = PSX_MODE_PRESSURES;
            break;
            
        // Bad state, just reset
        default:
            SPDR = packet_header[0];
            state = PSX_STATE_HEADER;
            bytenum = 1;
            length_to_send = 3;
            return;
    }
    
    // Move to next byte
    // Don't ack for last byte in header
    if (++bytenum < length_to_send)
        psx_ack();
    else
    {
        SPDR = packet_header[0];
        state = PSX_STATE_HEADER;
        bytenum = 1;
        length_to_send = 3;
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