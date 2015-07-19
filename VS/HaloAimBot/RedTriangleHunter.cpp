#include <stdint.h>

#include "RedTriangleHunter.h"

#define RED_OPPOSITE        (180)
#define SCORE_THRESH        (160)
#define MAX_SEARCH_PIXELS   (3000)

RedTriangleHunter::RedTriangleHunter()
    :
    _template(imread("C:/Users/John/Source/Repos/xbot2014/VS/HaloAimBot/template.png", CV_LOAD_IMAGE_GRAYSCALE)),
    _searchRegion(50, 25, 220, 190),
    _templateCenter(23, 9)
{
    resize(_template, _template, Size(), 0.5, 0.5, INTER_NEAREST);
    threshold(_template, _template, 100.0, 255.0, CV_THRESH_BINARY);
}

bool RedTriangleHunter::findTarget(Mat& frame, Point& target, bool modifyFrame)
{
    Mat proc;

    // Decrease resolution to speed processing and eliminate interlacing artifacts
    resize(frame, proc, Size(), 0.5, 0.5, INTER_NEAREST);

    // Split into hsv components
    Mat hsv[3];
    cvtColor(proc, proc, CV_BGR2HSV);
    split(proc, hsv);

    // Filter hue channel
    absdiff(hsv[0], RED_OPPOSITE / 2, hsv[0]);
    blur(hsv[0], hsv[0], Size(2, 2));
    threshold(hsv[0], hsv[0], 70.0, 255.0, CV_THRESH_BINARY);

    // Filter saturation channel
    blur(hsv[1], hsv[1], Size(2, 2));
    threshold(hsv[1], hsv[1], 70.0, 255.0, CV_THRESH_BINARY);

    // Filter value channel
    blur(hsv[2], hsv[2], Size(2, 2));
    threshold(hsv[2], hsv[2], 70.0, 255.0, CV_THRESH_BINARY);

    // Mix filtered hsv channels
    bitwise_and(hsv[0], hsv[1], proc);
    bitwise_and(hsv[2], proc, proc);

    // Show filtered image if requested
    if (modifyFrame)
    {
        resize(proc, frame, Size(), 2.0, 2.0, INTER_NEAREST);
        cvtColor(frame, frame, CV_GRAY2BGR);
    }

    // Check if too many pixels are red
    if (countNonZero(proc(_searchRegion)) > MAX_SEARCH_PIXELS)
        return false;

    // Find template in filtered image
    int bestScore = SCORE_THRESH;
    for (int x = _searchRegion.tl().x; x < _searchRegion.br().x; ++x)
    {
        for (int y = _searchRegion.tl().y; y < _searchRegion.br().y; ++y)
        {
            Point p(x, y);

            if (!proc.at<uint8_t>(p))
                continue;

            Mat diff;
            bitwise_xor(proc(Rect(p - _templateCenter, Size(_template.cols, _template.rows))), _template, diff);
            int score = countNonZero(diff);
            if (score < bestScore)
            {
                bestScore = score;
                target = p + Point(0, 4);
            }
        }
    }

    target *= 2;

    return bestScore < SCORE_THRESH;
}