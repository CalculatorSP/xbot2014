#pragma once

#include "opencv2/opencv.hpp"

#include "Collections/WindowedStats.h"

#define LONG_AVG    (91)
#define SHORT_AVG   (7)

class MoveDetector
{
    static const cv::Rect _whiteRoi;
    static const cv::Rect _blackRoi;

    WindowedStats<cv::Mat, LONG_AVG> _longSmootherWhite;
    WindowedStats<cv::Mat, SHORT_AVG> _shortSmootherWhite;
    WindowedStats<cv::Mat, LONG_AVG> _longSmootherBlack;
    WindowedStats<cv::Mat, SHORT_AVG> _shortSmootherBlack;

    static bool _checkForMove(const WindowedStats<cv::Mat, LONG_AVG>& _longSmoother, const WindowedStats<cv::Mat, SHORT_AVG>& _shortSmoother);

public:
    MoveDetector()
    { }

    bool detectWhiteMove(const cv::Mat& frame);
    bool detectBlackMove(const cv::Mat& frame);

};