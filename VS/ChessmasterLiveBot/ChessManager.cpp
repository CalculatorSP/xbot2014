#include "ChessManager.h"

ChessManager::ChessManager()
{

}

Mat ChessManager::depositFrame(const Mat& frame)
{
	Mat processedFrame = _frameProcessor.process(frame);

	return processedFrame;
}