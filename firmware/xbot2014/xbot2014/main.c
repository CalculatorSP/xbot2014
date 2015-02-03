/***********************************************************************
 *
 * Interface for Communication Between PC and PSX through Serial -> SPI
 * https://github.com/CalculatorSP/xbot2014
 * Copyright (c) 2014,2015 John Miller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 ***********************************************************************/

#include "main.h"

#include "psx.h"
#include "usb_serial.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define PACKET_SIZE (38)    // Does not include separator
#define BUFFER_SIZE (64)

int main(void)
{
    char buf[BUFFER_SIZE];
    char rumbuf[4];
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
            sprintf(rumbuf, "%04hx", rum);
            for (uint8_t i = 0; i < 4; ++i)
                usb_serial_putchar(rumbuf[i]);
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
    uint8_t instr[18];
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
            for (int i = 0; i < sizeof(instr); ++i)
            {
                char c = buf[(i+1)<<1];
                if (c >= '0' && c <= '9')
                    instr[i] = (c - '0') << 4;
                else if (c >= 'A' && c <= 'F')
                    instr[i] = (c - 'A') << 4;
                else if (c >= 'a' && c <= 'f')
                    instr[i] = (c - 'a') << 4;
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
                    instr[i] |= (c - 'A');
                else if (c >= 'a' && c <= 'f')
                    instr[i] |= (c - 'a');
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
            
            // Deposit multiple times, depending on second character
            for (uint8_t i = 0; i < instr_count; ++i)
                if (!psx_deposit(instr))
                {
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


