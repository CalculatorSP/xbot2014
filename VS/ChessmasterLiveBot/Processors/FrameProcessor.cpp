#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);

FrameProcessor::FrameProcessor()
{

}

Mat FrameProcessor::process(const Mat& frame)
{
	Mat workingCopy = frame.clone();

	Mat gameBoard(workingCopy, _gameBoardRoi);

	return gameBoard;
}