#include "MotionModel.h"
#include "MotionTracker.h"

MotionTracker::MotionTracker()
    : joystickVals(0.0f, 0.0f)
{ }

void MotionTracker::update()
{
    Point2f motionThisFrame = MotionModel::getRotationRate(joystickVals);
    motionHistory.deposit(Point2f(0.0f, 0.0f));
    for (int i = 0; i < MOTION_HISTORY_SIZE; ++i)
        motionHistory[i] += motionThisFrame;
}
