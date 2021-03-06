#include "ScreenGrabber.h"

using namespace cv;

ScreenGrabber::ScreenGrabber(int captureDevice, int fps, Scheduler* scheduler, FrameProcessor* frameProcessor, KeyHandler* keyHandler)
    : cap(captureDevice), _framePeriod(1000000UL / fps), _scheduler(scheduler), _frameProcessor(frameProcessor), _keyHandler(keyHandler)
{
    if (!cap.isOpened())
        return;

    _scheduleGrab();
}

void ScreenGrabber::_grab()
{
    _scheduleGrab();

    Mat frame;
    cap >> frame;
    if (!frame.empty())
        _frameProcessor->processFrame(frame);

    _keyHandler->handleKey(waitKey(1));
}

void ScreenGrabber::_scheduleGrab()
{
    _scheduler->postDelayed(_framePeriod, &ScreenGrabber::_grab, this);
}
