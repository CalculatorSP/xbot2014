#pragma once

#include "Collections/WindowedStats.h"
#include "opencv2/opencv.hpp"

#define LONG_AVG	(91)
#define SHORT_AVG	(7)

using namespace cv;

class MoveDetector
{
	static const Rect _whiteRoi;
	static const Rect _blackRoi;

	WindowedStats<Mat, LONG_AVG> _longSmootherWhite;
	WindowedStats<Mat, SHORT_AVG> _shortSmootherWhite;
	WindowedStats<Mat, LONG_AVG> _longSmootherBlack;
	WindowedStats<Mat, SHORT_AVG> _shortSmootherBlack;

	static bool _checkForMove(const WindowedStats<Mat, LONG_AVG>& _longSmoother, const WindowedStats<Mat, SHORT_AVG>& _shortSmoother);

public:
	MoveDetector()
	{ }

	bool detectWhiteMove(const Mat& frame);
	bool detectBlackMove(const Mat& frame);

};