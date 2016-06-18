#pragma once

#include "opencv2/opencv.hpp"

class RedTriangleHunter
{
    cv::Mat _template;
    const cv::Rect _searchRegion;
    const cv::Point _templateCenter;

public:
    RedTriangleHunter();

    bool findTarget(cv::Mat& frame, cv::Point& target, bool modifyFrame);

};
