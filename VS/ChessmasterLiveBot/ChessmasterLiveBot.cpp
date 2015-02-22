#include "ChessManager.h"

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

	int w = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int h = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	printf("(%d, %d)\n", w, h);

	Mat frame;
	ChessManager chessManager;

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			continue;

		chessManager.depositFrame(frame);

		imshow("Capture", frame);

		if (waitKey(30) == 27)
			break;
	}

	return 0;
}