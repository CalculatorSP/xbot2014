#pragma once

#include "PlaystationController.h"

namespace XboxButton
{
    enum Button
    {
        A,
        B,
        X,
        Y,
        UP,
        DOWN,
        LEFT,
        RIGHT,
        LEFT_BUMPER,
        RIGHT_BUMPER,
        LEFT_STICK_CLICK,
        RIGHT_STICK_CLICK,
        START,
        BACK,
        LEFT_TRIGGER,
        RIGHT_TRIGGER,
        LEFT_STICK_LEFT,
        LEFT_STICK_RIGHT,
        LEFT_STICK_UP,
        LEFT_STICK_DOWN,
        RIGHT_STICK_LEFT,
        RIGHT_STICK_RIGHT,
        RIGHT_STICK_UP,
        RIGHT_STICK_DOWN
    };
}

namespace XboxAnalog
{
    enum Analog
    {
        LEFT_TRIGGER,
        RIGHT_TRIGGER,
        LEFT_STICK_X,
        LEFT_STICK_Y,
        RIGHT_STICK_X,
        RIGHT_STICK_Y,
    };
}

class XboxController
{
    PlaystationController _playstationController;

public:

    // Sets up communication with the controller (Teensy) on serial port comport
    XboxController(const char *comport);

    // Checks if serial port has been opened successfully
    bool isConnected() const;

    // Sets button to specified value. Must manually call sendState() to see the effect
    void set(XboxButton::Button button, bool pressed);

    // Sets analog to specified value. Must manually call sendState() to see the effect
    void set(XboxAnalog::Analog analog, float amount);

    // Releases all buttons/analogs
    void reset();

    // Updates the controller to reflect the current state
    void sendState(int framecount = 3);

    // Presses specified button. Must manually call sendState() to see the effect
    inline void press(XboxButton::Button button)
    {
        set(button, true);
    }

    // Releases specified button. Must manually call sendState() to see the effect
    inline void release(XboxButton::Button button)
    {
        set(button, false);
    }

    // Releases specified analog. Must manually call sendState() to see the effect
    inline void release(XboxAnalog::Analog analog)
    {
        set(analog, 0.0f);
    }

    // Quickly press and release specified button for a set number of frames
    //  No need to call sendState()
    inline void tap(XboxButton::Button button, int framecount = 3)
    {
        set(button, true);
        sendState(framecount);
        set(button, false);
        sendState(framecount);
    }

    // Quickly press and release specified analog for a set number of frames
    //  No need to call sendState()
    inline void tap(XboxAnalog::Analog analog, float amount, int framecount = 3)
    {
        set(analog, amount);
        sendState(framecount);
        set(analog, 0.0f);
        sendState(framecount);
    }
};
