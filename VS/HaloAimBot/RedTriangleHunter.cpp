#include <stdint.h>

#include "RedTriangleHunter.h"

#define TMPLFILE        "C:/Users/John/Git/xbot2014/VS/HaloAimBot/enemy_arrow2.png"
#define DETECT_THRESH   (0.04)

using namespace cv;

RedTriangleHunter::RedTriangleHunter()
{
    _makeOneChannel(imread(TMPLFILE), _template);
}

bool RedTriangleHunter::findTarget(const Mat& frame, Point& target) const
{
    Mat proc;
    _makeOneChannel(frame, proc);
    matchTemplate(proc, _template, proc, TM_SQDIFF_NORMED);

    double minval;
    minMaxLoc(proc, &minval, NULL, &target, NULL);
    
    target += Point(13, 16);

    return minval < DETECT_THRESH;
}

void RedTriangleHunter::_makeOneChannel(const Mat& src, Mat& dst)
{
    Mat tmp = src;
    std::vector<Mat> chans;
    split(tmp, chans);
    chans[2] = 255 - chans[2];
    dst = min(max(max(chans[0], chans[1]), chans[2]), 170);
}
