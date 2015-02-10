#include "Processors/FrameProcessor.h"

#include "opencv2/opencv.hpp"

#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, const char **argv)
{
	VideoCapture cap(1);

	if (!cap.isOpened())
	{
		cerr << "Could not open capture device" << endl;
		return -1;
	}

	Mat frame;
	Mat processed;
	FrameProcessor frameProcessor;

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			continue;

		processed = frameProcessor.process(frame);

		imshow("Capture", frame);
		imshow("Processed", processed);
		if (waitKey(30) == 27)
			break;
	}

	return 0;
}