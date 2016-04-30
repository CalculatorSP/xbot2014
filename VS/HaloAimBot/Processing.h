#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "RedTriangleHunter.h"
#include "TargetTracker.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

using namespace cv;

class HaloAimBotAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler*                  _scheduler;
    RedTriangleHunter           _hunter;
    TargetTracker               _targetTracker;

    bool _keepGoing;
    bool _autoAim;
    bool _eDetect;
    bool _screenshot;
    int _ssCounter;
    bool _recording;
    vector<Mat> _frames;

    const Point _crosshairLocation;

    void _quit();
    void _saveRecording();
    void _updateStateMachine(Mat& frame);

public:
    HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller);
    ~HaloAimBotAppManager();

    void run();
    void processFrame(Mat& frame);
    void handleKey(int key);

};
