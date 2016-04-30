//#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
//
//#include <iostream>
//
//#define CAPFILE     "C:/Users/John/Desktop/videoSamples/rag.mp4"
//#define TMPLFILE    "C:/Users/John/Source/Repos/xbot2014/VS/HaloAimBot/template.png"
//
//using namespace cv;
//using namespace std;
//
//int main(int argc, const char **argv)
//{
//    VideoCapture cap(CAPFILE);
//    Mat frame;
//    Mat proc;
//    
//    Mat kernel = imread(TMPLFILE);
//
//    namedWindow("result", CV_WINDOW_AUTOSIZE);
//    namedWindow("original", CV_WINDOW_AUTOSIZE);
//
//    bool keepGoing = true;
//    int i = 0;
//    while (keepGoing)
//    {
//        cap >> frame;
//
//        imshow("original", frame);
//        if (i++ > 700)
//        {
//            frame.convertTo(frame, CV_32F);
//            filter2D(frame, proc, -1, kernel);
//            imshow("result", proc);
//        }
//        
//        int k = waitKey(1);
//        switch (k)
//        {
//        case 27:
//            keepGoing = false;
//            break;
//
//        default:
//            break;
//        }
//    }
//
//    return 0;
//}
