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

	// Sets button to specified value. Must manually call sendState() to see the effect
	void set(XboxButton button, bool pressed);

	// Sets analog to specified value. Must manually call sendState() to see the effect
	void set(XboxAnalog analog, float amount);

	// Releases all buttons/analogs
	void reset();

	// Updates the controller to reflect the current state
	void sendState(int framecount = 4) const;

	// Presses specified button. Must manually call sendState() to see the effect
	inline void press(XboxButton button)
	{
		set(button, true);
	}

	// Releases specified button. Must manually call sendState() to see the effect
	inline void release(XboxButton button)
	{
		set(button, false);
	}

	// Releases specified analog. Must manually call sendState() to see the effect
	inline void release(XboxAnalog analog)
	{
		set(analog, 0.0f);
	}

	// Quickly press and release specified button for a set number of frames
	//  No need to call sendState()
	inline void tap(XboxButton button, int framecount = 4)
	{
		set(button, true);
		sendState(framecount);
		set(button, false);
		sendState(framecount);
	}

	// Quickly press and release specified analog for a set number of frames
	//  No need to call sendState()
	inline void tap(XboxAnalog analog, float amount, int framecount = 4)
	{
		set(analog, amount);
		sendState(framecount);
		set(analog, 0.0f);
		sendState(framecount);
	}
};