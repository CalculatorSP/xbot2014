#include "ScreenGrabber.h"

ScreenGrabber::ScreenGrabber(int captureDevice, int fps, Scheduler* scheduler, FrameProcessor* frameProcessor, KeyHandler* keyHandler)
	: cap(captureDevice), _framePeriod(1000000UL / fps), _scheduler(scheduler), _frameProcessor(frameProcessor), _keyHandler(keyHandler)
{
	if (!cap.isOpened())
		return;

	_scheduleGrab();
}

void ScreenGrabber::_grab()
{
	cv::Mat frame;
	cap >> frame;
	if (!frame.empty())
		_frameProcessor->processFrame(frame);

	_keyHandler->handleKey(cvWaitKey(1));

	_scheduleGrab();
}

void ScreenGrabber::_scheduleGrab()
{
	_scheduler->postDelayed(_framePeriod, &ScreenGrabber::_grab, this);
}
