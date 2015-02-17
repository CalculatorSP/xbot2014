#pragma once

#include "Collections/WindowedStats.h"

#include "opencv2/opencv.hpp"

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;
	static const Rect _squareRoi;

	WindowedStats<Mat, 20> frameBuffer;

public:
	FrameProcessor()
	{ }

	void process(const Mat& frame, Mat& result);

};