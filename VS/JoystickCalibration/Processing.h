#pragma once

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Task/Scheduler.h"
#include "Util/ScreenGrabber.h"

class JoystickCalibrationAppManager : public FrameProcessor, public KeyHandler
{
	Scheduler* _scheduler;
	XboxController* _controller;
	bool _keepGoing;

	void _measure();

public:
	JoystickCalibrationAppManager(Scheduler* scheduler, XboxController* controller)
		: _scheduler(scheduler), _controller(controller), _keepGoing(true)
	{ }

	void run();
	void processFrame(cv::Mat& frame);
	void handleKey(int key);

};
