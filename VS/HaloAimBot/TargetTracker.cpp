#include "MotionModel.h"
#include "TargetTracker.h"

#define HIT_THRESH  (5.0f)

TargetTracker::TargetTracker()
{
    reset();
}

void TargetTracker::trackWithTarget(Point2f newTarget, TargetTrackerOutput& out)
{
    std::cout << "Target: " << newTarget << std::endl;
    if (!_tracking)
    {
        _startPursuing(newTarget);
    }
    else
    {
        _kalmanFilter.predict(_control);
        _curEstimate = _kalmanFilter.correct((Mat_<float>(2, 1) << newTarget.x, newTarget.y));
    }

    _updateControl(out);
}

void TargetTracker::trackWithoutTarget(TargetTrackerOutput& out)
{
    if (!_tracking)
    {
        // No target
        out.joystickVals.x = 0.0f;
        out.joystickVals.y = 0.0f;
        out.pullTrigger = false;
        out.giveUp = true;
        return;
    }

    _curEstimate = _kalmanFilter.predict(_control);

    _kalmanFilter.statePre.copyTo(_kalmanFilter.statePost);
    _kalmanFilter.errorCovPre.copyTo(_kalmanFilter.errorCovPost);

    _updateControl(out);
}

void TargetTracker::reset()
{
    _tracking = false;
    _kalmanFilter.init(10, 2, 2, CV_32F);

    _kalmanFilter.transitionMatrix = *(Mat_<float>(10, 10) <<
        1, 0, 1, 0, 0, 0, 1, 0, 0, 0,
        0, 1, 0, 1, 0, 0, 0, 1, 0, 0,
        0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    _kalmanFilter.controlMatrix = *(Mat_<float>(10, 2) <<
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

    _control = Mat::zeros(2, 1, CV_32F);
    _curEstimate.setTo(Scalar(0));

    _kalmanFilter.statePre.setTo(Scalar(0));
    setIdentity(_kalmanFilter.measurementMatrix);
    setIdentity(_kalmanFilter.processNoiseCov, Scalar::all(0));
    _kalmanFilter.processNoiseCov.at<float>(2, 2) = 1;
    _kalmanFilter.processNoiseCov.at<float>(3, 3) = 1;
    _kalmanFilter.processNoiseCov.at<float>(4, 4) = 3000;
    _kalmanFilter.processNoiseCov.at<float>(5, 5) = 3000;
    setIdentity(_kalmanFilter.measurementNoiseCov, Scalar::all(1));
    setIdentity(_kalmanFilter.errorCovPre, Scalar::all(3000));
    setIdentity(_kalmanFilter.errorCovPost, Scalar::all(3000));
}

void TargetTracker::_startPursuing(Point2f target)
{
    _kalmanFilter.statePre.at<float>(0) = target.x;
    _kalmanFilter.statePre.at<float>(1) = target.y;

    _kalmanFilter.statePost.at<float>(0) = target.x;
    _kalmanFilter.statePost.at<float>(1) = target.y;

    _kalmanFilter.statePre.copyTo(_curEstimate);

    _tracking = true;
}

void TargetTracker::_updateControl(TargetTrackerOutput& out)
{
    // Copy kalman filter so we don't mess up our current state
    KalmanFilter tmpKalman;
    _kalmanCopy(_kalmanFilter, tmpKalman);

    // Start by checking current location
    Mat_<float> curState(_curEstimate);
    for (int i = -2; i < 10; ++i)
    {
        Point2f curTarget(curState.at<float>(0), curState.at<float>(1));
        Point2f joystickVals;
        std::cout << "trying (" << i << "): " << std::endl << _kalmanFilter.measurementNoiseCov << std::endl;
        if (_iCanHit(curTarget, i, joystickVals))
        {
            printf("INTERSECTION IN %d\n", i);

            if (i == 0)
            {
                // Pull trigger
                out.joystickVals = joystickVals;
                out.giveUp = false;
                out.pullTrigger = true;
                return;
            }

            // Set controls for kalman filter
            Point2f pixelMovement = MotionModel::getRotationRate(joystickVals);
            _control.at<float>(0) = pixelMovement.x;
            _control.at<float>(1) = pixelMovement.y;

            // Tilt in appropriate direction
            out.joystickVals = joystickVals;
            out.giveUp = false;
            out.pullTrigger = false;
            return;
        }

        // Predict with no control. We will calculate control based on result of this loop
        curState = tmpKalman.predict();
        tmpKalman.statePre.copyTo(tmpKalman.statePost);
        tmpKalman.errorCovPre.copyTo(tmpKalman.errorCovPost);
    }

    // Lost target
    out.joystickVals.x = 0.0f;
    out.joystickVals.y = 0.0f;
    out.giveUp = true;
    out.pullTrigger = false;
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

bool TargetTracker::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals)
{
    if (framesInFuture < 0)
        return false;

    if (framesInFuture == 0)
        return hypotf(intersection.x, intersection.y) < HIT_THRESH;

    joystickVals = MotionModel::getJoystickVals(1.0f / framesInFuture * intersection);
    printf("req: %f, %f\n", joystickVals.x, joystickVals.y);
    return hypotf(joystickVals.x, joystickVals.y) < 0.9f;
}
