#include "ChessManager.h"

#define GAME_TIME	(600)

ChessManager::ChessManager(const ChessActuator& chessActuator)
	: _chessActuator(chessActuator), _inGame(false)
{
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

	// Redirect cout so we can talk to stockfish more easily
	_cout_backup = std::cout.rdbuf(_sfOut.rdbuf());
}

ChessManager::~ChessManager()
{
	// Stop processing
	endGame();

	// Restore cout
	std::cout.rdbuf(_cout_backup);
}

void ChessManager::depositFrame(const Mat& frame)
{
	if (!_inGame)
		return;

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

			// Check special case for castling
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

			// Probability of move depends on amount of change in 2 squares involved
			float moveProb = probabilities.at<float>(rank_of(~to), file_of(~to)) * probabilities.at<float>(rank_of(~from), file_of(~from));
			if (moveProb >= maxProb)
			{
				maxProb = moveProb;
				mostLikelyMove = m;
			}
		}

		// Too much change means it was probably just noise
		if (maxProb > 5000.0)
			mostLikelyMove = MOVE_NONE;

		printf("%f\t%s\n", maxProb, UCI::move(mostLikelyMove, _pos.is_chess960()).c_str());

		// Process the move, if it was validated
		if (mostLikelyMove != MOVE_NONE)
		{
			// Set the position in our private chess pos
			_SetupStates->push(StateInfo());
			_pos.do_move(mostLikelyMove, _SetupStates->top());
			_moveStr += " " + UCI::move(mostLikelyMove, _pos.is_chess960());

			// If this is the first move, start the timer
			if (_whiteStartTime == 0)
				_whiteStartTime = 1000 * GAME_TIME + 1000 * time(NULL);
			if (_blackStartTime == 0)
				_blackStartTime = 1000 * GAME_TIME + 1000 * time(NULL);

			// Set the position in stockfish
			UCI::loop("position startpos moves" + _moveStr);
			char response[1000];
			while (_sfOut.getline(response, sizeof(response)))
				printf("%s\n", response);
			_sfOut.clear();

			// If it's our turn, look up the best move
			if (_pos.side_to_move() == WHITE)
			{
				// Send "go" command to stockfish
				std::ostringstream oss;
				oss << "go";
				//oss << "go wtime " << _whiteStartTime - 1000 * time(NULL) << "btime " << _blackStartTime + 1000 * time(NULL);
				UCI::loop(oss.str());
				String token, bestmove, ponder;
				while (true)
				{
					// Wait for a response
					while (!_sfOut.getline(response, sizeof(response)))
						_sfOut.clear();

					printf("%s\n", response);
					std::istringstream is(response);
					is >> std::skipws >> token;
					if (token == "bestmove")
					{
						is >> bestmove >> token;
						if (token == "ponder")
							is >> ponder;
						break;
					}
				}

				// Execute the best move
				_chessActuator.doMove(UCI::to_move(_pos, bestmove));
			}
		}
	}
}

void ChessManager::reset()
{
	_inGame = true;

	// Keep track of position independently of stockfish, so we can check for valid moves
	_pos = Position(_StartFEN, false, Threads.main());
	_moveStr = "";

	_SetupStates.release();
	_SetupStates = Search::StateStackPtr(new std::stack<StateInfo>());

	// Start stockfish
	UCI::loop("uci");
	char response[1000];
	while (_sfOut.getline(response, sizeof(response)))
		printf("%s\n", response);
	_sfOut.clear();

	UCI::loop("isready");
	while (_sfOut.getline(response, sizeof(response)))
		printf("%s\n", response);
	_sfOut.clear();

	UCI::loop("ucinewgame");
	while (_sfOut.getline(response, sizeof(response)))
		printf("%s\n", response);
	_sfOut.clear();

	// Initialize start times
	_whiteStartTime = 0;
	_blackStartTime = 0;

	// Get first move for white
	UCI::loop("position startpos moves" + _moveStr);
	while (_sfOut.getline(response, sizeof(response)))
		printf("%s\n", response);
	_sfOut.clear();

	UCI::loop("go");
	//UCI::loop("go wtime 1000 * GAME_TIME btime 1000 * GAME_TIME");
	String token, bestmove, ponder;
	while (true)
	{
		// Wait for a response
		while (!_sfOut.getline(response, sizeof(response)))
			_sfOut.clear();

		printf("%s\n", response);
		std::istringstream is(response);
		is >> std::skipws >> token;
		if (token == "bestmove")
		{
			is >> bestmove >> token;
			if (token == "ponder")
				is >> ponder;
			break;
		}
	}

	// Execute the best move
	_chessActuator.doMove(UCI::to_move(_pos, bestmove));
}

void ChessManager::endGame()
{
	// Quit stockfish
	UCI::loop("quit");
	Threads.wait_for_think_finished(); // Cannot quit whilst the search is running
	Threads.exit();

	_inGame = false;
}
