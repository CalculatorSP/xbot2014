#pragma once

#include "opencv2/opencv.hpp"

using namespace cv;

class RedTriangleHunter
{
    Mat _template;
    const Rect _searchRegion;
    const Point _templateCenter;

public:
    RedTriangleHunter();

    bool findTarget(Mat& frame, Point2f& target, bool modifyFrame);

};
