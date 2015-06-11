#include "Processing.h"

#define KEY_ESC	(27)

void JoystickCalibrationAppManager::processFrame(cv::Mat& frame)
{
	imshow("result", frame);
}

void JoystickCalibrationAppManager::handleKey(int key)
{
	switch (key)
	{
	case KEY_ESC:
		_keepGoing = false;
		return;

	case 'g':
		// DO STUFF
		return;

	default:
		return;
	}
}

void JoystickCalibrationAppManager::run()
{
	while (_keepGoing)
		_scheduler->run();
}
