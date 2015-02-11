#include "opencv2/opencv.hpp"

using namespace cv;

class FrameProcessor
{
public:
	FrameProcessor();

	Mat process(const Mat& frame);

private:
	static const Rect _gameBoardRoi;

};