#include <iostream>

#include <opencv2/opencv.hpp>
#include <PSX/XboxController.h>

#include "MotionModel.h"

using namespace cv;

static void mouseCallback(int evt, int x, int y, int flags, void* usrData);

static const char* comport = "COM3";
static const Point2f CROSSHAIR_LOCATION(639.0f, 441.0f);
static Point2f aimPoint(0.0f, 0.0f);
static bool haveAimPoint(false);

int main(int argc, const char **argv)
{
    XboxController xboxController(comport);
    if (!xboxController.isConnected())
    {
        std::cerr << "Could not open serial port " << comport << std::endl;
        return -1;
    }

    VideoCapture cap(0);
    if (!cap.isOpened())
    {
        std::cerr << "Could not open capture device" << std::endl;
        return -1;
    }

    Mat frame;
    int controlCount = 0;

    namedWindow("PointAndShoot", WINDOW_AUTOSIZE);
    setMouseCallback("PointAndShoot", mouseCallback, NULL);

    while (true)
    {
        cap >> frame;
        if (frame.empty())
            break;

        Point p = Point((int)CROSSHAIR_LOCATION.x, (int)CROSSHAIR_LOCATION.y);
        circle(frame, p, 5, Scalar(255, 255, 255));

        Point2f joystickVals;
        if (haveAimPoint)
        {
            switch (++controlCount)
            {
            case 1:
                joystickVals = MotionModel::getJoystickVals(aimPoint - CROSSHAIR_LOCATION);
                if (joystickVals.dot(joystickVals) < 1.0f)
                {
                    xboxController.set(XboxAnalog::RIGHT_STICK_X, joystickVals.x);
                    xboxController.set(XboxAnalog::RIGHT_STICK_Y, joystickVals.y);
                    xboxController.sendState(2);
                }
                else
                {
                    controlCount = 0;
                    haveAimPoint = false;
                }

                break;

            case 2:
                xboxController.set(XboxAnalog::RIGHT_STICK_X, 0.0f);
                xboxController.set(XboxAnalog::RIGHT_STICK_Y, 0.0f);
                xboxController.press(XboxButton::RIGHT_TRIGGER);
                xboxController.sendState(3);
                break;

            case 3:
                xboxController.reset();
                xboxController.sendState(3);
                controlCount = 0;
                haveAimPoint = false;
                break;

            default:
                break;
            }
        }

        imshow("PointAndShoot", frame);
        switch (waitKey(1))
        {
        case 27:
            return 0;
        default:
            break;
        }
    }

    cap.release();
    destroyAllWindows();
    return 0;
}

static void mouseCallback(int evt, int x, int y, int flags, void* usrData)
{
    switch (evt)
    {
    case EVENT_LBUTTONUP:
        if (!haveAimPoint)
        {
            aimPoint = Point2f((float)x, (float)y);
            haveAimPoint = true;
        }
        break;

    default:
        break;
    }
}
