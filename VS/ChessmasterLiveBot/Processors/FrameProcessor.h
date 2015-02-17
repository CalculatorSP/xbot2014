#pragma once

#include "Collections/FirFilter.h"
#include "Collections/ModBuffer.h"

#include "opencv2/opencv.hpp"

using namespace cv;

class FrameProcessor
{
	static const Rect _gameBoardRoi;
	static const Rect _squareRoi;

	static const float _gaussianWeights[3];
	static const float _derivativeWeights[3];

	static const ModBuffer<float, 3> _gaussianKernel;
	static const ModBuffer<float, 3> _derivativeKernel;

	FirFilter<Mat, 5> _fullFilter;

public:
	FrameProcessor()
	{
		ModBuffer<float, 5> fullKernel;
		ModBuffer<float, 3>::convolve<float, 3>(fullKernel, _gaussianKernel, _derivativeKernel);

		_fullFilter.setKernel(fullKernel);
	}

	void process(const Mat& frame, Mat& result);

};