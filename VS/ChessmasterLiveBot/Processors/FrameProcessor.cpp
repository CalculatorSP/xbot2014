#include "FrameProcessor.h"

const Rect FrameProcessor::_gameBoardRoi(218, 115, 282, 256);

bool FrameProcessor::checkForMoves(const Mat& frame, Mat& probabilities)
{
	if (_moveDetector.detectWhiteMove(frame))
	{
		_prevBoard = _gameBoard.getMean();
		_gameBoard.reset();
		_lookingForMove = true;
	}

	if (_moveDetector.detectBlackMove(frame))
	{
		_prevBoard = _gameBoard.getMean();
		_gameBoard.reset();
		_lookingForMove = true;
	}

	if (!_gameBoard.isFull())
	{
		Mat mathable;
		frame(_gameBoardRoi).convertTo(mathable, CV_32F);
		_gameBoard.deposit(mathable.clone());
		if (_lookingForMove && _gameBoard.isFull())
		{
			_getMoveProbabilities(probabilities);
			_lookingForMove = false;
			return true;
		}
	}

	return false;
}

void FrameProcessor::_getMoveProbabilities(Mat &probabilities) const
{
	Mat channels[3];
	split(abs(_gameBoard.getMean() - _prevBoard), channels);

	Mat diff = min(min(channels[0], channels[1]), channels[2]);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			Rect gameSquare(i * _gameBoardRoi.width / 8, j * _gameBoardRoi.height / 8, _gameBoardRoi.width / 8, _gameBoardRoi.height / 8);
			probabilities.at<float>(j, i) = (float)(norm(diff(gameSquare), NORM_L1) / gameSquare.area());
		}
	}
}

