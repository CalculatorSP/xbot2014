#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);
const Rect FrameProcessor::_squareRoi(218, 115, (unsigned char)(282 / 8), (unsigned char)(256 / 8));

void FrameProcessor::process(const Mat& frame, Mat& result)
{
	if (!(_counter % 60))
	{
		Mat gameBoard = frame(_gameBoardRoi);

		if (!_lastFrameBoard.empty())
		{
			result = abs(gameBoard - _lastFrameBoard);
		}
		else
			result.create(gameBoard.rows, gameBoard.cols, CV_8UC3);

		_lastFrameBoard = gameBoard.clone();
	}
	_counter++;
}