#include "PlaystationController.h"

const PlaystationPacket_t PlaystationController::defaultPacket =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PlaystationController::PlaystationController(const char *comport) : _serialPort(comport)
{	
	reset();
}

void PlaystationController::reset()
{
	state.packet = defaultPacket;

	_serialPort.flush();
}

void PlaystationController::sendState(int framecount) const
{
	char serialString[PACKET_SIZE];
	char miniString[3];

	serialString[0] = 'c';	// Controller update instruction
	serialString[1] = '0' + framecount;	// Hold for specified number of frames
	for (int i = 0; i < sizeof(state.packet); i++)
	{
		sprintf_s(miniString, 3, "%02hhx", state.packet.arr[i]);
		serialString[2 * i + 2] = miniString[0];
		serialString[2 * i + 3] = miniString[1];
	}
	serialString[PACKET_SIZE - 1] = '\n';	// Separator

	_serialPort.write((uint8_t *)serialString, sizeof(serialString));
}