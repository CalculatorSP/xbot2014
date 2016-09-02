#include <fstream>
#include <sstream>

#include "Processing.h"

#define KEY_ESC                 (27)
#define MAX_RECORDING_FRAMES    (900)

using namespace cv;

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _xboxController(controller),
    _joystickVals(0.0f, 0.0f),
    _keepGoing(true),
    _lastCapTime(0),
    _MIN_TICK_PERIOD((int64)(.02*getTickFrequency())),
    _autoAim(false),
    _screenshot(false),
    _ssCounter(0),
    _recording(false),
    _crosshairLocation(639, 441)

{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    _dbgPrint.str(std::string());

    // Downsample from 60fps to 30fps
    int64 curTime = getTickCount();
    if (curTime - _lastCapTime < _MIN_TICK_PERIOD)
        return;

    std::cout << 1000.0 * (curTime - _lastCapTime) / getTickFrequency() << std::endl;
    _dbgPrint << 1000.0 * (curTime - _lastCapTime) / getTickFrequency() << std::endl;

    _lastCapTime = curTime;
    _updateStateMachine(frame);

    circle(frame, _crosshairLocation, 8, Scalar(0, 255, 255));

    imshow("result", frame);

    if (_recording)
    {
        _frames.push_back(frame.clone());
        _dbgPrintVector.push_back(_dbgPrint.str());
    }

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
        {
            _targetTracker.reset();
            _clearController();
        }
        _recording = !_recording;
        if (!_recording)
            _saveRecording();
        break;

    case 's':
        _screenshot = true;
        break;

    case 'r':
        break;

    default:
        break;
    }
}

void HaloAimBotAppManager::run()
{
    namedWindow("result", WINDOW_AUTOSIZE);
    _clearController();
    while (_keepGoing)
        _scheduler->run();
}

void HaloAimBotAppManager::_quit()
{
    _targetTracker.reset();
    _clearController();
    _keepGoing = false;
    _scheduler->clear();
}

void HaloAimBotAppManager::_clearController()
{
    _xboxController->set(XboxAnalog::RIGHT_STICK_X, 0.0f);
    _xboxController->set(XboxAnalog::RIGHT_STICK_Y, 0.0f);
    _xboxController->set(XboxButton::RIGHT_TRIGGER, false);
    _xboxController->sendState(3);
}

void HaloAimBotAppManager::_saveRecording()
{
    int tmp = _ssCounter;;
    for (int i = 0; i < _frames.size(); ++i)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << tmp++ << ".png";
        imwrite(filename.str(), _frames[i]);
    }

    for (int i = 0; i < _dbgPrintVector.size(); ++i)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".txt";
        std::ofstream outfile;
        outfile.open(filename.str(), std::ios::out | std::ios::trunc);
        outfile << _dbgPrintVector[i];
    }
    
    _frames.clear();
}

void HaloAimBotAppManager::_updateStateMachine(Mat& frame)
{
    Point target;
    bool targetFound = _hunter.findTarget(frame, target);
    if (targetFound)
    {
        circle(frame, target, 3, Scalar(0, 255, 0), -1);
        Point2f aimPoint((float)(target.x - _crosshairLocation.x), (float)(target.y - _crosshairLocation.y));

        if (_autoAim)
        {
            TargetTrackerOutput controls;
            _targetTracker.trackWithTarget(aimPoint, _joystickVals, controls, _dbgPrint);
            if (controls.giveUp)
            {
                std::cout << "TARGET LOST" << std::endl;
                _dbgPrint << "TARGET LOST" << std::endl;
            }

            _xboxController->set(XboxAnalog::RIGHT_STICK_X, controls.joystickVals.x);
            _xboxController->set(XboxAnalog::RIGHT_STICK_Y, controls.joystickVals.y);
            _xboxController->set(XboxButton::RIGHT_TRIGGER, controls.pullTrigger);
            _xboxController->sendState((controls.pullTrigger || controls.giveUp) ? 3 : 2);

            _joystickVals = controls.joystickVals;
        }
    }
    else if (_targetTracker.hasTarget() && _autoAim)
    {
        TargetTrackerOutput controls;
        _targetTracker.trackWithoutTarget(_joystickVals, controls, _dbgPrint);
        if (controls.giveUp)
        {
            std::cout << "TARGET LOST" << std::endl;
            _dbgPrint << "TARGET LOST" << std::endl;
        }
        _xboxController->set(XboxAnalog::RIGHT_STICK_X, controls.joystickVals.x);
        _xboxController->set(XboxAnalog::RIGHT_STICK_Y, controls.joystickVals.y);
        _xboxController->set(XboxButton::RIGHT_TRIGGER, controls.pullTrigger);
        _xboxController->sendState((controls.pullTrigger || controls.giveUp) ? 3 : 2);

        _joystickVals = controls.joystickVals;
    }
}
