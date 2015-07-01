#pragma once

#include "opencv2/opencv.hpp"

#include "FlowCalculator.h"
#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"
#include <fstream>

using namespace cv;

class JoystickCalibrationAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler* _scheduler;
    XboxController* _controller;
    FlowCalculator _flowCalculator;
    std::ofstream _outFile;

    static const Rect _flowRoi;

    bool _keepGoing;
    bool _running;
    int _frameCounter;

public:
    JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller);

    void run();
    void processFrame(Mat& frame);
    void handleKey(int key);

};
