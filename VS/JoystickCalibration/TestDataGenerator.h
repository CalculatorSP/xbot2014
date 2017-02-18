#pragma once

#include <fstream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"

class TestDataGenerator
{
    cv::VideoCapture _cap;
    XboxController _controller;
    std::ofstream _outFile;

    const int64 _MIN_TICK_PERIOD;
    int64 _lastCapTime;

    bool _keepGoing;
    bool _running;
    int _frameCounter;

    float _xRate;
    float _yRate;

    void processFrame(cv::Mat& frame);
    void handleKey(int key);
    void _quit();

public:
    TestDataGenerator(const cv::VideoCapture& cap, const XboxController& controller);
    ~TestDataGenerator();

    void run();

};
