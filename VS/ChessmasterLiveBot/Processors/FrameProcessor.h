#pragma once

#include "MoveDetector.h"
#include "Collections/WindowedStats.h"
#include "opencv2/opencv.hpp"

#define GAMEBOARD_WINDOW	(31)

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;
	static const Rect _squareRoi;

	WindowedStats<Mat, GAMEBOARD_WINDOW> _gameBoard;
	Mat _prevBoard;
	MoveDetector _moveDetector;

	int _count = 0;

public:
	FrameProcessor()
	{ }

	void process(const Mat& frame, Mat& result);

};