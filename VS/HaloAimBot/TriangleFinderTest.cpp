#include <iostream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define WEBCAM      (0)
#define CAPCARD     (1)
#define TMPLFILE    "enemy_arrow2.png"

using namespace cv;

static void getKernel(Mat& kernel, Mat& mask);
static void makeOneChannel(const Mat& src, Mat& dst);
static void matchTemplateWithMask(const Mat& src, const Mat& tmpl, const Mat& mask, Mat& result);

int main(int argc, const char **argv)
{
    VideoCapture cap(CAPCARD);
    Mat frame, proc, result, kernel, mask;

    namedWindow("orig", WINDOW_AUTOSIZE);
    namedWindow("result", WINDOW_AUTOSIZE);
    namedWindow("kernel", WINDOW_AUTOSIZE);
    namedWindow("mask", WINDOW_AUTOSIZE);

    getKernel(kernel, mask);
    imshow("kernel", kernel);
    imshow("mask", mask);

    int64 lastTime = getTickCount();

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        //resize(frame, frame, Size(), 640.0 / 1280.0, 480.0 / 720.0, INTER_NEAREST);
        makeOneChannel(frame, proc);
        matchTemplateWithMask(proc, kernel, mask, result);
        double minval, maxval;
        Point minloc, maxloc;
        minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);
        circle(frame, minloc + Point(13, 16), 4, Scalar(0, 255, 0), -1);

        std::cout << minval << ", ";

        imshow("orig", frame);
        imshow("result", (result - minval) / (maxval - minval));
        switch (waitKey(1))
        {
        case 'p':
            if (waitKey() == 27)
                return 0;
            break;
        case 27:
            return 0;
        default:
            break;
        }

        int64 curTime = getTickCount();
        std::cout << 1000.0 * (curTime - lastTime) / getTickFrequency() << std::endl;
        lastTime = curTime;
    }

    return 0;
}

static void getKernel(Mat& kernel, Mat& mask)
{
    kernel = imread(TMPLFILE);
    std::vector<Mat> chans;
    split(kernel, chans);
    threshold(chans[0], mask, 0, 0, THRESH_BINARY);
    mask += 255;
    makeOneChannel(kernel, kernel);
}

static void makeOneChannel(const Mat& src, Mat& dst)
{
    Mat tmp = src;
    std::vector<Mat> chans;
    split(tmp, chans);
    chans[2] = 255 - chans[2];
    merge(chans, dst);
    dst = min(max(max(chans[0], chans[1]), chans[2]), 170);
}

static void matchTemplateWithMask(const Mat& src, const Mat& tmpl, const Mat& mask, Mat& result)
{
    Mat i, t, w, r1, r2;
    src.convertTo(i, CV_32F, 1.0 / 255.0);
    tmpl.convertTo(t, CV_32F, 1.0 / 255.0);
    mask.convertTo(w, CV_32F, 1.0 / 255.0);

    static Scalar adj = sum(w.mul(t.mul(t)));

    matchTemplate(i.mul(i), w, r1, TM_CCORR);
    matchTemplate(i, w.mul(t, 2.0), r2, TM_CCORR);
    result = r1 - r2 + adj;
}
