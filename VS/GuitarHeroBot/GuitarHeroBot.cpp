//#include "opencv2/opencv.hpp"
#include "Task/Scheduler.h"
#include "MidiParser.h"
#include <iostream>
#include <Windows.h>
#include "PSX/XboxController.h"

#define WEBCAM	(0)
#define CAPCARD	(1)

//using namespace cv;

const char* filename = "C:/Users/John/Source/Repos/xbot2014/VS/GuitarHeroBot/songs/Through the Fire and Flames.mid";

int main(int argc, const char **argv)
{
	//VideoCapture cap(CAPCARD);

	//if (!cap.isOpened())
	//{
	//	std::cerr << "Could not open capture device" << std::endl;
	//	return -1;
	//}

	XboxController xboxController("COM4");

	Song* song = MidiParser::parseMidi(filename, GUITAR, EXPERT);
	if (song == NULL)
	{
		std::cerr << "Could not open file " << filename << std::endl;
		return -1;
	}

	Scheduler scheduler;
	int foo;
	scanf_s("%d", &foo);

	uint64_t microsOffset = scheduler.getTime();
	for (int i = 0; i < song->size; ++i)
	{
		if ((*song)[i].type == PREPARE)
		{
			switch ((*song)[i].key)
			{
			case GREEN:
				if ((*song)[i].press)
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::LT_D);
				else
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::LT_D);
				break;
			case RED:
				if ((*song)[i].press)
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::LB);
				else
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::LB);
				break;
			case YELLOW:
				if ((*song)[i].press)
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::RB);
				else
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::RB);
				break;
			case BLUE:
				if ((*song)[i].press)
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::RT_D);
				else
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::RT_D);
				break;
			case ORANGE:
				if ((*song)[i].press)
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::press, &xboxController, XboxController::A);
				else
					scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::release, &xboxController, XboxController::A);
				break;
			default:
				break;
			}
		}
		else
			scheduler.postAtTime(microsOffset + (*song)[i].timestamp, &XboxController::sendState, &xboxController, 2);

	}

	while (scheduler.run());

	return 0;
}