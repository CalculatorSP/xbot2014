#include "PlaystationController.h"

const uint8_t PlaystationController::defaultPacket[18] =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PlaystationController::PlaystationController(const char *comport) : _serialPort(comport)
{	
	std::copy(std::begin(defaultPacket), std::end(defaultPacket), std::begin(state.packet));

	_serialPort.flush();
}

void PlaystationController::sendState(int framecount) const
{
	char serialString[PACKET_SIZE];

	serialString[0] = 'c';	// Controller update instruction
	serialString[1] = '0' + framecount;	// Hold for specified number of frames
	for (int i = 0; i < sizeof(state.packet); i++)
		sprintf(serialString + ((i + 1) << 1), "%02hhx", state.packet[i]);
	serialString[PACKET_SIZE - 1] = '\n';	// Separator

	_serialPort.write((uint8_t *)serialString, sizeof(serialString));
}