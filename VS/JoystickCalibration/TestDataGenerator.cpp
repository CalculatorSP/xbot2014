#include "TestDataGenerator.h"

#define KEY_ESC             (27)
#define FRAMES_TO_RUN       (30)

using namespace cv;

TestDataGenerator::TestDataGenerator(VideoCapture* cap, XboxController* controller) :
    _cap(cap),
    _controller(controller),
    _MIN_TICK_PERIOD((int64)(.02*getTickFrequency())),
    _lastCapTime(0),

    _keepGoing(true),
    _running(false),
    _frameCounter(0),

    _xRate(-0.9f),
    _yRate(0.0f)
{
    _outFile.open("C:/Users/John/Desktop/75.csv");
    _outFile << "frame,x,y" << std::endl;

    namedWindow("result", WINDOW_AUTOSIZE);
}

TestDataGenerator::~TestDataGenerator()
{
    _quit();
}

void TestDataGenerator::processFrame(Mat& frame)
{
    imshow("result", frame);
    if (_running)
    {
        _controller->set(XboxAnalog::RIGHT_STICK_X, _xRate);
        _controller->sendState();

        if (++_frameCounter >= FRAMES_TO_RUN)
        {
            // Release the joysticks
            _controller->release(XboxAnalog::RIGHT_STICK_X);
            _controller->release(XboxAnalog::RIGHT_STICK_Y);
            _controller->sendState();

            // Print the results to a file
            if (_outFile.is_open())
            {

            }

            _frameCounter = 0;
            _running = false;
            _keepGoing = false;
        }
    }
}

void TestDataGenerator::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _keepGoing = false;
        break;

    case 'g':
        if (!_running)
            _running = true;
        break;

    default:
        break;
    }
}

void TestDataGenerator::run()
{
    Mat frame;
    while (_keepGoing)
    {
        *_cap >> frame;
        if (frame.empty())
            break;

        // Downsample from 60fps to 30fps
        int64 curTime = getTickCount();
        if (curTime - _lastCapTime >= _MIN_TICK_PERIOD)
        {
            processFrame(frame);
            _lastCapTime = curTime;
        }

        handleKey(waitKey(1));
    }

    _quit();
}

void TestDataGenerator::_quit()
{
    _controller->release(XboxAnalog::RIGHT_STICK_X);
    _controller->release(XboxAnalog::RIGHT_STICK_Y);
    _controller->sendState();
    if (_outFile.is_open())
        _outFile.close();
}
