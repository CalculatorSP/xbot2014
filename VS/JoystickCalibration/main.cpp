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