#include "MotionModel.h"

namespace MotionModel
{
    Point2f getRotationRate(Point2f joystickVals)
    {
        if (joystickVals.x == 0.0f && joystickVals.y == 0.0f)
            return Point2f(0.0f, 0.0f);

        float mag = _getRotationMagnitude(hypotf(joystickVals.x, joystickVals.y));

        Point2f rotationRate;

        if (joystickVals.x == 0.0f)
            rotationRate = Point2f(0.0f, mag);
        else if (joystickVals.y == 0.0f)
            rotationRate = Point2f(mag, 0.0f);
        else
            rotationRate = Point2f(mag / hypotf(1.0f, joystickVals.y / joystickVals.x), mag / hypotf(1.0f, joystickVals.x / joystickVals.y));

        // Y moves slower!
        rotationRate.y *= 0.5f;

        if (joystickVals.x < 0.0f)
            rotationRate.x *= -1.0f;
        if (joystickVals.y > 0.0f)
            rotationRate.y *= -1.0f;

        return rotationRate;
    }

    Point2f getJoystickVals(Point2f rotationRate)
    {
        if (rotationRate.x == 0.0f && rotationRate.y == 0.0f)
            return Point2f(0.0f, 0.0f);

        // Y moves slower!
        rotationRate.y *= 2.0f;

        float mag = _getRotationMagnitude.inverse(hypotf(rotationRate.x, rotationRate.y), 0.0f, 0.5f);
        
        Point2f joystickVals;

        if (rotationRate.x == 0.0f)
            joystickVals = Point2f(0.0f, mag);
        else if (rotationRate.y == 0.0f)
            joystickVals = Point2f(mag, 0.0f);
        else
            joystickVals = Point2f(mag / hypotf(1.0f, rotationRate.y / rotationRate.x), mag / hypotf(1.0f, rotationRate.x / rotationRate.y));

        if (rotationRate.x < 0.0f)
            joystickVals.x *= -1.0f;
        if (rotationRate.y > 0.0f)
            joystickVals.y *= -1.0f;

        return joystickVals;
    }
}
