#include "ChessActuator.h"

void ChessActuator::doMove(Move move)
{
	// Go to desired piece and pick it up
	_goToSquare(from_sq(move));
	_xboxController.tap(XboxButton::A);
	Sleep(500);

	// Go to destination square and drop piece
	_goToSquare(to_sq(move));
	_xboxController.tap(XboxButton::A);
	Sleep(500);
}

void ChessActuator::goHome()
{
	// Move to A1
	for (int i = 0; i < 8; ++i)
	{
		_xboxController.tap(XboxButton::LEFT);
		_xboxController.tap(XboxButton::DOWN);
		Sleep(500);
	}

	_currentSquare = SQ_A1;
}

void ChessActuator::_goToSquare(Square sq)
{
	int x1 = file_of(_currentSquare);
	int x2 = file_of(sq);
	int y1 = rank_of(_currentSquare);
	int y2 = rank_of(sq);
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;

	// Move towards target square
	while (!(deltaX == 0 && deltaY == 0))
	{
		if (deltaX > 0)
		{
			_xboxController.tap(XboxButton::RIGHT);
			--deltaX;
		}
		if (deltaX < 0)
		{
			_xboxController.tap(XboxButton::LEFT);
			++deltaX;
		}
		if (deltaY > 0)
		{
			_xboxController.tap(XboxButton::UP);
			--deltaY;
		}
		if (deltaY < 0)
		{
			_xboxController.tap(XboxButton::DOWN);
			++deltaY;
		}
		Sleep(500);
	}

	_currentSquare = sq;
}