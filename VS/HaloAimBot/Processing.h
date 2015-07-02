#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "PursuitController.h"
#include "RedTriangleHunter.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

using namespace cv;

class HaloAimBotAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler*          _scheduler;
    RedTriangleHunter   _hunter;
    PursuitController   _pursuitController;

    enum
    {
        HUNTING,
        PURSUIT,
        QUIT

    } _state;

    void _quit();

public:
    HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller);
    ~HaloAimBotAppManager();

    void run();
    void processFrame(Mat& frame);
    void handleKey(int key);

};
