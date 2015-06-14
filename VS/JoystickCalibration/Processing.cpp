#include "Processing.h"

#define KEY_ESC	(27)

using namespace cv;

void JoystickCalibrationAppManager::processFrame(cv::Mat& frame)
{
    imshow("result", frame);

    switch (_state)
    {
    case 1:
    {
        // Compute grayscale average of a few frames
        Mat mathable;
        frame.convertTo(mathable, CV_32F);
        cvtColor(mathable, mathable, COLOR_BGR2GRAY);
        _smoother.deposit(mathable.clone());

        // If we have deposited enough frames, take the average
        if (_smoother.isFull())
        {
            // Store the mean before moving joystick
            _oldScreen = _smoother.getMean();

            // Tilt to the right for 1 frame
            _controller->tap(XboxAnalog::RIGHT_STICK_X, 0.75f, 2);

            // Wait a few frames
            _frameCounter = 0;
            _smoother.reset();
            _state = 2;
        }
        return;
    }

    case 2:
        // Wait a few frames
        if (_frameCounter++ >= 30)
            _state = 3;
        return;

    case 3:
    {
        // Compute grayscale average of a few frames
        Mat mathable;
        frame.convertTo(mathable, CV_32F);
        cvtColor(mathable, mathable, COLOR_BGR2GRAY);
        _smoother.deposit(mathable.clone());

        // If we have deposited enough frames, take the average
        if (_smoother.isFull())
        {
            // Store the mean after moving joystick
            _newScreen = _smoother.getMean();

            // Compute the optical flow between frames
            _computeFlow(_oldScreen, _newScreen);
            _state = 0;
        }
        return;
    }

    default:
        return;
    }
}

void JoystickCalibrationAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        // Exit the app
        _keepGoing = false;
        return;

    case 'g':
        if (_state == 0)
        {
            // Move the joystick and compute the optical flow
            _smoother.reset();
            _state = 1;
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

void JoystickCalibrationAppManager::_computeFlow(const Mat& oldImg, const Mat& newImg)
{

}
