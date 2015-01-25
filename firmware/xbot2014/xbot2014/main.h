#ifndef xbot2014_main_h
#define xbot2014_main_h

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

#define PSX_PORT    (PORTB)
#define PSX_DDR     (DDRB)

#define PSX_SS   (PB0)
#define PSX_CLK  (PB1)
#define PSX_MISO (PB2)
#define PSX_MOSI (PB3)
#define PSX_ACK  (PB7)

#define PSX_STATE_HEADER        (0x00)
#define PSX_STATE_PRESSURE      (0x40)
#define PSX_STATE_CHECK         (0x41)
#define PSX_STATE_POLL          (0x42)
#define PSX_STATE_CONFIG        (0x43)
#define PSX_STATE_MODE          (0x44)
#define PSX_STATE_STATUS        (0x45)
#define PSX_STATE_CONST0        (0x46)
#define PSX_STATE_CONST1        (0x47)
#define PSX_STATE_CONST2        (0x4C)
#define PSX_STATE_VIBRATE       (0x4D)
#define PSX_STATE_BYTESET       (0x4F)

#define PSX_CMD_HEADER          (0x01)
#define PSX_CMD_ENTER_CONFIG    (0x01)
#define PSX_CMD_EXIT_CONFIG     (0x00)
#define PSX_CMD_DIGITAL         (0x00)
#define PSX_CMD_ANALOG          (0x01)
#define PSX_CMD_LOCKED          (0x03)

#define PSX_MODE_DIGITAL        (0x41)
#define PSX_MODE_ANALOG         (0x73)
#define PSX_MODE_PRESSURES      (0x79)
#define PSX_MODE_CONFIG         (0xF3)

#endif
