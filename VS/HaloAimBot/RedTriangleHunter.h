#pragma once

#include "opencv2/opencv.hpp"

class RedTriangleHunter
{
    cv::Mat _template;

    static void _makeOneChannel(const cv::Mat& src, cv::Mat& dst);

public:
    RedTriangleHunter();

    bool findTarget(const cv::Mat& frame, cv::Point& target) const;

};
