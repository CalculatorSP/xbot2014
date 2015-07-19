#include "MotionModel.h"
#include "PursuitController.h"

#define MAX_FRAMES_IN_FUTURE    (10)
#define AIM_THRESHOLD           (5.0f)

PursuitController::PursuitController(Scheduler* scheduler, XboxController* xboxController, MotionTracker* motionTracker)
    :
    _scheduler(scheduler),
    _xboxController(xboxController),
    _motionTracker(motionTracker),
    _framesSinceTargetUpdate(0)
{ }

PursuitController::~PursuitController()
{
    reset();
}

bool PursuitController::startPursuing(Point2f target)
{
    reset();
    return updateWithTarget(target);
}

void PursuitController::updateTargetHistory(Point2f motion)
{
    // Update target history to reflect last frame's motion
    for (int i = 0; i < _targetHistory.getCount(); ++i)
        _targetHistory[-(i + 1)] -= motion;
}

bool PursuitController::updateWithTarget(Point2f newTarget)
{
    // Deposit the new measurement of the target
    _targetHistory.deposit(newTarget);
    _targetHistorySpacing.deposit(_framesSinceTargetUpdate);
    _framesSinceTargetUpdate = 0;

    // Proceed with the update
    return updateWithoutTarget();
}

bool PursuitController::updateWithoutTarget()
{
    // Track time between target updates
    ++_framesSinceTargetUpdate;

    for (int i = 0; i < _targetHistory.getCount(); ++i)
        printf("%d: (%f, %f)\n", -(i + 1), _targetHistory[-(i + 1)].x, _targetHistory[-(i + 1)].y);

    // Compute trajectory intersection
    Point2f intersection(0.0f, 0.0f);
    int framesInFuture = 0;
    Point2f joystickVals(0.0f, 0.0f);
    if (_findIntersectionPoint(intersection, framesInFuture, joystickVals))
    {
        // If our aim is close enough, pull the trigger!
        if (framesInFuture == 0)
        {
            printf("ENGAGING TARGET\n");
            _xboxController->tap(XboxButton::RIGHT_TRIGGER);
            return false;
        }

        _motionTracker->joystickVals = joystickVals;
        _xboxController->set(XboxAnalog::RIGHT_STICK_X, joystickVals.x);
        _xboxController->set(XboxAnalog::RIGHT_STICK_Y, joystickVals.y);
        _xboxController->sendState(2);
        return true;
    }

    return false;
}

void PursuitController::reset()
{
    _xboxController->reset();
    _xboxController->sendState();
    
    _motionTracker->joystickVals = Point2f(0.0f, 0.0f);

    _targetHistory.reset();
    _targetHistorySpacing.reset();
    _framesSinceTargetUpdate = 0;
}

bool PursuitController::_findIntersectionPoint(Point2f& intersection, int& framesInFuture, Point2f& joystickVals)
{
    printf("looking for intersection...\n");
    Point2f velocity;
    Point2f acceleration;

    switch (_targetHistory.getCount())
    {
    case 3:
    {
        float t1 = (float)_targetHistorySpacing[-2];
        float t2 = t1 + _targetHistorySpacing[-1];
        Point2f x0 = _targetHistory[-3];
        Point2f x1 = _targetHistory[-2];
        Point2f x2 = _targetHistory[-1];

        assert(t1 > 0.0f);
        assert(t2 > 0.0f);
        assert(t2 - t1 > 0.0f);

        acceleration = 2 / (t2 - t1) * (1 / t1 * (x1 - x0) - 1 / t2 * (x2 - x0));
        velocity = 0.5f * acceleration * t1 + 1 / t1 * (x1 - x0);
        intersection = x2;
        break;
    }

    case 2:
    {
        float t1 = (float)_targetHistorySpacing[-1];
        Point2f x0 = _targetHistory[-2];
        Point2f x1 = _targetHistory[-1];

        assert(t1 > 0.0f);

        acceleration = Point2f(0.0f, 0.0f);
        velocity = 1 / t1 * (x1 - x0);
        intersection = x1;
        break;
    }

    case 1:
    {
        Point2f x0 = _targetHistory[-1];

        acceleration = Point2f(0.0f, 0.0f);
        velocity = Point2f(0.0f, 0.0f);
        intersection = x0;
        break;
    }

    default:
        printf("no history\n");
        return false;
    }

    for (framesInFuture = -_framesSinceTargetUpdate + 1; framesInFuture < MAX_FRAMES_IN_FUTURE; ++framesInFuture)
    {
        if (_iCanHit(intersection, framesInFuture, joystickVals))
            return true;

        intersection += velocity + 0.5f * acceleration;
        velocity += acceleration;
    }

    return false;
}

bool PursuitController::_iCanHit(Point2f intersection, int framesInFuture, Point2f& joystickVals)
{
    if (framesInFuture < 0)
        return false;

    if (framesInFuture == 0)
        return hypotf(intersection.x, intersection.y) < AIM_THRESHOLD;

    joystickVals = MotionModel::getJoystickVals(1.0f / framesInFuture * intersection);
    printf("req: %f, %f\n", joystickVals.x, joystickVals.y);
    return hypotf(joystickVals.x, joystickVals.y) < 0.9f;
}