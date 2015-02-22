#pragma once

#include "MoveDetector.h"
#include "Collections/WindowedStats.h"
#include "opencv2/opencv.hpp"

#define GAMEBOARD_WINDOW	(11)

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;

	WindowedStats<Mat, GAMEBOARD_WINDOW> _gameBoard;
	Mat _prevBoard;

	MoveDetector _moveDetector;

	void _getMoveProbabilities(Mat &probabilities) const;
	bool _lookingForMove;

public:
	FrameProcessor()
		: _lookingForMove(false)
	{ }

	bool checkForMoves(const Mat& frame, Mat& result);

};