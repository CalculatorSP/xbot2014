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

void XboxController::set(XboxButton button, bool pressed)
{
	switch (button)
	{
	case XboxButton::A:		_playstationController.state.physical.b_CROSS	 = !pressed; break;
	case XboxButton::B:		_playstationController.state.physical.b_CIRCLE	 = !pressed; break;
	case XboxButton::X:		_playstationController.state.physical.b_SQUARE	 = !pressed; break;
	case XboxButton::Y:		_playstationController.state.physical.b_TRIANGLE = !pressed; break;
	case XboxButton::UP:	_playstationController.state.physical.b_UP		 = !pressed; break;
	case XboxButton::DOWN:	_playstationController.state.physical.b_DOWN	 = !pressed; break;
	case XboxButton::LEFT:	_playstationController.state.physical.b_LEFT	 = !pressed; break;
	case XboxButton::RIGHT:	_playstationController.state.physical.b_RIGHT	 = !pressed; break;
	case XboxButton::LB:	_playstationController.state.physical.b_L1		 = !pressed; break;
	case XboxButton::RB:	_playstationController.state.physical.b_R1		 = !pressed; break;
	case XboxButton::LS:	_playstationController.state.physical.b_L3		 = !pressed; break;
	case XboxButton::RS:	_playstationController.state.physical.b_R3		 = !pressed; break;
	case XboxButton::START:	_playstationController.state.physical.b_START	 = !pressed; break;
	case XboxButton::BACK:	_playstationController.state.physical.b_SELECT	 = !pressed; break;

	case XboxButton::LT_D:	_playstationController.state.physical.p_L2		 = pressed ? 0xFF : 0x00; break;
	case XboxButton::RT_D:	_playstationController.state.physical.p_R2		 = pressed ? 0xFF : 0x00; break;

	case XboxButton::LX_L:	_playstationController.state.physical.joyLX		 = pressed ? 0x00 : 0x7F; break;
	case XboxButton::LX_R:	_playstationController.state.physical.joyLX		 = pressed ? 0xFF : 0x7F; break;
	case XboxButton::LY_U:	_playstationController.state.physical.joyLY		 = pressed ? 0x00 : 0x7F; break;
	case XboxButton::LY_D:	_playstationController.state.physical.joyLY		 = pressed ? 0xFF : 0x7F; break;
	case XboxButton::RX_L:	_playstationController.state.physical.joyRX		 = pressed ? 0x00 : 0x7F; break;
	case XboxButton::RX_R:	_playstationController.state.physical.joyRX		 = pressed ? 0xFF : 0x7F; break;
	case XboxButton::RY_U:	_playstationController.state.physical.joyRY		 = pressed ? 0x00 : 0x7F; break;
	case XboxButton::RY_D:	_playstationController.state.physical.joyRY		 = pressed ? 0xFF : 0x7F; break;

	default: break;
	}
}

void XboxController::set(XboxAnalog analog, float amount)
{
	switch (analog)
	{
	case XboxAnalog::LT: _playstationController.state.physical.p_L2  = (uint8_t)(amount * 0xFF); break;
	case XboxAnalog::RT: _playstationController.state.physical.p_R2  = (uint8_t)(amount * 0xFF); break;

	case XboxAnalog::LX: _playstationController.state.physical.joyLX = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case XboxAnalog::LY: _playstationController.state.physical.joyLY = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case XboxAnalog::RX: _playstationController.state.physical.joyRX = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case XboxAnalog::RY: _playstationController.state.physical.joyRY = (uint8_t)((amount + 1.0f) * 0x7F); break;

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