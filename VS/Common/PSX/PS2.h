#include "Serial.h"

class PS2
{
private:
	Serial serialPort;
	unsigned char Buffer[7];

	void WriteToController();

public:
	unsigned int b_SELECT	: 1;
	unsigned int b_L3		: 1;
	unsigned int b_R3		: 1;
	unsigned int b_START	: 1;
	unsigned int b_UP		: 1;
	unsigned int b_RIGHT	: 1;
	unsigned int b_DOWN		: 1;
	unsigned int b_LEFT		: 1;
	
	unsigned int b_L2		: 1;
	unsigned int b_R2		: 1;
	unsigned int b_L1		: 1;
	unsigned int b_R1		: 1;
	unsigned int b_TRIANGLE : 1;
	unsigned int b_CIRCLE	: 1;
	unsigned int b_CROSS	: 1;
	unsigned int b_SQUARE	: 1;
	
	float joyRX;
	float joyRY;
	float joyLX;
	float joyLY;
	
	void UpdateController();
	
	PS2();  
};