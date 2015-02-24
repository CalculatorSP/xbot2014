#pragma once

#include "PSX/XboxController.h"
#include "Processors/Stockfish/types.h"

class ChessActuator
{
	XboxController _xboxController;
	Square _currentSquare;

	void _goHome();
	void _goToSquare(Square sq);

public:
	ChessActuator(const char *comport)
		: _xboxController(comport), _currentSquare(SQ_A1)
	{ }

	void doMove(Move move);

};