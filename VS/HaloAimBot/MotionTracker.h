#pragma once

#include "Collections/ModBuffer.h"
#include "MotionModel.h"
#include "opencv2/opencv.hpp"

using namespace cv;

template <int N>
class MotionTracker
{
public:
    ModBuffer<Point2f, N> motionHistory;
    Point2f joystickVals;

    MotionTracker()
        : joystickVals(0.0f, 0.0f)
    { }

    void update()
    {
        Point2f motionThisFrame = MotionModel::getRotationRate(joystickVals);
        motionHistory.deposit(Point2f(0.0f, 0.0f));
        for (int i = 0; i < N; ++i)
            motionHistory[i] += motionThisFrame;
    }
};
