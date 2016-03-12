#include "opencv2/highgui/highgui.hpp"

#include "MotionModel.h"
#include "TargetTracker.h"

using namespace cv;

static void mouseCallback(int evt, int x, int y, int flags, void* usrData);
static void drawFrame(Mat& img, bool startedTracking, bool pullTrigger, Point2f target);

static bool haveTarget = false;
static Point2f mouseSelection(0.0f, 0.0f);
static const Point2f CROSSHAIR_LOCATION(318.0f, 294.0f);

int main(int argc, const char **argv)
{
    TargetTracker targetTracker;
    TargetTrackerOutput control;
    Mat img(Size(640, 480), CV_8UC3);
    Point2f target(0.0f, 0.0f);

    namedWindow("result", CV_WINDOW_AUTOSIZE);
    setMouseCallback("result", mouseCallback, NULL);

    bool startedTracking = false;
    bool keepGoing = true;
    while (keepGoing)
    {
        if (control.giveUp)
        {
            printf("gave up...\n");
            control = TargetTrackerOutput();
            haveTarget = false;
            startedTracking = false;
            targetTracker.reset();
        }

        drawFrame(img, startedTracking, control.pullTrigger, target);
        imshow("result", img);
        
        switch (waitKey(10))
        {
        case 's':
            if (control.pullTrigger)
            {
                control.giveUp = true;
            }
            else if (startedTracking)
            {
                targetTracker.trackWithTarget(target - CROSSHAIR_LOCATION, control);
                target -= MotionModel::getRotationRate(control.joystickVals);
            }
            else if (haveTarget)
            {
                target = mouseSelection;
                startedTracking = true;
            }
            break;

        case 'b':
            if (startedTracking)
            {
                targetTracker.trackWithoutTarget(control);
                target -= MotionModel::getRotationRate(control.joystickVals);
            }
            break;

        case 'r':
            control.giveUp = true;
            break;

        case 27:
            keepGoing = false;
            break;

        default:
            break;
        }
    }

    return 0;
}

static void mouseCallback(int evt, int x, int y, int flags, void* usrData)
{
    switch (evt)
    {
    case EVENT_LBUTTONUP:
        haveTarget = true;
        mouseSelection = Point2f((float)x, (float)y);
        break;

    case EVENT_RBUTTONUP:
        haveTarget = false;
        break;

    default:
        break;
    }
}

static void drawFrame(Mat& img, bool startedTracking, bool pullTrigger, Point2f target)
{
    img.setTo(Scalar(0));

    if (startedTracking)
        circle(img, Point((int)target.x, (int)target.y), 5, Scalar(0, 0, 255));
    else if (haveTarget)
        circle(img, Point((int)mouseSelection.x, (int)mouseSelection.y), 5, Scalar(255, 0, 0));

    if (pullTrigger)
        circle(img, Point((int)CROSSHAIR_LOCATION.x, (int)CROSSHAIR_LOCATION.y), 3, Scalar(0, 255, 0));
    else
        circle(img, Point((int)CROSSHAIR_LOCATION.x, (int)CROSSHAIR_LOCATION.y), 3, Scalar(255, 255, 255));
}
