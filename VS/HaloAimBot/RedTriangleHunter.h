#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

class RedTriangleHunter
{

public:
    RedTriangleHunter();

    bool findTarget(const Mat& frame, Point2f& target);

};
