#pragma once

#include "PlaystationController.h"

class XboxController
{
	PlaystationController _playstationController;

public:
	enum XboxButton
	{
		A,
		B,
		X,
		Y,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		LB,
		RB,
		LS,
		RS,
		START,
		BACK,
		LT_D,
		RT_D,
		LX_L,
		LX_R,
		LY_U,
		LY_D,
		RX_L,
		RX_R,
		RY_U,
		RY_D
	};

	enum XboxAnalog
	{
		LT,
		RT,
		LX,
		LY,
		RX,
		RY,
	};

	// Sets up communication with the controller (Teensy) on serial port comport
	XboxController(const char *comport);

	// Presses specified button. Must manually call sendState() to see the effect
	void press(XboxButton button);

	// Releases specified button/analog. Must manually call sendState() to see the effect
	void release(XboxButton button);
	void release(XboxAnalog analog);

	// Sets button/analog to specified value. Must manually call sendState() to see the effect
	void set(XboxButton button, bool pressed);
	void set(XboxAnalog analog, float amount);

	// Quickly press and release specified button/analog for a set number of frames
	//  No need to call sendState()
	void tap(XboxButton button, int framecount);
	void tap(XboxAnalog analog, float amount, int framecount);

	// Releases all buttons/analogs
	void reset();

	// Updates the controller to reflect the current state
	void sendState(int framecount) const;

};