#include <sstream>
#include "Processing.h"

#define KEY_ESC	            (27)
#define FRAME_DELAY         (3)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _pursuitController(scheduler, controller, &_motionTracker),
    _state(HUNTING),
    _eDetect(false),
    _pMode(false),
    _screenshot(false),
    _ssCounter(0),
    _crosshairLocation(318, 294)
{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    _motionTracker.update();
    _pursuitController.updateTargetHistory(_motionTracker.motionHistory[-1]);
    _updateStateMachine(frame);

    circle(frame, Point(318, 294), 8, Scalar(0, 255, 255));
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

    case 'p':
        _pMode = !_pMode;
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
        //Point target;
        //if (_hunter.findTarget(frame, target, _eDetect))
        if (_pMode)
        {
            _target = Point2f(200.0f, 200.0f);
            if (Rect(0, 0, frame.cols, frame.rows).contains(_target))
                circle(frame, _target + _motionTracker.motionHistory[-FRAME_DELAY], 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint(_target.x - _crosshairLocation.x, _target.y - _crosshairLocation.y);
            printf("Frame comp1: (%f, %f)\n", _motionTracker.motionHistory[-1].x, _motionTracker.motionHistory[-1].y);
            printf("Frame comp2: (%f, %f)\n", _motionTracker.motionHistory[-2].x, _motionTracker.motionHistory[-2].y);
            printf("Frame comp3: (%f, %f)\n", _motionTracker.motionHistory[-3].x, _motionTracker.motionHistory[-3].y);
            if (_pursuitController.startPursuing(aimPoint))// - _motionTracker.motionHistory[-FRAME_DELAY]))
            {
                _state = PURSUIT;
                printf("TARGET ACQUIRED\n");
            }
        }
        break;
    }

    case PURSUIT:
    {
        //Point newTarget;
        //if (_hunter.findTarget(frame, newTarget, _eDetect))
        {
            _target -= _motionTracker.motionHistory[-1];
            if (Rect(0, 0, frame.cols, frame.rows).contains(_target))
                circle(frame, _target + _motionTracker.motionHistory[-FRAME_DELAY], 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint(_target.x - _crosshairLocation.x, _target.y - _crosshairLocation.y);
            printf("Frame comp1: (%f, %f)\n", _motionTracker.motionHistory[-1].x, _motionTracker.motionHistory[-1].y);
            printf("Frame comp2: (%f, %f)\n", _motionTracker.motionHistory[-2].x, _motionTracker.motionHistory[-2].y);
            printf("Frame comp3: (%f, %f)\n", _motionTracker.motionHistory[-3].x, _motionTracker.motionHistory[-3].y);
            if (!_pursuitController.updateWithTarget(aimPoint) || !_pMode)// - _motionTracker.motionHistory[-FRAME_DELAY]) || !_pMode)
            {
                _pursuitController.reset();
                _state = HUNTING;
                printf("TARGET LOST\n");
                _pMode = false;
            }
        }
        break;
    }

    default:
        break;
    }
}
