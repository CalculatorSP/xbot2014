#include "ChessManager.h"

ChessManager::ChessManager()
{

}

void ChessManager::depositFrame(const Mat& frame, Mat& result)
{
	_frameProcessor.process(frame, result);
}