#include "MotionModel.h"
#include "PursuitController.h"

PursuitController::PursuitController(Scheduler* scheduler, XboxController* xboxController)
    :
    _scheduler(scheduler),
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
    _kalmanFilter.statePre.at<float>(1) = target.y;
    return updateWithoutTarget();
}

bool PursuitController::updateWithTarget(Point2f newTarget)
{
    _curPrediction = _kalmanFilter.predict(_control);
    _curEstimate = _kalmanFilter.correct((Mat_<float>(2, 1) << newTarget.x, newTarget.y));

    std::cout << "measurement: " << (Mat_<float>(2, 1) << newTarget.x, newTarget.y) << std::endl;
    std::cout << "control: " << _control << std::endl;
    std::cout << "prediction: " << _curPrediction << std::endl;
    std::cout << "estimated: " << _curEstimate << std::endl << std::endl;

    return true;
}

bool PursuitController::updateWithoutTarget()
{
    _curPrediction = _kalmanFilter.predict(_control);

    return true;
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
