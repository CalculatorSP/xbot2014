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
    bool                        _tracking;

    void _startTracking(Point2f newTarget);
    void _updateControl(TargetTrackerOutput& out);
    static void _kalmanCopy(const KalmanFilter& src, KalmanFilter& dst);
    static bool _iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals);

public:
    TargetTracker();

    void trackWithTarget(Point2f newTarget, Point2f joystickVals, TargetTrackerOutput& out);
    void trackWithoutTarget(Point2f joystickVals, TargetTrackerOutput& out);
    void reset();
    bool hasTarget() const;
};
