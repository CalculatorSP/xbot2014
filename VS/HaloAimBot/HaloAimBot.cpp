#include <iostream>

#include "opencv2/opencv.hpp"
#include "opencv2/videoio.hpp"

#include "PSX/XboxController.h"
#include "Util/ScreenGrabber.h"

#include "Processing.h"

#define WEBCAM  (0)
#define CAPCARD (0)

using namespace cv;

const char* comport = "COM3";

int main(int argc, const char **argv)
{
    Scheduler                       scheduler;
    XboxController                  xboxController(comport);
    HaloAimBotAppManager            appManager(&scheduler, &xboxController);
    ScreenGrabber                   grabber(CAPCARD, 60, &scheduler, &appManager, &appManager);

    if (!grabber.cap.isOpened())
    {
        std::cerr << "Could not open capture device" << std::endl;
        //return -1;
    }

    if (!xboxController.isConnected())
    {
        std::cerr << "Could not open serial port " << comport << std::endl;
        //return -1;
    }

    appManager.run();
    return 0;
}
