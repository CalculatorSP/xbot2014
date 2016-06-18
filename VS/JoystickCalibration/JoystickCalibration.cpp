#include <iostream>

#include "opencv2/opencv.hpp"

#include "PSX/XboxController.h"
#include "Util/ScreenGrabber.h"

#include "Processing.h"

#define WEBCAM  (0)
#define CAPCARD (1)

using namespace cv;

const char* comport = "COM4";

int main(int argc, const char **argv)
{
    Scheduler                       scheduler;
    XboxController                  xboxController(comport);
    JoystickCalibrationAppManager   appManager(&scheduler, &xboxController);
    ScreenGrabber                   grabber(CAPCARD, 30, &scheduler, &appManager, &appManager);

    if (!grabber.cap.isOpened())
    {
        std::cerr << "Could not open capture device" << std::endl;
        return -1;
    }

    if (!xboxController.isConnected())
    {
        std::cerr << "Could not open serial port " << comport << std::endl;
        //return -1;
    }

    cvNamedWindow("result", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("flow1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("flow2", CV_WINDOW_AUTOSIZE);

    appManager.run();

    return 0;
}
