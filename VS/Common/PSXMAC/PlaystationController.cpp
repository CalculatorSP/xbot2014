#include "PlaystationController.h"
#include <cstdio>
const PlaystationPacket_t PlaystationController::defaultPacket =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PlaystationController::PlaystationController(const char *comport)
{
    serialFile = fopen("/dev/cu.usbmodem12341", "r+");
	reset();
}

void PlaystationController::reset()
{
	state.packet = defaultPacket;
}

void PlaystationController::sendState(int framecount) const
{
	char serialString[PACKET_SIZE];
	char miniString[3];

	serialString[0] = 'c';	// Controller update instruction
	serialString[1] = '0' + framecount;	// Hold for specified number of frames
	for (int i = 0; i < sizeof(state.packet); i++)
	{
		snprintf(miniString, 3, "%02hhx", state.packet.arr[i]);
		serialString[2 * i + 2] = miniString[0];
		serialString[2 * i + 3] = miniString[1];
	}
	serialString[PACKET_SIZE - 1] = '\n';	// Separator

	fwrite((uint8_t *)serialString, 1, PACKET_SIZE, serialFile);
}