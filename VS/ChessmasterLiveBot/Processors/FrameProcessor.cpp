#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);
const Rect FrameProcessor::_squareRoi(218, 115, (unsigned char)(282 / 8), (unsigned char)(256 / 8));
const float FrameProcessor::_kernel[3] = { 1.0f, 1.0f, 1.0f };

void FrameProcessor::process(const Mat& frame, Mat& result)
{
	//frameBuffer.deposit(frame(_gameBoardRoi).clone());
	//result = frameBuffer.getMean().clone();
}