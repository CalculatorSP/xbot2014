#pragma once

#include <fstream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

#include "FlowCalculator.h"

class JoystickCalibrationAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler* _scheduler;
    XboxController* _controller;
    FlowCalculator _flowCalculator;
    std::ofstream _outFile;

    static const cv::Rect _flowRoi;

    const int64 _MIN_TICK_PERIOD;
    int64 _lastCapTime;

    bool _keepGoing;
    bool _running;
    bool _switchToY;
    bool _done;
    int _frameCounter;

    float _xRate;
    float _yRate;

public:
    JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller);

    void run();
    void processFrame(cv::Mat& frame);
    void handleKey(int key);

};
