/***********************************************************************
 *
 * main.c
 *
 * Interface for Communication Between PC and PSX through Serial -> SPI
 *
 * https://github.com/CalculatorSP/xbot2014
 * Copyright (c) 2014,2015 John Miller
 *
 ***********************************************************************/

#include "main.h"

#include "psx.h"
#include "usb_serial.h"

#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define PACKET_SIZE (38)    // Does not include separator
#define BUFFER_SIZE (64)

static uint8_t recv_str(char *buf, uint8_t size);
static void parse_and_execute_command(const char *buf);

int main(void)
{
    char buf[BUFFER_SIZE];
    uint8_t n;
    uint16_t rum;
    
    CPU_PRESCALE(CPU_16MHz);
    LED_CONFIG;
    
    // Start communications with PSX
    psx_setup();
    
    // Initialize USB and wait for communication to PC
    usb_init();
    while (!usb_configured())
        DO_NOTHING();
        
    _delay_ms(1000.0);
        
    while (1)
    {
        // Wait for the user to run xbot on PC
        while (!(usb_serial_get_control() & USB_SERIAL_DTR))
            DO_NOTHING();
                        
        // Discard any garbage from OS
        usb_serial_flush_input();
        
        // Indicate ready
        usb_serial_putchar('r');
        usb_serial_putchar('\n');
        
        // Listen for commands and process them
        while (1)
        {
            // Get command
            n = recv_str(buf, sizeof(buf));
            
            // Check for missing delimeter
            if (n == BUFFER_SIZE)
            {
                usb_serial_putchar('d');
                usb_serial_putchar(':');
                for (uint8_t i = 0; i < BUFFER_SIZE; ++i)
                    usb_serial_putchar(buf[i]);
                usb_serial_putchar('\n');
                continue;
            }
            
            // Check for disconnect
            if (n == 255)
                break;
            
            // Execute command
            parse_and_execute_command(buf);
            
            // Get rumble data from PSX
            psx_get_rumble(&rum);
            
            // Send devive mode
            usb_serial_putchar('m');
            usb_serial_putchar(':');
            switch (psx_get_devicemode())
            {
                case PSX_MODE_DIGITAL:
                    usb_serial_putchar('d');
                    break;
                    
                case PSX_MODE_ANALOG:
                    usb_serial_putchar('a');
                    break;
                    
                case PSX_MODE_PRESSURES:
                    usb_serial_putchar('p');
                    break;
                    
                default:
                    usb_serial_putchar('e');
                    break;
            }
            usb_serial_putchar('\n');
            
            // Send rumble data
            usb_serial_putchar('v');
            usb_serial_putchar(':');
            for (int8_t i = 12; i >= 0; i -= 4)
            {
                uint8_t nibble = (rum >> i) & 0x000F;
                if (nibble < 0xA)
                    usb_serial_putchar('0' + nibble);
                else
                    usb_serial_putchar('A' + (nibble - 0xA));
            }
            usb_serial_putchar('\n');
        }
    }
    
    return 0;
}

// Receive a string from the USB serial port. The string is stored
// in the buffer and this function will not exceed the buffer size.
// Any character outside ' ' to '~' completes the string, and will be
// replaced by a null terminator.
// The return value is the number of characters received, not
// including null terminator, or 255 if the virtual serial
// connection was closed while waiting.
//
uint8_t recv_str(char *buf, uint8_t size)
{
    int16_t r;
    uint8_t count=0;
    
    while (count < size)
    {
        r = usb_serial_getchar();
        if (r != -1)
        {
            if (r >= '!' && r <= '~')
            {
                *buf++ = r;
                count++;
            }
            else
            {
                *buf = '\0';
                return count;
            }
        }
        else if (!usb_configured() || !(usb_serial_get_control() & USB_SERIAL_DTR))
            return 255;
    }
    return count;
}

void parse_and_execute_command(const char *buf)
{
    uint8_t instr[19];
    int instr_count = 0;
    
    if (strlen(buf) != PACKET_SIZE)
    {
        // Size error
        usb_serial_putchar('s');
        usb_serial_putchar(':');
        for (uint8_t j = 0; j < strlen(buf); ++j)
            usb_serial_putchar(buf[j]);
        usb_serial_putchar('\n');
        return;
    }
    
    // First character is header (instruction)
    switch (buf[0])
    {
        // Controller instruction: deposit in PSX
        case 'c':
            // Second character determines how many times to deposit instr
            instr_count = buf[1] - '0';
            
            // Parse string into uint8_t's
            for (int i = 0; i < sizeof(instr)-1; ++i)
            {
                char c = buf[(i+1)<<1];
                if (c >= '0' && c <= '9')
                    instr[i] = (c - '0') << 4;
                else if (c >= 'A' && c <= 'F')
                    instr[i] = (c - 'A' + 0xA) << 4;
                else if (c >= 'a' && c <= 'f')
                    instr[i] = (c - 'a' + 0xA) << 4;
                else
                {
                    // Parse error
                    usb_serial_putchar('p');
                    usb_serial_putchar(':');
                    for (uint8_t j = 0; j < strlen(buf); ++j)
                        usb_serial_putchar(buf[j]);
                    usb_serial_putchar('\n');
                    return;
                }
                
                c = buf[((i+1)<<1)+1];
                if (c >= '0' && c <= '9')
                    instr[i] |= (c - '0');
                else if (c >= 'A' && c <= 'F')
                    instr[i] |= (c - 'A' + 0xA);
                else if (c >= 'a' && c <= 'f')
                    instr[i] |= (c - 'a' + 0xA);
                else
                {
                    // Parse error
                    usb_serial_putchar('p');
                    usb_serial_putchar(':');
                    for (uint8_t j = 0; j < strlen(buf); ++j)
                        usb_serial_putchar(buf[j]);
                    usb_serial_putchar('\n');
                    return;
                }
            }
            
            // Last byte is always 0xFF
            instr[18] = 0xFF;
            
            // Deposit multiple times, depending on second character
            for (uint8_t i = 0; i < instr_count; ++i)
                if (!psx_deposit(instr))
                {
                    LED_ON;
                    // Buffer full, can't deposit
                    usb_serial_putchar('f');
                    usb_serial_putchar(':');
                    usb_serial_putchar('0'+i);
                    usb_serial_putchar('\n');
                    return;
                }
            
            // Instruction ok
            usb_serial_putchar('k');
            usb_serial_putchar('\n');
            break;
            
        // Get vibration data only (no new instruction)
        case 'v':
            break;
            
        // Program device: jump to bootloader
        case 'p':
            // Downloading
            usb_serial_putchar('d');
            usb_serial_putchar('\n');
            JUMP_TO_BOOTLOADER();
            
            
        // Unknown instruction
        default:
            // Header error
            usb_serial_putchar('h');
            usb_serial_putchar(':');
            for (uint8_t j = 0; j < strlen(buf); ++j)
                usb_serial_putchar(buf[j]);
            usb_serial_putchar('\n');
            break;
    }
}


