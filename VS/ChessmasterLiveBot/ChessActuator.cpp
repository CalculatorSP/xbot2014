#include "ChessActuator.h"

void ChessActuator::doMove(Move move)
{
	// Make sure we start at a known position
	_goHome();

	// Go to desired piece and pick it up
	_goToSquare(from_sq(move));
	_xboxController.tap(XboxController::A);
	Sleep(125);

	// Go to destination square and drop piece
	_goToSquare(to_sq(move));
	_xboxController.tap(XboxController::A);
	Sleep(125);
}

void ChessActuator::_goHome()
{
	// Move to A1
	for (int i = 0; i < 8; ++i)
	{
		_xboxController.tap(XboxController::LEFT);
		_xboxController.tap(XboxController::DOWN);
		Sleep(125);
	}

	_currentSquare = SQ_A1;
}

void ChessActuator::_goToSquare(Square sq)
{
	int deltaX = file_of(sq) - file_of(_currentSquare);
	int deltaY = rank_of(sq) - file_of(_currentSquare);

	// Move towards target square
	while (!(deltaX == 0 && deltaY == 0))
	{
		if (deltaX > 0)
		{
			_xboxController.tap(XboxController::RIGHT);
			--deltaX;
		}
		if (deltaX < 0)
		{
			_xboxController.tap(XboxController::LEFT);
			++deltaX;
		}
		if (deltaY > 0)
		{
			_xboxController.tap(XboxController::UP);
			--deltaY;
		}
		if (deltaY < 0)
		{
			_xboxController.tap(XboxController::DOWN);
			++deltaY;
		}
		Sleep(125);
	}
}