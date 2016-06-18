#include "MoveDetector.h"

using namespace cv;

const Rect MoveDetector::_whiteRoi(55, 225, 100, 105);
const Rect MoveDetector::_blackRoi(485, 225, 100, 105);

bool MoveDetector::detectWhiteMove(const Mat& frame)
{
    Mat mathable;
    frame(_whiteRoi).convertTo(mathable, CV_32F);
    cvtColor(mathable, mathable, COLOR_BGR2GRAY);
    _longSmootherWhite.deposit(mathable.clone());
    _shortSmootherWhite.deposit(mathable.clone());

    if (_longSmootherWhite.isFull() && _checkForMove(_longSmootherWhite, _shortSmootherWhite))
    {
        _longSmootherWhite.reset();
        _shortSmootherWhite.reset();
        return true;
    }

    return false;
}

bool MoveDetector::detectBlackMove(const Mat& frame)
{
    Mat mathable;
    frame(_blackRoi).convertTo(mathable, CV_32F);
    cvtColor(mathable, mathable, COLOR_BGR2GRAY);
    _longSmootherBlack.deposit(mathable.clone());
    _shortSmootherBlack.deposit(mathable.clone());

    if (_longSmootherBlack.isFull() && _checkForMove(_longSmootherBlack, _shortSmootherBlack))
    {
        _longSmootherBlack.reset();
        _shortSmootherBlack.reset();
        return true;
    }

    return false;
}

bool MoveDetector::_checkForMove(const WindowedStats<Mat, LONG_AVG>& longSmoother, const WindowedStats<Mat, SHORT_AVG>& shortSmoother)
{
    return (countNonZero(abs(shortSmoother.getMean() - longSmoother.getMean()) > 25.0) > 40);
}