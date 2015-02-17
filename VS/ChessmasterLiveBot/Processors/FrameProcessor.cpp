#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);
const Rect FrameProcessor::_squareRoi(218, 115, (unsigned char)(282 / 8), (unsigned char)(256 / 8));

const float FrameProcessor::_gaussianWeights[3] = { 0.157731f, 0.684538f, 0.157731f };
const float FrameProcessor::_derivativeWeights[3] = { 1.0f, 0.0f, -1.0f };

const ModBuffer<float, 3> FrameProcessor::_gaussianKernel(_gaussianWeights);
const ModBuffer<float, 3> FrameProcessor::_derivativeKernel(_derivativeWeights);

void FrameProcessor::process(const Mat& frame, Mat& result)
{
	Mat mathable;
	frame(_gameBoardRoi).convertTo(mathable, CV_32F);
	_fullFilter.process(mathable).convertTo(result, CV_8U);
}