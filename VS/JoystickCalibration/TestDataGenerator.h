#pragma once

#include <fstream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"

class TestDataGenerator
{
    cv::VideoCapture* _cap;
    XboxController* _controller;
    std::vector<cv::Mat> _frames;
    std::vector<std::string> _dbgPrintVector;
    std::stringstream _printStr;

    const int64 _MIN_TICK_PERIOD;
    int64 _lastCapTime;

    float _xRate;
    float _yRate;

    bool _keepGoing;
    bool _running;
    bool _yMode;
    int _frameCounter;
    int _globalFrameCounter;

    void _processFrame(cv::Mat& frame);
    void _handleKey(int key);
    void _updateController(float x, float y);
    void _quit();

public:
    TestDataGenerator(cv::VideoCapture* cap, XboxController* controller);
    ~TestDataGenerator();

    void run();

};
