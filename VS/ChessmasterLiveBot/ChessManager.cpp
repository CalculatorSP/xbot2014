#include "ChessManager.h"

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

	_pos = Position(_StartFEN, false, Threads.main());
	_SetupStates = Search::StateStackPtr(new std::stack<StateInfo>());
}

ChessManager::~ChessManager()
{
	Threads.exit();
}

void ChessManager::depositFrame(const Mat& frame)
{
	Mat probabilities(8, 8, CV_32F);

	// Check if a move was made
	if (_frameProcessor.checkForMoves(frame, probabilities))
	{
		// Threshold for detecting moves
		float maxProb = 150.0f;
		Move mostLikelyMove = MOVE_NONE;

		// Loop through all legal moves and find the one most consistent with our image processing
		for (MoveList<LEGAL> it(_pos); *it; ++it)
		{
			Move m = *it;
			if (m == MOVE_NONE || m == MOVE_NULL)
				continue;

			Square from = from_sq(*it);
			Square to = to_sq(*it);

			if (type_of(m) == CASTLING)
			{
				to = make_square(to > from ? FILE_G : FILE_C, rank_of(from));
				if (probabilities.at<float>(rank_of(~to), file_of(~to)) > 20.0 && \
					probabilities.at<float>(rank_of(~from), file_of(~from)) > 20.0 &&\
					probabilities.at<float>(rank_of(~to), file_of(~to) + 1) > 20.0)
				{
					maxProb = 1000.0;
					mostLikelyMove = m;
					break;
				}
			}

			float moveProb = probabilities.at<float>(rank_of(~to), file_of(~to)) * probabilities.at<float>(rank_of(~from), file_of(~from));
			if (moveProb >= maxProb)
			{
				maxProb = moveProb;
				mostLikelyMove = m;
			}
		}

		if (maxProb > 5000.0)
			mostLikelyMove = MOVE_NONE;

		if (mostLikelyMove != MOVE_NONE)
		{
			_SetupStates->push(StateInfo());
			_pos.do_move(mostLikelyMove, _SetupStates->top());
		}

		std::cout << maxProb << " " << UCI::move(mostLikelyMove, _pos.is_chess960()) << std::endl;
	}
}