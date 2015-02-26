#include "ChessActuator.h"

void ChessActuator::doMove(Move move)
{
	// Make sure we start at a known position
	_goHome();

	// Go to desired piece and pick it up
	_goToSquare(from_sq(move));
	_xboxController.tap(XboxController::A, 6);
	Sleep(1000);

	// Go to destination square and drop piece
	_goToSquare(to_sq(move));
	_xboxController.tap(XboxController::A, 6);
	Sleep(1000);
}

void ChessActuator::_goHome()
{
	// Move to A1
	for (int i = 0; i < 8; ++i)
	{
		_xboxController.tap(XboxController::LEFT, 6);
		_xboxController.tap(XboxController::DOWN, 6);
		Sleep(1000);
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
			_xboxController.tap(XboxController::RIGHT, 6);
			--deltaX;
		}
		if (deltaX < 0)
		{
			_xboxController.tap(XboxController::LEFT, 6);
			++deltaX;
		}
		if (deltaY > 0)
		{
			_xboxController.tap(XboxController::UP, 6);
			--deltaY;
		}
		if (deltaY < 0)
		{
			_xboxController.tap(XboxController::DOWN, 6);
			++deltaY;
		}
		Sleep(1000);
	}

	_currentSquare = sq;
}