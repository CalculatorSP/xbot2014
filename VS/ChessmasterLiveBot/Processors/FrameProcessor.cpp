#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);
const Rect FrameProcessor::_squareRoi(218, 115, (unsigned char)(282 / 8), (unsigned char)(256 / 8));

void FrameProcessor::process(const Mat& frame, Mat& result)
{
	if (!_gameBoard.isFull())
	{	
		Mat mathable;
		frame(_gameBoardRoi).convertTo(mathable, CV_32F);
		_gameBoard.deposit(mathable.clone());
		if (_gameBoard.isFull())
		{
			Mat diff = abs(_gameBoard.getMean() - _prevBoard);
			diff.convertTo(result, CV_8U);
		}
	}

	if (_moveDetector.detectWhiteMove(frame))
	{
		printf("White move was made! %d\n", _count++);
		_prevBoard = _gameBoard.getMean();
		_gameBoard.reset();
	}

	if (_moveDetector.detectBlackMove(frame))
	{
		printf("Black move was made! %d\n", _count++);
		_prevBoard = _gameBoard.getMean();
		_gameBoard.reset();
	}

}

