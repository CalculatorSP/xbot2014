#include <sstream>
#include "Processing.h"

#define KEY_ESC	            (27)
#define FRAME_DELAY         (2)
#define RED_OPPOSITE        (180)

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
    _preprocess(frame);
    imshow("result", frame);

    _motionTracker.update();
    _updateStateMachine(frame);
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

void HaloAimBotAppManager::_preprocess(Mat& frame)
{
    resize(frame, frame, Size(), 1.0, 0.5, INTER_NEAREST);
    if (_eDetect)
    {
        Mat hsv[3];
        cvtColor(frame, frame, CV_BGR2HSV);
        split(frame, hsv);
        absdiff(hsv[0], RED_OPPOSITE / 2, frame);
        //Canny(frame, frame, 90.0, 100.0, 3, false);
    }

    if (_screenshot)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), frame);
        _screenshot = false;
    }
}

void HaloAimBotAppManager::_updateStateMachine(const Mat& frame)
{
    switch (_state)
    {
    case HUNTING:
    {
        Point2f target(0.0f, 0.0f);
        if (_hunter.findTarget(frame, target))
        {
            if (_pursuitController.startPursuing(target - _motionTracker.motionHistory[-FRAME_DELAY]))
                _state = PURSUIT;
        }

        break;
    }

    case PURSUIT:
    {
        Point2f newTarget(0.0f, 0.0f);
        if (_hunter.findTarget(frame, newTarget))
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
