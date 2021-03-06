#include "opencv2/opencv.hpp"

#include <iostream>

#define WEBCAM	(0)
#define CAPCARD	(1)

using namespace cv;

int main(int argc, const char **argv)
{
	VideoCapture cap(CAPCARD);

	if (!cap.isOpened())
	{
		std::cerr << "Could not open capture device" << std::endl;
		return -1;
	}

	Mat frame;
	cvNamedWindow("result", CV_WINDOW_AUTOSIZE);

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			continue;
		imshow("result", frame);
		if (waitKey(30) == 27)
			break;
	}

	return 0;
}