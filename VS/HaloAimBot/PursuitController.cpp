#include "MotionModel.h"
#include "PursuitController.h"

#define MAX_FRAMES_IN_FUTURE    (5)
#define AIM_THRESHOLD           (5.0f)

PursuitController::PursuitController(Scheduler* scheduler, XboxController* xboxController, MotionTracker* motionTracker)
    :
    _scheduler(scheduler),
    _xboxController(xboxController),
    _motionTracker(motionTracker)
{ }

PursuitController::~PursuitController()
{
    reset();
}

bool PursuitController::startPursuing(Point2f target)
{
    reset();
    return updateTarget(target);
}

bool PursuitController::updateTarget(Point2f newTarget)
{
    if (hypotf(newTarget.x, newTarget.y) < AIM_THRESHOLD)
    {
        _xboxController->tap(XboxButton::RIGHT_TRIGGER);
        return false;
    }

    _targetHistory.deposit(newTarget);

    Point2f intersection(0.0f, 0.0f);
    int framesInFuture = 0;
    Point2f joystickVals(0.0f, 0.0f);
    if (_findIntersectionPoint(intersection, framesInFuture, joystickVals))
    {
        _motionTracker->joystickVals = joystickVals;
        _xboxController->set(XboxAnalog::RIGHT_STICK_X, joystickVals.x);
        _xboxController->set(XboxAnalog::RIGHT_STICK_Y, joystickVals.y);
        _xboxController->sendState();
        return true;
    }

    return false;
}

void PursuitController::reset()
{
    _xboxController->reset();
    _xboxController->sendState();

    _targetHistory.reset();
}

bool PursuitController::_findIntersectionPoint(Point2f& intersection, int& framesInFuture, Point2f& joystickVals)
{
    Point2f firstTerm(0.0f, 0.0f);
    Point2f secondTerm(0.0f, 0.0f);

    switch (_targetHistory.getCount())
    {
    case 3:
        secondTerm = _targetHistory[-1] - 2*_targetHistory[-2] + _targetHistory[-3];

    case 2:
        firstTerm = _targetHistory[-1] - _targetHistory[-2];

    case 1:
        intersection = _targetHistory[-1];
        break;

    default:
        return false;
    }

    for (framesInFuture = 1; framesInFuture < MAX_FRAMES_IN_FUTURE; ++framesInFuture)
    {
        firstTerm += secondTerm;
        intersection += firstTerm;

        if (_iCanHit(intersection, framesInFuture, joystickVals))
            return true;
    }

    return false;
}

bool PursuitController::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals)
{
    if (framesInFuture <= 0)
        return false;

    joystickVals = MotionModel::getJoystickVals(1.0f / framesInFuture * intersection);

    return hypotf(joystickVals.x, joystickVals.y) < 0.9f;
}