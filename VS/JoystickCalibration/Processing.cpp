#include "Processing.h"

#define KEY_ESC	            (27)
#define FIELD_OF_VIEW_DEG   (61.0f)
#define SCREEN_WIDTH        (640)

const Rect JoystickCalibrationAppManager::_flowRoi(150, 40, 280, 100);

JoystickCalibrationAppManager::JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller)
    : _scheduler(scheduler),
    _controller(controller),
    _flowCalculator(SCREEN_WIDTH, FIELD_OF_VIEW_DEG),

    _keepGoing(true),
    _running(false),
    _frameCounter(0)
{ }

void JoystickCalibrationAppManager::processFrame(Mat& frame)
{
    imshow("result", frame);

    if (_running)
    {
        resize(frame, frame, Size(), 1.0, 0.5, INTER_NEAREST);
        _flowCalculator.depositFrame(frame(_flowRoi));

        if (++_frameCounter >= 90)
        {
            // Release the joystick
            _controller->release(XboxAnalog::RIGHT_STICK_X);
            _controller->sendState();

            // Compute the rotation for each frame
            _flowCalculator.calculate();

            // Print the results to a file
            if (_outFile.is_open())
            {
                _outFile << "time,gamma,beta,alpha" << std::endl;
                for (int i = 0; i < _flowCalculator.gammaBetaAlphaRotations.size(); ++i)
                {
                    _outFile <<
                        i << "," <<
                        _flowCalculator.gammaBetaAlphaRotations[i].x << "," <<
                        _flowCalculator.gammaBetaAlphaRotations[i].y << "," <<
                        _flowCalculator.gammaBetaAlphaRotations[i].z << std::endl;
                }

                _outFile.close();
            }

            _frameCounter = 0;
            _running = false;
        }
    }
}

void JoystickCalibrationAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        // Exit the app
        if (_running)
        {
            if (_outFile.is_open())
                _outFile.close();

            _controller->release(XboxAnalog::RIGHT_STICK_X);
            _controller->sendState();
            _running = false;
        }
        _keepGoing = false;
        return;

    case 'g':
        // Move the joystick and compute the optical flow
        if (!_running)
        {
            _outFile.open("C:\\Users\\John\\Desktop\\75.csv");
            _controller->set(XboxAnalog::RIGHT_STICK_X, 0.90f);
            _controller->sendState();
            _frameCounter = 0;
            _running = true;
        }
        return;

    default:
        return;
    }
}

void JoystickCalibrationAppManager::run()
{
    while (_keepGoing)
        _scheduler->run();
}
