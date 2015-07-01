#pragma once

#include "Math/GlobalMath.h"
#include "opencv2/opencv.hpp"

using namespace cv;

class FlowCalculator
{
    vector<Mat> _frames;

    // Global Params
    const GlobalRadius _globalRadius;

    // Feature Params
    const int _maxCorners;
    const double _qualityLevel;
    const int _minDistance;
    const int _blockSize;

    // LK Params
    const Size _winSize;
    const int _maxLevel;
    const TermCriteria _termCrit;

    template<typename T>
    static T _getMedian(vector<T>& vals);

public:
    vector<Point2f> gammaAlphaRotations;

    FlowCalculator(int screenWidth, float fieldOfViewDegrees);

    void depositFrame(const Mat& frame);
    void calculate();
};
