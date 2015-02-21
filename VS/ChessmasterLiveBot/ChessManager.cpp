#include "ChessManager.h"

#include "Processors/Stockfish/bitboard.h"
#include "Processors/Stockfish/evaluate.h"
#include "Processors/Stockfish/position.h"
#include "Processors/Stockfish/search.h"
#include "Processors/Stockfish/thread.h"
#include "Processors/Stockfish/tt.h"
#include "Processors/Stockfish/uci.h"
#include "Processors/Stockfish/syzygy/tbprobe.h"

ChessManager::ChessManager()
{
	std::cout << engine_info() << std::endl;

	UCI::init(Options);
	Bitboards::init();
	Position::init();
	Bitbases::init();
	Search::init();
	Eval::init();
	Pawns::init();
	Threads.init();
	Tablebases::init(Options["SyzygyPath"]);
	TT.resize(Options["Hash"]);

	UCI::loop(0, NULL);
}

ChessManager::~ChessManager()
{
	Threads.exit();
}

void ChessManager::depositFrame(const Mat& frame, Mat& result)
{
	_frameProcessor.process(frame, result);
}