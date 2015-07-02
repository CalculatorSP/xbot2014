#pragma once

#include "opencv2/opencv.hpp"

#include "Collections/ModBuffer.h"
#include "PSX/XboxController.h"
#include "Task/Scheduler.h"

using namespace cv;

class PursuitController
{
    Scheduler*              _scheduler;
    XboxController*         _xboxController;
    ModBuffer<Point2f, 3>   _targetHistory;

public:
    PursuitController(Scheduler* scheduler, XboxController* xboxController);
    ~PursuitController();

    void startPursuing(Point2f target);
    void stop();

};
