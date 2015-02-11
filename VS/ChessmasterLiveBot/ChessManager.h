#include "Processors/FrameProcessor.h"

#include "opencv2/opencv.hpp"

using namespace cv;

class ChessManager
{
public:
	ChessManager();

	Mat depositFrame(const Mat& frame);

private:
	FrameProcessor _frameProcessor;

};