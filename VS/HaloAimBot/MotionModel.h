#pragma once

#include "opencv2/opencv.hpp"

#include "Math/InvertibleFunction.h"

namespace MotionModel
{
    static class : public InvertibleFunction
    {
        float derivativeAt(float joyMag)
        {
            float x = joyMag;
            float x2 = x * x;

            return 581.49f*x2 - 302.78f*x + 49.809f;
        }

    public:
        float operator()(float joyMag)
        {
            float x = joyMag;
            float x2 = x * x;
            float x3 = x2 * x;

            return 193.83f*x3 - 151.39f*x2 + 49.809f*x - 5.2601f;
        }
    } _getRotationMagnitude;

    cv::Point2f getRotationRate(cv::Point2f joystickVals);
    cv::Point2f getJoystickVals(cv::Point2f rotationRate);
}
