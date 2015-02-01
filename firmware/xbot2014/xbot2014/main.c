#include "main.h"

#include "psx.h"
#include "usb_serial.h"

#include <stdint.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>


int main(void)
{
    uint8_t instr[18];
    
    CPU_PRESCALE(CPU_16MHz);
    LED_CONFIG;
    
    // Start communications with PSX
    psx_setup();
    
    // Initialize USB and wait for communication to PC
    usb_init();
    while (!usb_configured())
        DO_NOTHING();
        
    _delay_ms(1000.0);
    
    // Discard any garbage sent by OS
    usb_serial_flush_input();
    
    memcpy(instr, psx_get_default(), sizeof(instr));
    instr[1] &= ~(1<<6);
    instr[1] &= ~(1<<2);
    instr[1] &= ~(1<<1);
    
    while(1)
    {
        _delay_ms(1000.0);
        instr[17] = 0xFF;
        psx_deposit(instr);
        
        _delay_ms(1000.0);
        instr[17] = 0x00;
        psx_deposit(instr);
    }
    
    return 0;
}


