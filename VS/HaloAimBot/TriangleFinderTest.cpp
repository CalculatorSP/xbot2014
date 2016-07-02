#include <iostream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define WEBCAM      (0)
#define CAPCARD     (1)
#define TMPLFILE    "C:/Users/John/Git/xbot2014/VS/HaloAimBot/enemy_arrow2.png"

using namespace cv;

static void makeOneChannel(const Mat& src, Mat& dst);

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: HaloAimBot.exe video_file" << std::endl;
        return -1;
    }

    VideoCapture cap(argv[1]);
    if (!cap.isOpened())
    {
        std::cout << "Could not open capture " << argv[1] << std::endl;
        return -1;
    }

    Mat kernel;
    kernel = imread(TMPLFILE);
    if (kernel.empty())
    {
        std::cout << "Could not open template file " << TMPLFILE << std::endl;
        return -1;
    }
    makeOneChannel(kernel, kernel);

    namedWindow("orig", WINDOW_AUTOSIZE);
    Mat frame, proc, result;

    int64 lastTime = getTickCount();
    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        makeOneChannel(frame, proc);
        matchTemplate(proc, kernel, result, TM_SQDIFF_NORMED);
        double minval, maxval;
        Point minloc, maxloc;
        minMaxLoc(result, &minval, &maxval, &minloc, &maxloc);
        circle(frame, minloc + Point(13, 16), 4, Scalar(0, 255, 0), -1);

        std::cout << minval << ", ";

        imshow("orig", frame);
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

static void makeOneChannel(const Mat& src, Mat& dst)
{
    Mat tmp = src;
    std::vector<Mat> chans;
    split(tmp, chans);
    chans[2] = 255 - chans[2];
    dst = min(max(max(chans[0], chans[1]), chans[2]), 170);
}
