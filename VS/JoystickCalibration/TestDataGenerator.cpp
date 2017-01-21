#include "TestDataGenerator.h"

#define KEY_ESC             (27)
#define FRAMES_TO_RUN       (30)
#define MIN_DATAPOINTS      (5)

using namespace cv;

TestDataGenerator::TestDataGenerator(Scheduler* scheduler, XboxController* controller)
    : _scheduler(scheduler),
    _controller(controller),

    _MIN_TICK_PERIOD((int64)(.02*getTickFrequency())),
    _lastCapTime(0),

    _keepGoing(true),
    _running(false),
    _frameCounter(0),

    _xRate(-0.9f),
    _yRate(0.0f)
{
    //TODO output header
    _outFile.open("C:/Users/John/Desktop/75.csv");
    _outFile << "x,y,gamma,alpha" << std::endl;
}

TestDataGenerator::~TestDataGenerator()
{
    _quit();
}

void TestDataGenerator::processFrame(Mat& frame)
{
    // Downsample from 60fps to 30fps
    int64 curTime = getTickCount();
    if (curTime - _lastCapTime < _MIN_TICK_PERIOD)
        return;
    _lastCapTime = curTime;

    imshow("result", frame);
    if (_running)
    {
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
        }
    }
}

void TestDataGenerator::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _quit();
        break;

    case 'g':
        if (!_running)
        {
            _frameCounter = 0;
            _running = true;
        }
        break;

    default:
        break;
    }
}

void TestDataGenerator::run()
{
    while (_keepGoing)
        _scheduler->run();
}

void TestDataGenerator::_quit()
{
    _controller->release(XboxAnalog::RIGHT_STICK_X);
    _controller->release(XboxAnalog::RIGHT_STICK_Y);
    _controller->sendState();
    _running = false;

    if (_outFile.is_open())
        _outFile.close();

    _keepGoing = false;
}
