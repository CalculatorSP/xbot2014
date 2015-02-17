#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);
const Rect FrameProcessor::_squareRoi(218, 115, (unsigned char)(282 / 8), (unsigned char)(256 / 8));

void FrameProcessor::process(const Mat& frame, Mat& result)
{
	frameBuffer.deposit(frame(_gameBoardRoi).clone());
	result = frameBuffer.getMean().clone();
}