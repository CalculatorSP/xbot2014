ODIR = obj
SDIR = src

xbotCommon:
	g++ -c -I./Common ./Common/PSXMAC/PlaystationController.cpp -o ./bin/PlaystationController.o
	g++ -c -I./Common ./Common/PSXMAC/XboxController.cpp -o ./bin/XboxController.cpp
	ar rvs xbotCommon PlaystationController.o XboxController.o	

chess:
	g++ -o xbotChessmaster ./ChessmasterLiveBot/*.cpp ./ChessmasterLiveBot/Processors/*.cpp ./ChessmasterLiveBot/Processors/Stockfish/*.cpp ./ChessmasterLiveBot/Processors/Stockfish/syzygy/*.cpp -I./ChessmasterLiveBot -I./Common