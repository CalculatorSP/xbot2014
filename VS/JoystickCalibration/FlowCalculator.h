#pragma once

#include "opencv2/opencv.hpp"

class FlowCalculator
{
    std::vector<cv::Mat> _frames;

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

    FlowCalculator();

    void depositFrame(const cv::Mat& frame);
    void calculate();
};
