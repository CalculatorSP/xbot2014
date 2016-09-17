#include "MotionModel.h"
#include "TargetTracker.h"

#define HIT_THRESH  (5.0f)

using namespace cv;

TargetTracker::TargetTracker() : _tracking(false)
{ }

void TargetTracker::trackWithTarget(Point2f newTarget, Point2f joystickVals, TargetTrackerOutput& out, std::stringstream& dbg)
{
    dbg << "With target: " << newTarget << std::endl;

    if (_tracking)
    {
        int64 starttime = getTickCount();
        Point2f rotationRate = MotionModel::getRotationRate(joystickVals);
        Mat_<float> control = (Mat_<float>(2, 1) <<
            rotationRate.x,
            rotationRate.y);
        int64 endtime = getTickCount();

        dbg << "MotionModel: " << 1000.0 * (endtime - starttime) / getTickFrequency() << std::endl;

        starttime = getTickCount();
        _kalmanFilter.predict(control);
        endtime = getTickCount();

        dbg << "_kalmanFilter.predict(): " << 1000.0 * (endtime - starttime) / getTickFrequency() << std::endl;

        starttime = getTickCount();
        _kalmanFilter.correct((Mat_<float>(2, 1) << newTarget.x, newTarget.y));
        endtime = getTickCount();

        dbg << "_kalmanFilter.correct(): " << 1000.0 * (endtime - starttime) / getTickFrequency() << std::endl;
    }
    else
    {
        int64 starttime = getTickCount();
        _startTracking(newTarget);
        int64 endtime = getTickCount();

        dbg << "_startTracking: " << 1000.0 * (endtime - starttime) / getTickFrequency() << std::endl;
    }

    int64 starttime = getTickCount();
    _updateControl(out, dbg);
    int64 endtime = getTickCount();

    dbg << "_updateControl: " << 1000.0 * (endtime - starttime) / getTickFrequency() << std::endl;
}

void TargetTracker::trackWithoutTarget(Point2f joystickVals, TargetTrackerOutput& out, std::stringstream& dbg)
{
    dbg << "Without target" << std::endl;

    if (_tracking)
    {
        Point2f rotationRate = MotionModel::getRotationRate(joystickVals);
        Mat_<float> control = (Mat_<float>(2, 1) <<
            rotationRate.x,
            rotationRate.y);

        _kalmanFilter.predict(control);
        _kalmanFilter.statePre.copyTo(_kalmanFilter.statePost);
        _kalmanFilter.errorCovPre.copyTo(_kalmanFilter.errorCovPost);
        _updateControl(out, dbg);
    }
    else
    {
        // No target
        out.joystickVals.x = 0.0f;
        out.joystickVals.y = 0.0f;
        out.pullTrigger = false;
        out.giveUp = true;
    }
}

void TargetTracker::reset()
{
    _tracking = false;
}

bool TargetTracker::hasTarget() const
{
    return _tracking;
}

void TargetTracker::_startTracking(Point2f newTarget)
{
    _kalmanFilter.init(12, 2, 2, CV_32F);

    _kalmanFilter.transitionMatrix = (Mat_<float>(12, 12) <<
        1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    _kalmanFilter.processNoiseCov = (Mat_<float>(12, 12) <<
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    _kalmanFilter.controlMatrix = (Mat_<float>(12, 2) <<
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        -1, 0,
        0, -1);

    _kalmanFilter.measurementMatrix = (Mat_<float>(2, 12) <<
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    _kalmanFilter.measurementNoiseCov = (Mat_<float>(2, 2) <<
        0, 0,
        0, 0);

    _kalmanFilter.statePost = (Mat_<float>(12, 1) <<
        newTarget.x,
        newTarget.y,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0);

    _kalmanFilter.errorCovPost = (Mat_<float>(12, 12) <<
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1000, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    _tracking = true;
}

void TargetTracker::_updateControl(TargetTrackerOutput& out, std::stringstream& dbg)
{
    // Copy kalman filter so we don't mess up our current state
    KalmanFilter tmpKalman;
    _kalmanCopy(_kalmanFilter, tmpKalman);

    // Start by checking current location
    for (int i = -3; i < 10; ++i)
    {
        Point2f curTarget(tmpKalman.statePost.at<float>(0), tmpKalman.statePost.at<float>(1));
        Point2f joystickVals;
        dbg << "trying (" << i << "): " << std::endl << tmpKalman.statePost << std::endl;
        if (_iCanHit(curTarget, i, joystickVals, dbg))
        {
            dbg << "INTERSECTION IN " << i << std::endl;

            if (i == 0)
            {
                // Pull trigger
                out.joystickVals = joystickVals;
                out.giveUp = false;
                out.pullTrigger = true;
                _tracking = false;
                return;
            }

            // Set controls for kalman filter
            Point2f pixelMovement = MotionModel::getRotationRate(joystickVals);

            // Tilt in appropriate direction
            out.joystickVals = joystickVals;
            out.giveUp = false;
            out.pullTrigger = false;
            return;
        }

        // Predict with no control. We will calculate control based on result of this loop
        tmpKalman.predict();
        tmpKalman.statePre.copyTo(tmpKalman.statePost);
        tmpKalman.errorCovPre.copyTo(tmpKalman.errorCovPost);
    }

    // Lost target
    out.joystickVals.x = 0.0f;
    out.joystickVals.y = 0.0f;
    out.giveUp = true;
    out.pullTrigger = false;

    _tracking = false;
}

void TargetTracker::_kalmanCopy(const KalmanFilter& src, KalmanFilter& dst)
{
    src.statePre.copyTo(dst.statePre);
    src.statePost.copyTo(dst.statePost);
    src.transitionMatrix.copyTo(dst.transitionMatrix);
    src.controlMatrix.copyTo(dst.controlMatrix);
    src.measurementMatrix.copyTo(dst.measurementMatrix);
    src.processNoiseCov.copyTo(dst.processNoiseCov);
    src.measurementNoiseCov.copyTo(dst.measurementNoiseCov);
    src.errorCovPre.copyTo(dst.errorCovPre);
    src.gain.copyTo(dst.gain);
    src.errorCovPost.copyTo(dst.errorCovPost);
    src.temp1.copyTo(dst.temp1);
    src.temp2.copyTo(dst.temp2);
    src.temp3.copyTo(dst.temp3);
    src.temp4.copyTo(dst.temp4);
    src.temp5.copyTo(dst.temp5);
}

bool TargetTracker::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals, std::stringstream& dbg)
{
    if (framesInFuture < 0)
        return false;

    if (framesInFuture == 0)
        return hypotf(intersection.x, intersection.y) < HIT_THRESH;

    joystickVals = MotionModel::getJoystickVals(1.0f / framesInFuture * intersection);
    dbg << "req: " << joystickVals.x << ", " << joystickVals.y << std::endl;
    return hypotf(joystickVals.x, joystickVals.y) < 0.9f;
}
