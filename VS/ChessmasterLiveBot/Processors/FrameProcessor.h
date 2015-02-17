#pragma once

#include "Collections/FirFilter.h"

#include "opencv2/opencv.hpp"

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;
	static const Rect _squareRoi;
	static const float _kernel[3];
	FirFilter<Mat, 3> _frameBuffer;

public:
	FrameProcessor()
		: _frameBuffer(_kernel)
	{ }

	void process(const Mat& frame, Mat& result);

};