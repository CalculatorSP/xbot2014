#include <sstream>
#include "Processing.h"

#define KEY_ESC	            (27)
#define FRAME_DELAY         (2)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _pursuitController(scheduler, controller, &_motionTracker),
    _state(HUNTING),
    _eDetect(false),
    _screenshot(false),
    _ssCounter(0)
{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    _motionTracker.update();
    _updateStateMachine(frame);

    imshow("result", frame);

    if (_screenshot)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), frame);
        _screenshot = false;
    }
}

void HaloAimBotAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _quit();
        break;

    case 'e':
        _eDetect = !_eDetect;
        break;

    case 'r':
        _screenshot = true;
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
    _pursuitController.reset();
    _state = QUIT;
    _scheduler->clear();
}

void HaloAimBotAppManager::_updateStateMachine(Mat& frame)
{
    switch (_state)
    {
    case HUNTING:
    {
        Point2f target(0.0f, 0.0f);
        if (_hunter.findTarget(frame, target, _eDetect))
        {
            circle(frame, target, 3, Scalar(0, 255, 0), -1);
            //if (_pursuitController.startPursuing(target - _motionTracker.motionHistory[-FRAME_DELAY]))
            //    _state = PURSUIT;
        }
        break;
    }

    case PURSUIT:
    {
        Point2f newTarget(0.0f, 0.0f);
        if (_hunter.findTarget(frame, newTarget, _eDetect))
        {
            if (!_pursuitController.updateTarget(newTarget - _motionTracker.motionHistory[-FRAME_DELAY]))
            {
                _pursuitController.reset();
                _state = HUNTING;
            }
        }
        break;
    }

    default:
        break;
    }
}
