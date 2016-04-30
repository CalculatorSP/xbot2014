#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

struct TargetTrackerOutput
{
    bool giveUp;
    bool pullTrigger;
    Point2f joystickVals;

    TargetTrackerOutput() : giveUp(false), pullTrigger(false), joystickVals(0.0f, 0.0f)
    { }
};

class TargetTracker
{
    KalmanFilter                _kalmanFilter;
    Mat_<float>                 _control;
    bool _tracking;

    void _updateControl(TargetTrackerOutput& out);
    static void _kalmanCopy(const KalmanFilter& src, KalmanFilter& dst);
    static bool _iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals);

public:
    TargetTracker();

    void trackWithTarget(Point2f newTarget, TargetTrackerOutput& out);
    void trackWithoutTarget(TargetTrackerOutput& out);
    void reset();

    bool hasTarget() const;
};
