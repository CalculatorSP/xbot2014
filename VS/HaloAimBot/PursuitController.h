#pragma once

#include "opencv2/opencv.hpp"

#include "Collections/ModBuffer.h"
#include "MotionTracker.h"
#include "PSX/XboxController.h"
#include "Task/Scheduler.h"

using namespace cv;

class PursuitController
{
    Scheduler*              _scheduler;
    XboxController*         _xboxController;
    MotionTracker*           _motionTracker;
    ModBuffer<Point2f, 3>   _targetHistory;

    bool _findIntersectionPoint(Point2f& intersection, int& framesInFuture, Point2f& joystickVals);
    bool _iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals);

public:
    PursuitController(Scheduler* scheduler, XboxController* xboxController, MotionTracker* motionTracker);
    ~PursuitController();

    bool startPursuing(Point2f target);
    bool updateTarget(Point2f newTarget);
    void reset();

};
