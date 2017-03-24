#include "TestDataGenerator.h"

#define KEY_ESC             (27)

using namespace cv;

TestDataGenerator::TestDataGenerator(VideoCapture* cap, XboxController* controller) :
    _cap(cap),
    _controller(controller),
    _frames(),

    _MIN_TICK_PERIOD((int64)(.02*getTickFrequency())),
    _lastCapTime(0),

    _xRate(0.0f),
    _yRate(0.0f),

    _keepGoing(true),
    _running(false),
    _yMode(false),
    _frameCounter(0),
    _globalFrameCounter(0)
{
    namedWindow("result", WINDOW_AUTOSIZE);
}

TestDataGenerator::~TestDataGenerator()
{
    _quit();
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
            _processFrame(frame);
            _lastCapTime = curTime;
        }

        _handleKey(waitKey(1));
    }

    _quit();
}

void TestDataGenerator::_processFrame(Mat& frame)
{
    imshow("result", frame);
    if (_running)
    {
        _printStr.str(std::string());

        ++_frameCounter;
        ++_globalFrameCounter;
        _frames.push_back(frame.clone());
        _printStr << _globalFrameCounter << "," << _xRate << "," << _yRate << std::endl;
        _dbgPrintVector.push_back(_printStr.str());

        if (_yMode && _frameCounter > 15)
        {
            _updateController(0.75f, 0.0f);
            _yMode = false;
            _frameCounter = 0;
        }
        else if (_frameCounter > 120)
        {
            _updateController(0.0f, 0.75f);
            _frameCounter = 0;
            _yMode = true;
        }
    }
}

void TestDataGenerator::_handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _keepGoing = false;
        break;

    case 'g':
        if (!_running)
        {
            _updateController(0.75f, 0.0f);
            _frameCounter = 0;
            _yMode = false;
            _running = true;
        }
        break;

    default:
        break;
    }
}

void TestDataGenerator::_updateController(float x, float y)
{
    _xRate = x;
    _yRate = y;
    _controller->set(XboxAnalog::RIGHT_STICK_X, _xRate);
    _controller->set(XboxAnalog::RIGHT_STICK_Y, _yRate);
    _controller->sendState();
}

void TestDataGenerator::_quit()
{
    _controller->release(XboxAnalog::RIGHT_STICK_X);
    _controller->release(XboxAnalog::RIGHT_STICK_Y);
    _controller->sendState();

    for (int i = 0; i < _frames.size(); ++i)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << i << ".png";
        imwrite(filename.str(), _frames[i]);
    }

    std::ofstream outfile;
    outfile.open("C:/Users/John/Desktop/cap/input.csv", std::ios::out | std::ios::app);
    for (int i = 0; i < _dbgPrintVector.size(); ++i)
        outfile << _dbgPrintVector[i];
    outfile.close();

    _frames.clear();
    _dbgPrintVector.clear();
}
