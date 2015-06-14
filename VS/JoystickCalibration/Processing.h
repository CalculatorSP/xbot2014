#pragma once

#include "opencv2/opencv.hpp"

#include "Collections/WindowedStats.h"
#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

#define SMOOTH_SIZE (30)

class JoystickCalibrationAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler* _scheduler;
    XboxController* _controller;
    WindowedStats<cv::Mat, SMOOTH_SIZE> _smoother;
    Mat _oldScreen, _newScreen;
    bool _keepGoing;
    int _state;
    int _frameCounter;

    void _computeFlow(const Mat& oldImg, const Mat& newImg);

public:
    JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller)
        : _scheduler(scheduler), _controller(controller), _keepGoing(true), _state(0), _frameCounter(0)
    { }

    void run();
    void processFrame(cv::Mat& frame);
    void handleKey(int key);

};
