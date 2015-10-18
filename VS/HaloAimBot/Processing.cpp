#include <sstream>
#include "Processing.h"

#define KEY_ESC	                (27)
#define MAX_RECORDING_FRAMES    (900)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _pursuitController(controller),
    _state(HUNTING),
    _autoAim(false),
    _eDetect(false),
    _screenshot(false),
    _ssCounter(0),
    _recording(false),
    _frames(),
    _crosshairLocation(318, 294)
{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    _updateStateMachine(frame);

    resize(frame, frame, Size(), 1.0, 0.5, INTER_NEAREST);
    resize(frame, frame, Size(), 1.0, 2.0, INTER_NEAREST);

    circle(frame, _crosshairLocation, 8, Scalar(0, 255, 255));

    imshow("result", frame);

    if (_recording)
        _frames.push_back(frame.clone());

    if (_frames.size() > MAX_RECORDING_FRAMES)
        _saveRecording();

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

    case 'a':
        _autoAim = !_autoAim;
        break;

    case 'e':
        _eDetect = !_eDetect;
        break;

    case 's':
        _screenshot = true;
        break;

    case 'r':
        _recording = !_recording;
        if (!_recording)
            _saveRecording();
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

void HaloAimBotAppManager::_saveRecording()
{
    for (int i = 0; i < _frames.size(); ++i)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), _frames[i]);
    }
    
    _frames.clear();
}

void HaloAimBotAppManager::_updateStateMachine(Mat& frame)
{
    switch (_state)
    {
    case HUNTING:
    {
        Point target;
        if (_hunter.findTarget(frame, target, _eDetect))
        {
            circle(frame, target, 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint((float)(target.x - _crosshairLocation.x), (float)(target.y - _crosshairLocation.y));

            if (!_autoAim)
                break;

            if (_pursuitController.startPursuing(aimPoint))
            {
                _state = PURSUIT;
                printf("TARGET ACQUIRED\n");
            }
        }
        break;
    }

    case PURSUIT:
    {
        Point newTarget;
        if (_hunter.findTarget(frame, newTarget, _eDetect))
        {
            circle(frame, newTarget, 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint((float)(newTarget.x - _crosshairLocation.x), (float)(newTarget.y - _crosshairLocation.y));

            if (!_pursuitController.updateWithTarget(aimPoint) || !_autoAim)
            {
                _pursuitController.reset();
                _state = HUNTING;
                printf("TARGET LOST\n");
            }
        }
        else if (!_pursuitController.updateWithoutTarget() || !_autoAim)
        {
            _pursuitController.reset();
            _state = HUNTING;
            printf("TARGET LOST\n");
        }
        break;
    }

    default:
        break;
    }
}
