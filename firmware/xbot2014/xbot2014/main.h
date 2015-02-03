#ifndef xbot2014_main_h
#define xbot2014_main_h

#include <stdint.h>

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))
#define CPU_16MHz       0x00
#define CPU_8MHz        0x01
#define CPU_4MHz        0x02
#define CPU_2MHz        0x03
#define CPU_1MHz        0x04
#define CPU_500kHz      0x05
#define CPU_250kHz      0x06
#define CPU_125kHz      0x07
#define CPU_62kHz       0x08

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_OFF		(PORTD &= ~(1<<6))
#define LED_ON		(PORTD |= (1<<6))
#define LED_TOGGLE  (PORTD ^= (1<<6)) // probably broken...

#define DO_NOTHING()

#ifdef __AVR_ATmega32U4__
#define JUMP_TO_BOOTLOADER() \
    do              \
    {               \
        cli();      \
        UDCON = 1;  \
        USBCON = (1<<FRZCLK);   \
        UCSR1B = 0; \
        _delay_ms(5);   \
        EIMSK = 0; PCICR = 0; SPCR = 0; ACSR = 0; EECR = 0; ADCSRA = 0; \
        TIMSK0 = 0; TIMSK1 = 0; TIMSK3 = 0; TIMSK4 = 0; UCSR1B = 0; TWCR = 0;   \
        DDRB = 0; DDRC = 0; DDRD = 0; DDRE = 0; DDRF = 0; TWCR = 0; \
        PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0; PORTF = 0;  \
        asm volatile("jmp 0x7E00"); \
    } while (0)
#endif


static uint8_t recv_str(char *buf, uint8_t size);
static void parse_and_execute_command(const char *buf);

#endif
