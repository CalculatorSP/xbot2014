#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "RedTriangleHunter.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

#include "TargetTracker.h"


class HaloAimBotAppManager : public FrameProcessor, public KeyHandler
{
    Scheduler*                  _scheduler;
    XboxController*             _xboxController;
    RedTriangleHunter           _hunter;
    TargetTracker               _targetTracker;
    cv::Point2f                 _joystickVals;

    bool _keepGoing;
    int64 _lastCapTime;
    const int64 _MIN_TICK_PERIOD;
    bool _autoAim;
    bool _screenshot;
    int _ssCounter;
    bool _recording;
    std::vector<cv::Mat> _frames;
    std::vector<std::string> _dbgPrintVector;
    std::stringstream _dbgPrint;

    const cv::Point _crosshairLocation;

    void _quit();
    void _saveRecording();
    void _updateStateMachine(cv::Mat& frame);
    void _clearController();

public:
    HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller);
    ~HaloAimBotAppManager();

    void run();
    void processFrame(cv::Mat& frame);
    void handleKey(int key);

};
