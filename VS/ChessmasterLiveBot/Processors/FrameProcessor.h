#include "opencv2/opencv.hpp"

// Area where Black's moves are drawn
#define ROI_BLACK_LEFT		(560)
#define ROI_BLACK_RIGHT		(660)
#define ROI_BLACK_TOP		(225)
#define ROI_BLACK_BOTTOM	(330)

// Area where White's moves are drawn
#define ROI_WHITE_LEFT		(55)
#define ROI_WHITE_RIGHT		(155)
#define ROI_WHITE_TOP		(225)
#define ROI_WHITE_BOTTOM	(330)

using namespace cv;

class FrameProcessor
{
public:
	FrameProcessor();

	Mat process(const Mat& frame);

private:
	static const Rect roiBlack;
	static const Rect roiWhite;

};