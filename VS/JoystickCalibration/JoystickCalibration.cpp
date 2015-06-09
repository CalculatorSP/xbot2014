#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Util/ScreenGrabber.h"
#include "Processing.h"

#include <iostream>

#define WEBCAM	(0)
#define CAPCARD	(1)

using namespace cv;

int main(int argc, const char **argv)
{
	Scheduler scheduler;
	FrameDisplay frameDisplay;
	InputMonitor inputMonitor;
	ScreenGrabber grabber(WEBCAM, 30, &scheduler, &frameDisplay, &inputMonitor);
	//XboxController xboxController("COM4");
	
	if (!grabber.cap.isOpened())
	{
		std::cerr << "Could not open capture device" << std::endl;
		return -1;
	}

	cvNamedWindow("result", CV_WINDOW_AUTOSIZE);

	while (inputMonitor.keepGoing)
		scheduler.run();

	return 0;
}
