#include "Serial.h"

#include <stdint.h>

#define PACKET_SIZE (39)	// Includes header and separator

typedef struct
{
	uint8_t b_SELECT : 1;
	uint8_t b_L3 : 1;
	uint8_t b_R3 : 1;
	uint8_t b_START : 1;
	uint8_t b_UP : 1;
	uint8_t b_RIGHT : 1;
	uint8_t b_DOWN : 1;
	uint8_t b_LEFT : 1;

	uint8_t b_L2 : 1;
	uint8_t b_R2 : 1;
	uint8_t b_L1 : 1;
	uint8_t b_R1 : 1;
	uint8_t b_TRIANGLE : 1;
	uint8_t b_CIRCLE : 1;
	uint8_t b_CROSS : 1;
	uint8_t b_SQUARE : 1;

	uint8_t joyRX;
	uint8_t joyRY;
	uint8_t joyLX;
	uint8_t joyLY;

	uint8_t p_RIGHT;
	uint8_t p_LEFT;
	uint8_t p_UP;
	uint8_t p_DOWN;
	uint8_t p_TRIANGLE;
	uint8_t p_CIRCLE;
	uint8_t p_CROSS;
	uint8_t p_SQUARE;
	uint8_t p_L1;
	uint8_t p_R1;
	uint8_t p_L2;
	uint8_t p_R2;
} PlaystationButtons_t;

class PlaystationController
{
public:
	static const uint8_t defaultPacket[18];

	union
	{
		uint8_t packet[18];
		PlaystationButtons_t physical;
	} state;

	PlaystationController(const char *comport);

	void reset();
	void sendState(int framecount) const;

private:
	Serial _serialPort;

};