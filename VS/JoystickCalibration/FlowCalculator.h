#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

class FlowCalculator
{
    vector<Mat> _frames;
    SVD _svd;

    // Global Params
    const float _globalRadius;

    // Feature Params
    const int _maxCorners;
    const double _qualityLevel;
    const int _minDistance;
    const int _blockSize;

    // LK Params
    const Size _winSize;
    const int _maxLevel;
    const TermCriteria _termCrit;

    void _computeRotationMatrix(const vector<Point3f>& r0, const vector<Point3f>& r1, Mat& r);

public:
    vector<Mat> rodriguesVectors;

    FlowCalculator(int screenWidth, float fieldOfViewDegrees);

    void depositFrame(const Mat& frame);
    void calculate();
};
