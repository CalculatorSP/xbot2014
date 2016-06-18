#pragma once

#include "opencv2/opencv.hpp"

#include "Math/GlobalMath.h"

class FlowCalculator
{
    std::vector<cv::Mat> _frames;

    // Global Params
    const GlobalRadius _globalRadius;

    // Feature Params
    const int _maxCorners;
    const double _qualityLevel;
    const int _minDistance;
    const int _blockSize;

    // LK Params
    const cv::Size _winSize;
    const int _maxLevel;
    const cv::TermCriteria _termCrit;

    template<typename T>
    static T _getMedian(std::vector<T>& vals);

public:
    std::vector<cv::Point2f> gammaAlphaRotations;

    FlowCalculator(int screenWidth, float fieldOfViewDegrees);

    void depositFrame(const cv::Mat& frame);
    void calculate();
};
