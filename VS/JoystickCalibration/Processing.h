#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

#define SMOOTH_SIZE (30)

class JoystickCalibrationAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler* _scheduler;
    XboxController* _controller;
    cv::Mat _prevFrame;
    bool _keepGoing;
    bool _running;
    int _frameCounter;
    FILE* _outfile;

    static int _computeFlow(const cv::Mat& oldImg, const cv::Mat& newImg);

public:
    JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller)
        : _scheduler(scheduler), _controller(controller), _keepGoing(true), _running(false), _frameCounter(0), _outfile(NULL)
    { }

    void run();
    void processFrame(cv::Mat& frame);
    void handleKey(int key);

};
