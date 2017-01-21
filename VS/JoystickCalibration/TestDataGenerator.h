#pragma once

#include <fstream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

class TestDataGenerator : public FrameProcessor, public KeyHandler
{
    Scheduler* _scheduler;
    XboxController* _controller;
    std::ofstream _outFile;

    const int64 _MIN_TICK_PERIOD;
    int64 _lastCapTime;

    bool _keepGoing;
    bool _running;
    int _frameCounter;

    float _xRate;
    float _yRate;

    void _quit();

public:
    TestDataGenerator(Scheduler* scheduler, XboxController* controller);
    ~TestDataGenerator();

    void run();
    void processFrame(cv::Mat& frame);
    void handleKey(int key);

};
