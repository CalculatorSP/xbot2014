#include "FrameProcessor.h"

const Rect FrameProcessor::roiBlack(Point(ROI_BLACK_LEFT, ROI_BLACK_TOP), Point(ROI_BLACK_RIGHT, ROI_BLACK_BOTTOM));
const Rect FrameProcessor::roiWhite(Point(ROI_WHITE_LEFT, ROI_WHITE_TOP), Point(ROI_WHITE_RIGHT, ROI_WHITE_BOTTOM));

FrameProcessor::FrameProcessor()
{

}

Mat FrameProcessor::process(const Mat& frame)
{
	Mat workingCopy = frame.clone();

	Mat blackMoves(frame, roiBlack);
	Mat whiteMoves(frame, roiWhite);

	rectangle(workingCopy, roiBlack, Scalar(0, 0, 0, 255));
	rectangle(workingCopy, roiWhite, Scalar(255, 255, 255, 255));

	return workingCopy;
}