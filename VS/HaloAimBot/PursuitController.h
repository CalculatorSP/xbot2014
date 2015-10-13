#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"

using namespace cv;

class PursuitController
{
    Scheduler*                  _scheduler;
    XboxController*             _xboxController;
    KalmanFilter                _kalmanFilter;

public:
    PursuitController(Scheduler* scheduler, XboxController* xboxController);
    ~PursuitController();

    Mat_<float>                 _control;
    Mat_<float>                 _curPrediction;
    Mat_<float>                 _curEstimate;

    bool startPursuing(Point2f target);
    bool updateWithTarget(Point2f newTarget);
    bool updateWithoutTarget();
    void reset();

};
