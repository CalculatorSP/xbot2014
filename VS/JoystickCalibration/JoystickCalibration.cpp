#include <iostream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "TestDataGenerator.h"

#define CAPCARD (0)

using namespace cv;

const char* comport = "COM3";

int main(int argc, const char **argv)
{
    VideoCapture cap(CAPCARD);
    XboxController xboxController(comport);

    if (!cap.isOpened())
    {
        std::cerr << "Could not open capture device" << std::endl;
        //return -1;
    }

    if (!xboxController.isConnected())
    {
        std::cerr << "Could not open serial port " << comport << std::endl;
        //return -1;
    }

    TestDataGenerator testDataGenerator(&cap, &xboxController);
    testDataGenerator.run();

    return 0;
}
