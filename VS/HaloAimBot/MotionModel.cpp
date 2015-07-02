#include "MotionModel.h"

namespace MotionModel
{
    Point2f getRotationRate(Point2f joystickVals)
    {
        float mag = _getRotationMagnitude(hypotf(joystickVals.x, joystickVals.y));
        Point2f rotationRate(mag / hypotf(1.0f, joystickVals.x / joystickVals.y), mag / hypotf(1.0f, joystickVals.y / joystickVals.x));

        if (joystickVals.x < 0.0f)
            rotationRate.x *= -1.0f;
        if (joystickVals.y < 0.0f)
            rotationRate.y *= -1.0f;

        return rotationRate;
    }

    Point2f getJoystickVals(Point2f rotationRate)
    {
        float mag = _getRotationMagnitude.inverse(hypotf(rotationRate.x, rotationRate.y), 0.0f, 0.5f);
        Point2f joystickVals(mag / hypotf(1.0f, rotationRate.x / rotationRate.y), mag / hypotf(1.0f, rotationRate.y / rotationRate.x));

        if (rotationRate.x < 0.0f)
            joystickVals.x *= -1.0f;
        if (rotationRate.y < 0.0f)
            joystickVals.y *= -1.0f;

        return joystickVals;
    }
}
