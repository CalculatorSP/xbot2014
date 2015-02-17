#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;
	static const Rect _squareRoi;
	int _counter;

	Mat _lastFrameBoard;

public:
	FrameProcessor() : _counter(0)
	{ }

	void process(const Mat& frame, Mat& result);

};