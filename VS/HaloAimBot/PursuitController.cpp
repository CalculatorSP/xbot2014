#include "PursuitController.h"

PursuitController::PursuitController(Scheduler* scheduler, XboxController* xboxController)
    :
    _scheduler(scheduler),
    _xboxController(xboxController)
{ }

PursuitController::~PursuitController()
{
    stop();
}

void PursuitController::startPursuing(Point2f target)
{

}

void PursuitController::stop()
{
    _xboxController->reset();
    _xboxController->sendState();
}