#pragma once

#include "ChessActuator.h"
#include "Processors/FrameProcessor.h"
#include "Processors/Stockfish/bitboard.h"
#include "Processors/Stockfish/evaluate.h"
#include "Processors/Stockfish/position.h"
#include "Processors/Stockfish/search.h"
#include "Processors/Stockfish/thread.h"
#include "Processors/Stockfish/tt.h"
#include "Processors/Stockfish/uci.h"
#include "Processors/Stockfish/syzygy/tbprobe.h"
#include "opencv2/opencv.hpp"
#include <time.h>

using namespace cv;

class ChessManager
{
	// FEN string of the initial position, normal chess
	const char* _StartFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	// Stack to keep track of the position states along the setup moves (from the
	// start position to the position just before the search starts). Needed by
	// 'draw by repetition' detection.
	Search::StateStackPtr _SetupStates;

	bool _inGame;
	ChessActuator _chessActuator;
	FrameProcessor _frameProcessor;
	Position _pos;
	String _moveStr;

	std::stringstream _sfIn;
	std::stringstream _sfOut;
	std::streambuf* _cout_backup;

	time_t _whiteStartTime;
	time_t _blackStartTime;

public:
	ChessManager(const ChessActuator& chessActuator);
	~ChessManager();

	// Process a game frame
	void depositFrame(const Mat& frame);

	// Start a new chess game
	void reset();

	// End current game (if game is going)
	void endGame();

};