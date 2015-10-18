#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"

using namespace cv;

class PursuitController
{
    XboxController*             _xboxController;
    KalmanFilter                _kalmanFilter;

    Mat_<float>                 _control;
    Mat_<float>                 _curEstimate;

    bool _updateControl();
    static void _kalmanCopy(const KalmanFilter& src, KalmanFilter& dst);
    static bool PursuitController::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals);

public:
    PursuitController(XboxController* xboxController);
    ~PursuitController();

    bool startPursuing(Point2f target);
    bool updateWithTarget(Point2f newTarget);
    bool updateWithoutTarget();
    void reset();

};
