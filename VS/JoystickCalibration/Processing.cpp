#include "Processing.h"

#define KEY_ESC             (27)
#define FRAMES_TO_RUN       (30)
#define MIN_DATAPOINTS      (5)

using namespace cv;

const Rect JoystickCalibrationAppManager::_flowRoi(150, 40, 360, 100);

JoystickCalibrationAppManager::JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller)
    : _scheduler(scheduler),
    _controller(controller),

    _MIN_TICK_PERIOD((int64)(.02*getTickFrequency())),
    _lastCapTime(0),

    _keepGoing(true),
    _running(false),
    _switchToY(false),
    _done(false),
    _frameCounter(0),

    _xRate(0.0f),
    _yRate(0.0f)
{
    _outFile.open("C:\\Users\\John\\Desktop\\75.csv");
    _outFile << "x,y,gamma,alpha" << std::endl;
}

void JoystickCalibrationAppManager::processFrame(Mat& frame)
{
    // Downsample from 60fps to 30fps
    int64 curTime = getTickCount();
    if (curTime - _lastCapTime < _MIN_TICK_PERIOD)
        return;
    _lastCapTime = curTime;

    rectangle(frame, _flowRoi, Scalar(255, 255, 255));
    imshow("result", frame);
    if (_running)
    {
        _flowCalculator.depositFrame(frame(_flowRoi));

        if (++_frameCounter >= FRAMES_TO_RUN)
        {
            // Release the joysticks
            _controller->release(XboxAnalog::RIGHT_STICK_X);
            _controller->release(XboxAnalog::RIGHT_STICK_Y);
            _controller->sendState();

            // Compute the rotation for each frame
            _flowCalculator.calculate();

            // Print the results to a file
            if (_outFile.is_open() && _flowCalculator.gammaAlphaRotations.size() >= MIN_DATAPOINTS)
            {
                for (int i = 0; i < _flowCalculator.gammaAlphaRotations.size(); ++i)
                {
                    _outFile <<
                        _xRate << "," <<
                        _yRate << "," <<
                        _flowCalculator.gammaAlphaRotations[i].x << "," <<
                        _flowCalculator.gammaAlphaRotations[i].y << std::endl;
                }

                if (!_switchToY)
                {
                    if (fabsf(_xRate) > 0.8f)
                        _xRate += 0.01f;
                    else if (fabsf(_xRate) > 0.7f)
                        _xRate += 0.02f;
                    else if (fabsf(_xRate) > 0.20f)
                        _xRate += 0.05f;
                    else
                        _xRate = 0.25f;

                    if (_xRate > 0.9f)
                    {
                        _xRate = 0.0f;
                        _yRate = -0.9f;
                        _switchToY = true;
                    }
                }
                else
                {
                    if (fabsf(_yRate) > 0.8f)
                        _yRate += 0.01f;
                    else if (fabsf(_yRate) > 0.7f)
                        _yRate += 0.02f;
                    else if (fabsf(_yRate) > 0.20f)
                        _yRate += 0.05f;
                    else
                        _yRate = 0.25f;

                    if (_yRate > 0.9f)
                        _done = true;
                }
            }

            _frameCounter = 0;
            _running = false;
        }
    }
}

void JoystickCalibrationAppManager::handleKey(int key)
{
    if (!_done)
    {
        switch (key)
        {
        case KEY_ESC:
            break;

        case 'g':
            // Move the joysticks and compute the optical flow
            if (!_running)
            {
                _xRate = -0.9f;
                _yRate = 0.0f;
                printf("\n-----------------------------\n%f, %f\n-----------------------------\n", _xRate, _yRate);
                _controller->set(XboxAnalog::RIGHT_STICK_X, _xRate);
                _controller->set(XboxAnalog::RIGHT_STICK_Y, _yRate);
                _controller->sendState();
                _frameCounter = 0;
                _running = true;
            }
            return;

        default:
            return;
        }
    }

    _controller->release(XboxAnalog::RIGHT_STICK_X);
    _controller->release(XboxAnalog::RIGHT_STICK_Y);
    _controller->sendState();
    _running = false;

    if (_outFile.is_open())
        _outFile.close();

    _keepGoing = false;
}

void JoystickCalibrationAppManager::run()
{
    while (_keepGoing)
        _scheduler->run();
}
