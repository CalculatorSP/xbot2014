#include "ChessManager.h"

#include "opencv2/opencv.hpp"

#include <iostream>

#define WEBCAM	(0)
#define CAPCARD	(1)
#define COMPORT "COM4"

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
	ChessActuator chessActuator(COMPORT);
	ChessManager chessManager(&chessActuator);

	cvNamedWindow("Capture", CV_WINDOW_AUTOSIZE);

	bool keepGoing = true;
	while (keepGoing)
	{
		cap >> frame;
		if (frame.empty())
			continue;

		chessManager.depositFrame(frame);

		imshow("Capture", frame);

		switch (waitKey(1))
		{
		case 'r':
			chessManager.reset();
			break;
		case 'q':
			chessManager.endGame();
			break;
		case 27:
			keepGoing = false;
			break;
		default:
			break;
		}
	}

	return 0;
}