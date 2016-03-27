#include "opencv2/highgui/highgui.hpp"

#include "Collections/ModBuffer.h"

#include "MotionModel.h"
#include "TargetTracker.h"

#define FRAME_DELAY (2)

using namespace cv;

static void mouseCallback(int evt, int x, int y, int flags, void* usrData);
static Point2f getInitialVelocity();
static void drawFrame(Mat& img, bool startedTracking, Point2f targetPosition, Point2f targetVelocity);

static bool haveTarget = false;
static Point2f mouseSelection(0.0f, 0.0f);
static Point2f velocitySelection(0.0f, 0.0f);
static const Point2f CROSSHAIR_LOCATION(318.0f, 294.0f);

int main(int argc, const char **argv)
{
    TargetTracker targetTracker;
    ModBuffer<TargetTrackerOutput, FRAME_DELAY> controlHist;
    for (int i = 0; i < FRAME_DELAY; ++i)
        controlHist.deposit(TargetTrackerOutput());

    Mat img(Size(640, 480), CV_8UC3);
    Point2f targetPosition(0.0f, 0.0f);
    Point2f targetVelocity(0.0f, 0.0f);

    namedWindow("result", CV_WINDOW_AUTOSIZE);
    setMouseCallback("result", mouseCallback, NULL);

    bool shooting = false;
    bool reset = false;
    bool startedTracking = false;
    bool keepGoing = true;
    while (keepGoing)
    {
        if (reset)
        {
            printf("resetting\n");
            shooting = false;
            reset = false;
            haveTarget = false;
            startedTracking = false;
            targetTracker.reset();
            for (int i = 0; i < FRAME_DELAY; ++i)
                controlHist.deposit(TargetTrackerOutput());
        }

        drawFrame(img, startedTracking, targetPosition, targetVelocity);
        imshow("result", img);
        
        switch (waitKey(10))
        {
        case 's':
            printf("\n------------------------------------------\n");
            if (shooting)
            {
                printf("Here we go!\n");
                Point2f rotationRate = MotionModel::getRotationRate(controlHist[0].joystickVals);
                std::cout << "Rotation rate: " << rotationRate << std::endl;
                targetPosition -= rotationRate;
                targetVelocity += targetVelocity;
                if (controlHist[0].pullTrigger)
                {
                    printf("FIRE!!!! (Press key...)\n");
                    waitKey();
                    shooting = false;
                    reset = true;
                }
                controlHist.deposit(TargetTrackerOutput());
            }
            else if (startedTracking)
            {
                TargetTrackerOutput control;
                targetTracker.trackWithTarget(targetPosition - CROSSHAIR_LOCATION, control);
                if (control.pullTrigger)
                {
                    shooting = true;
                    printf("gonna shoot...\n");
                }
                if (control.giveUp)
                {
                    reset = true;
                    printf("gave up.\n");
                }
                printf("Press key to continue...\n");
                waitKey();
                Point2f rotationRate = MotionModel::getRotationRate(controlHist[0].joystickVals);
                std::cout << "Rotation rate: " << rotationRate << std::endl;
                targetPosition -= rotationRate;
                targetPosition += targetVelocity;
                controlHist.deposit(control);
            }
            else if (haveTarget)
            {
                targetPosition = mouseSelection;
                targetVelocity = velocitySelection;
                startedTracking = true;
                controlHist.reset();
                for (int i = 0; i < FRAME_DELAY; ++i)
                    controlHist.deposit(TargetTrackerOutput());
            }
            break;

        case 'r':
            reset = true;
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
        velocitySelection = getInitialVelocity();
        std::cout << "YOU CHOSE: " << velocitySelection << std::endl;
        break;

    case EVENT_RBUTTONUP:
        haveTarget = false;
        break;

    default:
        break;
    }
}

static Point2f getInitialVelocity()
{
    float x = 0.0f;
    float y = 0.0f;
    printf("X Velocity: ");
    scanf("%f", &x);
    printf("Y Velocity: ");
    scanf("%f", &y);

    return Point2f(x, y);
}

static void drawFrame(Mat& img, bool startedTracking, Point2f targetPosition, Point2f targetVelocity)
{
    img.setTo(Scalar(0));

    if (startedTracking)
    {
        Point p1 = Point((int)targetPosition.x, (int)targetPosition.y);
        Point p2 = p1 + Point((int)targetVelocity.x, (int)targetVelocity.y);
        circle(img, p1, 5, Scalar(0, 0, 255));
        arrowedLine(img, p1, p2, Scalar(0, 0, 255));
    }
    else if (haveTarget)
    {
        Point p1 = Point((int)mouseSelection.x, (int)mouseSelection.y);
        Point p2 = p1 + Point((int)velocitySelection.x, (int)velocitySelection.y);
        circle(img, p1, 5, Scalar(255, 0, 0));
        arrowedLine(img, p1, p2, Scalar(255, 0, 0));
    }

    circle(img, Point((int)CROSSHAIR_LOCATION.x, (int)CROSSHAIR_LOCATION.y), 3, Scalar(255, 255, 255));
}
