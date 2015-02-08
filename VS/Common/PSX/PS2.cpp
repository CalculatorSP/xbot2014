#include "PS2.h"

PS2::PS2()
{
	b_SELECT	=1;
	b_L3		=1;
	b_R3		=1;
	b_START		=1;
	b_UP		=1;
	b_RIGHT		=1;
	b_DOWN		=1;
	b_LEFT		=1;
	
	b_L2		=1;
	b_R2		=1;
	b_L1		=1;
	b_R1		=1;
	b_TRIANGLE	=1;
	b_CIRCLE	=1;
	b_CROSS		=1;
	b_SQUARE	=1;
	
	joyRX		=0;
	joyRY		=0;
	joyLX		=0;
	joyLY		=0;

	serialPort.flush();
}

void PS2::WriteToController()
{
	serialPort.write(Buffer, 7);
}

void PS2::UpdateController()
{
	Buffer[0]=0x5A;
	Buffer[1]= (b_SELECT)|(b_L3<<1)|(b_R3<<2)|(b_START<<3)|(b_UP<<4)|(b_RIGHT<<5)|(b_DOWN<<6)|(b_LEFT<<7);
	Buffer[2]= (b_L2)|(b_R2<<1)|(b_L1<<2)|(b_R1<<3)|(b_TRIANGLE<<4)|(b_CIRCLE<<5)|(b_CROSS<<6)|(b_SQUARE<<7);
	Buffer[3]=(unsigned char) ((joyRX+1)*127);
	Buffer[4]=(unsigned char) ((joyRY+1)*127);
	Buffer[5]=(unsigned char) ((joyLX+1)*127);
	Buffer[6]=(unsigned char) ((joyLY+1)*127);
	WriteToController();
}