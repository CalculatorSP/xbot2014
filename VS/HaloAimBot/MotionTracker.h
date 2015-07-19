#pragma once

#include "Collections/ModBuffer.h"
#include "opencv2/opencv.hpp"

#define MOTION_HISTORY_SIZE (4)

using namespace cv;

class MotionTracker
{
public:
    ModBuffer<Point2f, MOTION_HISTORY_SIZE> motionHistory;
    Point2f joystickVals;

    MotionTracker();
    void update();
};
