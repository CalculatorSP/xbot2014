#include "XboxController.h"

XboxController::XboxController(const char *comport)
{
    _playstationController.connect(comport);
    reset();
}

bool XboxController::isConnected() const
{
    return _playstationController.isConnected();
}

void XboxController::set(XboxButton::Button button, bool pressed)
{
    switch (button)
    {
    case XboxButton::A:                 _playstationController.state.physical.b_CROSS = !pressed; return;
    case XboxButton::B:                 _playstationController.state.physical.b_CIRCLE = !pressed; return;
    case XboxButton::X:                 _playstationController.state.physical.b_SQUARE = !pressed; return;
    case XboxButton::Y:                 _playstationController.state.physical.b_TRIANGLE = !pressed; return;
    case XboxButton::UP:                _playstationController.state.physical.b_UP = !pressed; return;
    case XboxButton::DOWN:              _playstationController.state.physical.b_DOWN = !pressed; return;
    case XboxButton::LEFT:              _playstationController.state.physical.b_LEFT = !pressed; return;
    case XboxButton::RIGHT:             _playstationController.state.physical.b_RIGHT = !pressed; return;
    case XboxButton::LEFT_BUMPER:       _playstationController.state.physical.b_L1 = !pressed; return;
    case XboxButton::RIGHT_BUMPER:      _playstationController.state.physical.b_R1 = !pressed; return;
    case XboxButton::LEFT_STICK_CLICK:  _playstationController.state.physical.b_L3 = !pressed; return;
    case XboxButton::RIGHT_STICK_CLICK: _playstationController.state.physical.b_R3 = !pressed; return;
    case XboxButton::START:             _playstationController.state.physical.b_START = !pressed; return;
    case XboxButton::BACK:              _playstationController.state.physical.b_SELECT = !pressed; return;

    case XboxButton::LEFT_TRIGGER:      _playstationController.state.physical.p_L2 = pressed ? 0xFF : 0x00; return;
    case XboxButton::RIGHT_TRIGGER:     _playstationController.state.physical.p_R2 = pressed ? 0xFF : 0x00; return;

    case XboxButton::LEFT_STICK_LEFT:   _playstationController.state.physical.joyLX = pressed ? 0x00 : 0x7F; return;
    case XboxButton::LEFT_STICK_RIGHT:  _playstationController.state.physical.joyLX = pressed ? 0xFF : 0x7F; return;
    case XboxButton::LEFT_STICK_UP:     _playstationController.state.physical.joyLY = pressed ? 0x00 : 0x7F; return;
    case XboxButton::LEFT_STICK_DOWN:   _playstationController.state.physical.joyLY = pressed ? 0xFF : 0x7F; return;
    case XboxButton::RIGHT_STICK_LEFT:  _playstationController.state.physical.joyRX = pressed ? 0x00 : 0x7F; return;
    case XboxButton::RIGHT_STICK_RIGHT: _playstationController.state.physical.joyRX = pressed ? 0xFF : 0x7F; return;
    case XboxButton::RIGHT_STICK_UP:    _playstationController.state.physical.joyRY = pressed ? 0x00 : 0x7F; return;
    case XboxButton::RIGHT_STICK_DOWN:  _playstationController.state.physical.joyRY = pressed ? 0xFF : 0x7F; return;

    default: return;
    }
}

void XboxController::set(XboxAnalog::Analog analog, float amount)
{
    switch (analog)
    {
    case XboxAnalog::LEFT_TRIGGER:  _playstationController.state.physical.p_L2 = (uint8_t)(amount * 0xFF); return;
    case XboxAnalog::RIGHT_TRIGGER: _playstationController.state.physical.p_R2 = (uint8_t)(amount * 0xFF); return;

    case XboxAnalog::LEFT_STICK_X:  _playstationController.state.physical.joyLX = (uint8_t)((amount + 1.0f) * 0x7F); return;
    case XboxAnalog::LEFT_STICK_Y:  _playstationController.state.physical.joyLY = (uint8_t)((amount + 1.0f) * 0x7F); return;
    case XboxAnalog::RIGHT_STICK_X: _playstationController.state.physical.joyRX = (uint8_t)((amount + 1.0f) * 0x7F); return;
    case XboxAnalog::RIGHT_STICK_Y: _playstationController.state.physical.joyRY = (uint8_t)((amount + 1.0f) * 0x7F); return;

    default: break;
    }
}

void XboxController::reset()
{
    _playstationController.reset();

    // Digital state of triggers will always be pressed. Value will be controlled
    //  by modifying button pressures
    _playstationController.state.physical.b_L2 = 0;
    _playstationController.state.physical.b_R2 = 0;
}

void XboxController::sendState(int framecount)
{
    //bool press = false;
    //if (_playstationController.state.physical.p_L2 == 0xFF)
    //{
    //	printf("G");
    //	press = true;
    //}
    //if (_playstationController.state.physical.b_L1 == 0)
    //{
    //	printf("R");
    //	press = true;
    //}	if (_playstationController.state.physical.b_R1 == 0)
    //{
    //	printf("Y");
    //	press = true;
    //}	if (_playstationController.state.physical.p_R2 == 0xFF)
    //{
    //	printf("B");
    //	press = true;
    //}	if (_playstationController.state.physical.b_CROSS == 0)
    //{
    //	printf("O");
    //	press = true;
    //}
    //if (press)
    //	printf("\n");

    // Send the data to the device
    _playstationController.sendState(framecount);
}