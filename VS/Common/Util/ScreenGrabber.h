#pragma once

#include "opencv2/opencv.hpp"
#include "Task/Scheduler.h"

class FrameProcessor
{
public:
	virtual void processFrame(cv::Mat& frame) = 0;

};

class KeyHandler
{
public:
	virtual void handleKey(int key) = 0;

};

class ScreenGrabber
{
	Scheduler* _scheduler;
	FrameProcessor* _frameProcessor;
	KeyHandler* _keyHandler;
	const uint64_t _framePeriod;

	void _grab();
	void _scheduleGrab();

public:
	cv::VideoCapture cap;

	ScreenGrabber(int captureDevice, int fps, Scheduler* scheduler, FrameProcessor* frameProcessor, KeyHandler* keyHandler);

};

