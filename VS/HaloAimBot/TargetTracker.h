#pragma once

#include "opencv2/opencv.hpp"

struct TargetTrackerOutput
{
    bool giveUp;
    bool pullTrigger;
    cv::Point2f joystickVals;

    TargetTrackerOutput() : giveUp(false), pullTrigger(false), joystickVals(0.0f, 0.0f)
    { }
};

class TargetTracker
{
    cv::KalmanFilter            _kalmanFilter;
    bool                        _tracking;

    void _startTracking(cv::Point2f newTarget);
    void _updateControl(TargetTrackerOutput& out);
    static void _kalmanCopy(const cv::KalmanFilter& src, cv::KalmanFilter& dst);
    static bool _iCanHit(cv::Point2f intersection, int framesInFuture, cv::Point2f& joystickVals);

public:
    TargetTracker();

    void trackWithTarget(cv::Point2f newTarget, cv::Point2f joystickVals, TargetTrackerOutput& out);
    void trackWithoutTarget(cv::Point2f joystickVals, TargetTrackerOutput& out);
    void reset();
    bool hasTarget() const;
};
