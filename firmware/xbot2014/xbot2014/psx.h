#ifndef __xbot2014__psx__
#define __xbot2014__psx__

#include <stdint.h>

void psx_setup(void);                   // initialize SPI, etc.
uint8_t psx_deposit(const uint8_t *instr); // set the next controller state
                                        //  returns false if buffer is full (does not deposit)
const uint8_t *psx_get_default(void);   // get default packet (no buttons)
uint8_t psx_get_devicemode(void);       // get device mode (digital/analog/pressures)
uint8_t psx_get_rumble(uint16_t *rum);  // rumble0 is MSB, rumble1 is LSB
                                        //  return val indicates if data is new

#define PSX_MODE_DIGITAL        (0x41)
#define PSX_MODE_ANALOG         (0x73)
#define PSX_MODE_PRESSURES      (0x79)
#define PSX_MODE_CONFIG         (0xF3)


#ifdef PSX_PRIVATE_INCLUDE
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

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

#endif

#endif
