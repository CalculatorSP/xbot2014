#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define PSX_PORT PORTB
#define PSX_DDR DDRB

#define PSX_SS   PB0
#define PSX_CLK  PB1
#define PSX_MISO PB2
#define PSX_MOSI PB3
#define PSX_ACK  PB7

// Digital PS1 controller
#define PSX_DEVICE_TYPE 0x41

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

// Initial value for tmr0 giving us a 10uS delay (ACK timer)
const uint8_t tmr0Initial = 255 - (F_CPU / 100000);

uint8_t buttonsL;
uint8_t buttonsH;

void psx_setup(void)
{
    // Set up ACK and MISO as outputs, others as inputs
    PSX_DDR = 0;
    PSX_DDR |= (1 << PSX_ACK);
    PSX_DDR |= (1 << PSX_MISO);
    
    // Set up SPI
    SPCR = 0;
    SPCR |= (1<<SPE);  // Enable SPI
    SPCR |= (1<<SPIE); // Enable SPI interrupts
    SPCR |= (1<<DORD); // Data order reversed
    SPCR |= (1<<CPOL); // Read on rising edge
    SPCR |= (1<<CPHA); // Clock polarity inverted
    
    // Enable timer interrupts
    TIMSK0 = (1 << TOIE0);
}

void psx_ack(void)
{
    // Set ACK high
    PSX_PORT |= (1 << PSX_ACK);
    TCNT0 = 0;
    TCCR0B = (1 << CS00); // No prescaler
}

void psx_send(uint8_t data)
{
    SPDR = data;
}

int main(void)
{
    CPU_PRESCALE(0);
    
    // Set up PSX
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
    static uint8_t byteNumber = 0;
    uint8_t received = SPDR;
    
    if (received == 0x01)
    {
        byteNumber = 0; //reset back to start again
        psx_ack();
        
        //the next byte we send is our device type
        psx_send(~PSX_DEVICE_TYPE);
    }
    
    if (byteNumber == 1)
    {
        if (received == 0x42)
        {
            psx_ack();
            
            //the next byte we send is our preamble
            psx_send(0xa5);
        }
    }
    
    else if (byteNumber == 2)
    {
        if (received == 0x00)
        {
            psx_ack();
            
            //next thing we send is lower button byte
            psx_send(buttonsL);
        }
    }
    
    else if (byteNumber == 3)
    {
        if (received == 0x00)
        {
            psx_ack();
            
            //next thing we send is upper button byte
            psx_send(buttonsH);
        }
    }
    
    byteNumber++;
}

// ACK timer interrupt
ISR(TIMER0_OVF_vect)
{
    // Turn off timer
    TCCR0B = 0;
    
    // Set ACK low
    PSX_PORT &= ~(1 << PSX_ACK);
}