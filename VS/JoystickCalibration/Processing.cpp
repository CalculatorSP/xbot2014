#include "Processing.h"

#define KEY_ESC	(27)

using namespace cv;

void JoystickCalibrationAppManager::processFrame(cv::Mat& frame)
{
    imshow("result", frame);

    if (_running)
    {
        Mat mathable;
        frame.convertTo(mathable, CV_32F);

        if (_outfile != NULL)
        {
            fprintf_s(_outfile, "%d,%d\n", _frameCounter, _computeFlow(_prevFrame, frame));
            printf_s("%d,%d\n", _frameCounter, 0);
            _prevFrame = frame.clone();
        }

        if (++_frameCounter >= 36)
        {
            if (_outfile != NULL)
                fclose(_outfile);

            _controller->release(XboxAnalog::RIGHT_STICK_X);
            _controller->sendState();
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
            if (_outfile != NULL)
                fclose(_outfile);

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
            if (!fopen_s(&_outfile, "C:\\Users\\John\\Desktop\\75.csv", "w"))
            {
                _controller->set(XboxAnalog::RIGHT_STICK_X, 0.75f);
                _controller->sendState();
                _frameCounter = 0;
                _running = true;
            }
            else
            {
                std::cerr << "Could not open output file" << std::endl;
                _keepGoing = false;
            }
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

int JoystickCalibrationAppManager::_computeFlow(const Mat& oldImg, const Mat& newImg)
{
    return 0;
}
