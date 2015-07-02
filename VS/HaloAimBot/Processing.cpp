#include "Processing.h"

#define KEY_ESC	            (27)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    : 
    _scheduler(scheduler),
    _pursuitController(scheduler, controller),
    _state(HUNTING)
{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    imshow("result", frame);

    switch (_state)
    {
    case HUNTING:
    {
        Point2f target(0.0f, 0.0f);
        if (_hunter.findTarget(frame, target))
        {
            _state = PURSUIT;
            _pursuitController.startPursuing(target);
        }
        break;
    }

    case PURSUIT:
        break;

    default:
        break;
    }
}

void HaloAimBotAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _quit();
        break;

    default:
        break;
    }
}

void HaloAimBotAppManager::run()
{
    while (_state != QUIT)
        _scheduler->run();
}

void HaloAimBotAppManager::_quit()
{
    _pursuitController.stop();
    _state = QUIT;
    _scheduler->clear();
}
