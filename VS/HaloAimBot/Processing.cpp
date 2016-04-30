#include <sstream>
#include "Processing.h"

#define KEY_ESC	                (27)
#define MAX_RECORDING_FRAMES    (900)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _joystickVals(0.0f, 0.0f),
    _keepGoing(true),
    _autoAim(false),
    _eDetect(false),
    _screenshot(false),
    _ssCounter(0),
    _recording(false),
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
        if (!_autoAim)
            _targetTracker.reset();
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
    namedWindow("result", WINDOW_AUTOSIZE);

    while (_keepGoing)
        _scheduler->run();
}

void HaloAimBotAppManager::_quit()
{
    _targetTracker.reset();
    _keepGoing = false;
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
    Point target;
    bool targetFound = _hunter.findTarget(frame, target, _eDetect);
    if (targetFound)
    {
        circle(frame, target, 3, Scalar(0, 255, 0), -1);
        Point2f aimPoint((float)(target.x - _crosshairLocation.x), (float)(target.y - _crosshairLocation.y));

        if (_autoAim)
        {
            TargetTrackerOutput controls;
            _targetTracker.trackWithTarget(aimPoint, _joystickVals, controls);
            if (controls.giveUp)
            {
                printf("TARGET LOST\n");
            }
            else
            {
                //TODO apply controls
                _joystickVals = controls.joystickVals;
            }
        }
    }
    else if (_targetTracker.hasTarget() && _autoAim)
    {
        TargetTrackerOutput controls;
        _targetTracker.trackWithoutTarget(_joystickVals, controls);
        if (controls.giveUp)
        {
            printf("TARGET LOST\n");
        }
        else
        {
            //TODO apply controls
            _joystickVals = controls.joystickVals;
        }
    }
}
