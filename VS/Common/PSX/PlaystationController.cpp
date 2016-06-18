#include "PlaystationController.h"

const PlaystationPacket_t PlaystationController::defaultPacket =
{ 0xFF, 0xFF, 0x7F, 0x7F, 0x7F, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

PlaystationController::PlaystationController() : _serialPort(NULL)
{	
    reset();
}

PlaystationController::~PlaystationController()
{
    disconnect();
}

bool PlaystationController::isConnected() const
{
    return _serialPort != NULL;
}

bool PlaystationController::connect(const char* comport)
{
    if (isConnected())
        return false;

    try
    {
        _serialPort = new Serial(comport);
    }
    catch (...)
    {
        _serialPort = NULL;
        return false;
    }

    _serialPort->flush();
    return true;
}

void PlaystationController::disconnect()
{
    if (!isConnected())
        return;

    delete _serialPort;
    _serialPort = NULL;
}

void PlaystationController::reset()
{
    state.packet = defaultPacket;
}

void PlaystationController::sendState(int framecount) const
{
    char serialString[PACKET_SIZE];
    char miniString[3];

    serialString[0] = 'c';                  // Controller update instruction
    serialString[1] = '0' + framecount;     // Hold for specified number of frames
    for (int i = 0; i < sizeof(state.packet); i++)
    {
        sprintf_s(miniString, 3, "%02hhx", state.packet.arr[i]);
        serialString[2 * i + 2] = miniString[0];
        serialString[2 * i + 3] = miniString[1];
    }
    serialString[PACKET_SIZE - 1] = '\n';   // Separator

    if (isConnected())
        _serialPort->write((uint8_t *)serialString, sizeof(serialString));
}