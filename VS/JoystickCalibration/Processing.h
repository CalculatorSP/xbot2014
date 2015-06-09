#pragma once

#include "opencv2/opencv.hpp"

class FrameDisplay : public FrameProcessor
{
public:
	void processFrame(cv::Mat& frame)
	{
		imshow("result", frame);
	}
};

class InputMonitor : public KeyHandler
{
public:
	bool keepGoing = true;

	void handleKey(int key)
	{
		switch (key)
		{
		case 27:
			keepGoing = false;
			return;
		default:
			return;
		}
	}
};
