#include "opencv2/opencv.hpp"
#include "MidiParser.h"
#include <iostream>
#include <Windows.h>

#define WEBCAM	(0)
#define CAPCARD	(1)

using namespace cv;

const char* filename = "C:/Users/John/Source/Repos/xbot2014/VS/GuitarHeroBot/songs/Slow Ride.mid";

int main(int argc, const char **argv)
{
	VideoCapture cap(CAPCARD);

	//if (!cap.isOpened())
	//{
	//	std::cerr << "Could not open capture device" << std::endl;
	//	return -1;
	//}

	Song* song = MidiParser::parseMidi(filename, GUITAR, EXPERT);
	if (song == NULL)
	{
		std::cerr << "Could not open file " << std::string(filename) << std::endl;
		return -1;
	}

	uint64_t currentTime = 0;
	for (int i = 0; i < song->size; ++i)
	{
		uint32_t delay = (uint32_t)(((*song)[i].timestamp - currentTime) / 1000);
		currentTime = (uint32_t)(*song)[i].timestamp;
		Sleep(delay);
		if ((*song)[i].press)
			printf("%hhu\n", (*song)[i].key);
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