#include "XboxController.h"

XboxController::XboxController(const char *comport) : _playstationController(comport)
{ 
	reset();
}

void XboxController::set(XboxButton button, bool pressed)
{
	switch (button)
	{
	case A:		_playstationController.state.physical.b_CROSS	 = !pressed; break;
	case B:		_playstationController.state.physical.b_CIRCLE	 = !pressed; break;
	case X:		_playstationController.state.physical.b_SQUARE	 = !pressed; break;
	case Y:		_playstationController.state.physical.b_TRIANGLE = !pressed; break;
	case UP:	_playstationController.state.physical.b_UP		 = !pressed; break;
	case DOWN:	_playstationController.state.physical.b_DOWN	 = !pressed; break;
	case LEFT:	_playstationController.state.physical.b_LEFT	 = !pressed; break;
	case RIGHT:	_playstationController.state.physical.b_RIGHT	 = !pressed; break;
	case LB:	_playstationController.state.physical.b_L1		 = !pressed; break;
	case RB:	_playstationController.state.physical.b_R1		 = !pressed; break;
	case LS:	_playstationController.state.physical.b_L3		 = !pressed; break;
	case RS:	_playstationController.state.physical.b_R3		 = !pressed; break;
	case START:	_playstationController.state.physical.b_START	 = !pressed; break;
	case BACK:	_playstationController.state.physical.b_SELECT	 = !pressed; break;

	case LT_D:	_playstationController.state.physical.p_L2		 = pressed ? 0xFF : 0x00; break;
	case RT_D:	_playstationController.state.physical.p_R2		 = pressed ? 0xFF : 0x00; break;

	case LX_L:	_playstationController.state.physical.joyLX		 = pressed ? 0x00 : 0x7F; break;
	case LX_R:	_playstationController.state.physical.joyLX		 = pressed ? 0xFF : 0x7F; break;
	case LY_U:	_playstationController.state.physical.joyLY		 = pressed ? 0x00 : 0x7F; break;
	case LY_D:	_playstationController.state.physical.joyLY		 = pressed ? 0xFF : 0x7F; break;
	case RX_L:	_playstationController.state.physical.joyRX		 = pressed ? 0x00 : 0x7F; break;
	case RX_R:	_playstationController.state.physical.joyRX		 = pressed ? 0xFF : 0x7F; break;
	case RY_U:	_playstationController.state.physical.joyRY		 = pressed ? 0x00 : 0x7F; break;
	case RY_D:	_playstationController.state.physical.joyRY		 = pressed ? 0xFF : 0x7F; break;

	default: break;
	}
}

void XboxController::set(XboxAnalog analog, float amount)
{
	switch (analog)
	{
	case LT: _playstationController.state.physical.p_L2  = (uint8_t)(amount * 0xFF); break;
	case RT: _playstationController.state.physical.p_R2  = (uint8_t)(amount * 0xFF); break;

	case LX: _playstationController.state.physical.joyLX = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case LY: _playstationController.state.physical.joyLY = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case RX: _playstationController.state.physical.joyRX = (uint8_t)((amount + 1.0f) * 0x7F); break;
	case RY: _playstationController.state.physical.joyRY = (uint8_t)((amount + 1.0f) * 0x7F); break;

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

void XboxController::sendState(int framecount) const
{
	// Send the data to the device
	_playstationController.sendState(framecount);
}