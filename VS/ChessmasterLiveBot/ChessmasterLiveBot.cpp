#include "ChessManager.h"

#include "opencv2/opencv.hpp"

#include <iostream>

#define WEBCAM	(0)
#define CAPCARD	(1)

using namespace cv;
using namespace std;

int main(int argc, const char **argv)
{
	VideoCapture cap(WEBCAM);

	if (!cap.isOpened())
	{
		cerr << "Could not open capture device" << endl;
		return -1;
	}

	int w = (int)cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int h = (int)cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	printf("(%d, %d)\n", w, h);

	Mat frame;
	Mat processed;
	ChessManager chessManager;

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processed", CV_WINDOW_AUTOSIZE);

	for (;;)
	{
		cap >> frame;
		if (frame.empty())
			continue;

		chessManager.depositFrame(frame, processed);

		imshow("Capture", frame);
		if (!processed.empty())
			imshow("Processed", processed);

		if (waitKey(30) == 27)
			break;
	}

	return 0;
}