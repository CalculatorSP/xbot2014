#include "MotionModel.h"
#include "PursuitController.h"

#define HIT_THRESH  (5.0f)

PursuitController::PursuitController(XboxController* xboxController)
    :
    _xboxController(xboxController),
    _kalmanFilter()
{ }

PursuitController::~PursuitController()
{
    reset();
}

bool PursuitController::startPursuing(Point2f target)
{
    reset();
    _kalmanFilter.statePre.at<float>(0) = target.x;
    _kalmanFilter.statePre.at<float>(2) = target.x;
    _kalmanFilter.statePre.at<float>(4) = target.x;
    _kalmanFilter.statePre.at<float>(1) = target.y;
    _kalmanFilter.statePre.at<float>(3) = target.y;
    _kalmanFilter.statePre.at<float>(5) = target.y;

    _kalmanFilter.statePost.at<float>(0) = target.x;
    _kalmanFilter.statePost.at<float>(2) = target.x;
    _kalmanFilter.statePost.at<float>(4) = target.x;
    _kalmanFilter.statePost.at<float>(1) = target.y;
    _kalmanFilter.statePost.at<float>(3) = target.y;
    _kalmanFilter.statePost.at<float>(5) = target.y;
    return updateWithoutTarget();
}

bool PursuitController::updateWithTarget(Point2f newTarget)
{
    _kalmanFilter.predict(_control);
    _curEstimate = _kalmanFilter.correct((Mat_<float>(2, 1) << newTarget.x, newTarget.y));

    return _updateControl();
}

bool PursuitController::updateWithoutTarget()
{
    _curEstimate = _kalmanFilter.predict(_control);

    _kalmanFilter.statePre.copyTo(_kalmanFilter.statePost);
    _kalmanFilter.errorCovPre.copyTo(_kalmanFilter.errorCovPost);

    return _updateControl();
}

void PursuitController::reset()
{
    _xboxController->reset();
    _xboxController->sendState();

    _kalmanFilter.init(10, 2, 2, CV_32F);

    _kalmanFilter.transitionMatrix = *(Mat_<float>(10, 10) <<
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 0, 1, 0, 0, 0,
        0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 1);

    _kalmanFilter.controlMatrix = *(Mat_<float>(10, 2) <<
        0, 0,
        0, 0,
        0, 0,
        0, 0,
        -1, 0,
        0, -1,
        0, 0,
        0, 0,
        0, 0,
        0, 0);

    _control = Mat::zeros(2, 1, CV_32F);

    _kalmanFilter.statePre.setTo(Scalar(0));
    setIdentity(_kalmanFilter.measurementMatrix);
    setIdentity(_kalmanFilter.processNoiseCov, Scalar::all(0));
    _kalmanFilter.processNoiseCov.at<float>(6, 6) = 1;
    _kalmanFilter.processNoiseCov.at<float>(7, 7) = 1;
    _kalmanFilter.processNoiseCov.at<float>(8, 8) = 3000;
    _kalmanFilter.processNoiseCov.at<float>(9, 9) = 3000;
    setIdentity(_kalmanFilter.measurementNoiseCov, Scalar::all(1));
    setIdentity(_kalmanFilter.errorCovPost, Scalar::all(3000));
}

bool PursuitController::_updateControl()
{
    // Copy kalman filter so we don't mess up our current state
    KalmanFilter tmpKalman;
    _kalmanCopy(_kalmanFilter, tmpKalman);

    // Start by checking current location
    Mat_<float> curState(_curEstimate);
    std::cout << "starting at: " << _curEstimate << std::endl;
    for (int i = 0; i < 10; ++i)
    {
        Point2f curTarget(curState.at<float>(4), curState.at<float>(5));
        Point2f joystickVals;
        std:: cout << "trying: " << curState << std::endl;
        if (_iCanHit(curTarget, i, joystickVals))
        {
            if (i == 0)
            {
                // Fire in the hole!
                printf("ENGAGING TARGET\n");
                _xboxController->tap(XboxButton::RIGHT_TRIGGER);
                _xboxController->set(XboxAnalog::RIGHT_STICK_X, 0.0f);
                _xboxController->set(XboxAnalog::RIGHT_STICK_Y, 0.0f);
                _xboxController->sendState(2);
                return false;
            }

            printf("INTERSECTION IN %d\n", i);

            // Set controls for Kalman filter
            Point2f pixelMovement = MotionModel::getRotationRate(joystickVals);
            _control.at<float>(0) = pixelMovement.x;
            _control.at<float>(1) = pixelMovement.y;

            // Tilt joystick towards target
            _xboxController->set(XboxAnalog::RIGHT_STICK_X, joystickVals.x);
            _xboxController->set(XboxAnalog::RIGHT_STICK_Y, joystickVals.y);
            _xboxController->sendState(2);

            return true;
        }

        // Predict with no control. We will calculate control based on result of this loop
        curState = tmpKalman.predict();
        tmpKalman.statePre.copyTo(tmpKalman.statePost);
        tmpKalman.errorCovPre.copyTo(tmpKalman.errorCovPost);
    }

    return false;
}

void PursuitController::_kalmanCopy(const KalmanFilter& src, KalmanFilter& dst)
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

bool PursuitController::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals)
{
    if (framesInFuture < 0)
        return false;

    if (framesInFuture == 0)
        return hypotf(intersection.x, intersection.y) < HIT_THRESH;

    joystickVals = MotionModel::getJoystickVals(1.0f / framesInFuture * intersection);
    printf("req: %f, %f\n", joystickVals.x, joystickVals.y);
    return hypotf(joystickVals.x, joystickVals.y) < 0.9f;
}
