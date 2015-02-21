#pragma once

#include "Processors/FrameProcessor.h"

#include "opencv2/opencv.hpp"

using namespace cv;

class ChessManager
{
	FrameProcessor _frameProcessor;

public:
	ChessManager();
	~ChessManager();
	void depositFrame(const Mat& frame, Mat& result);	

};