#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>

#define TMPLFILE    "C:/Users/John/Desktop/hdSamples/enemy_arrow.png"

using namespace cv;

static void makeOneChannel(const Mat& src, Mat& dst);

int main(int argc, const char **argv)
{
    if (argc != 2)
    {
        printf("Please provide a filename or drag and drop");
        return -1;
    }

    VideoCapture cap(argv[1]);
    Mat frame, result;
    Mat kernel = imread(TMPLFILE);

    makeOneChannel(kernel, kernel);

    namedWindow("orig", CV_WINDOW_AUTOSIZE);
    namedWindow("result", CV_WINDOW_AUTOSIZE);
    namedWindow("kernel", CV_WINDOW_AUTOSIZE);

    imshow("kernel", kernel);

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        makeOneChannel(frame, frame);
        matchTemplate(frame, kernel, result, 1);
        double minval, maxval;
        minMaxIdx(result, &minval, &maxval);

        std::cout << minval << ", " << maxval << std::endl;

        imshow("orig", frame);
        imshow("result", result);
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
    }

    return 0;
}

static void makeOneChannel(const Mat& src, Mat& dst)
{
    std::vector<Mat> chans;
    split(src, chans);
    chans[2] = 255 - chans[2];
    merge(chans, dst);
    //cvtColor(dst, dst, CV_BGR2GRAY);
}

static void findEnemy(const Mat& src, Mat& dst)
{
    namedWindow("original", CV_WINDOW_AUTOSIZE);
    namedWindow("result", CV_WINDOW_AUTOSIZE);

    Mat tmp = src.clone();
    tmp.convertTo(tmp, CV_32F);
    cvtColor(tmp, tmp, CV_BGR2GRAY);
    tmp /= 255.0f;

    Mat kernel = imread(TMPLFILE);
    kernel.convertTo(kernel, CV_32F);
    cvtColor(kernel, kernel, CV_BGR2GRAY);
    kernel /= 255.0f;

    matchTemplate(tmp, kernel, dst, 1);

    double minVal, maxVal;
    Point minLoc, maxLoc;
    minMaxLoc(dst, &minVal, &maxVal, &minLoc, &maxLoc);
    minLoc += Point(kernel.cols / 2, kernel.rows);
    maxLoc += Point(kernel.cols / 2, kernel.rows);
    dst /= maxVal;
    std::cout << minLoc << ", " << minVal << std::endl << maxLoc << ", " << maxVal << std::endl;

    cvtColor(tmp, tmp, CV_GRAY2BGR);
    circle(tmp, minLoc, 8, Scalar(0.0f, 1.0f, 0.0f));
    circle(tmp, maxLoc, 8, Scalar(0.0f, 0.0f, 1.0f));

    imshow("original", tmp);
    imshow("result", dst);
}
