#include <sstream>
#include "Processing.h"

#define KEY_ESC	            (27)
#define FRAME_DELAY         (2)
#define RED_OPPOSITE        (180)
#define SCORE_THRESH        (160)
#define MAX_SEARCH_PIXELS   (3000)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _pursuitController(scheduler, controller, &_motionTracker),
    _state(HUNTING),
    _eDetect(false),
    _screenshot(false),
    _ssCounter(0),
    _template(imread("C:/Users/John/Desktop/cap/template.png", CV_LOAD_IMAGE_GRAYSCALE)),
    _searchRegion(50, 25, 220, 190),
    _templateCenter(23, 9)
{
    resize(_template, _template, Size(), 0.5, 0.5, INTER_NEAREST);
}

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    uint64_t startTime = Scheduler::getTime();
    _preprocess(frame);
    printf("%llu\n", Scheduler::getTime() - startTime);

    resize(frame, frame, Size(), 2.0, 2.0, INTER_NEAREST);
    imshow("result", frame);

    if (_screenshot)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), frame);
        _screenshot = false;
    }

    _motionTracker.update();
    _updateStateMachine(frame);
}

void HaloAimBotAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _quit();
        break;

    case 'e':
        _eDetect = !_eDetect;
        break;

    case 'r':
        _screenshot = true;
        break;

    default:
        break;
    }
}

void HaloAimBotAppManager::run()
{
    while (_state != QUIT)
        _scheduler->run();
}

void HaloAimBotAppManager::_quit()
{
    _pursuitController.reset();
    _state = QUIT;
    _scheduler->clear();
}

void HaloAimBotAppManager::_preprocess(Mat& frame)
{
    resize(frame, frame, Size(), 0.5, 0.5, INTER_NEAREST);

    // Split into hsv components
    Mat proc = frame.clone();
    Mat hsv[3];
    cvtColor(proc, proc, CV_BGR2HSV);
    split(proc, hsv);

    // Filter hue channel
    absdiff(hsv[0], RED_OPPOSITE / 2, hsv[0]);
    blur(hsv[0], hsv[0], Size(2, 2));
    threshold(hsv[0], hsv[0], 70.0, 255.0, CV_THRESH_BINARY);

    // Filter saturation channel
    blur(hsv[1], hsv[1], Size(2, 2));
    threshold(hsv[1], hsv[1], 70.0, 255.0, CV_THRESH_BINARY);

    // Filter value channel
    blur(hsv[2], hsv[2], Size(2, 2));
    threshold(hsv[2], hsv[2], 70.0, 255.0, CV_THRESH_BINARY);

    // Mix filtered hsv channels
    bitwise_and(hsv[0], hsv[1], proc);
    bitwise_and(hsv[2], proc, proc);

    if (_eDetect)
        cvtColor(proc, frame, CV_GRAY2BGR);

    rectangle(frame, _searchRegion, Scalar(255, 0, 0));

    // Check if too many pixels are red
    if (countNonZero(proc(_searchRegion)) > MAX_SEARCH_PIXELS)
        return;

    // Find template in filtered image
    int bestScore = SCORE_THRESH;
    Point loc;
    for (int x = _searchRegion.tl().x; x < _searchRegion.br().x; ++x)
    {
        for (int y = _searchRegion.tl().y; y < _searchRegion.br().y; ++y)
        {
            Point p(x, y);

            if (!proc.at<uint8_t>(p))
                continue;

            Mat diff;
            bitwise_xor(proc(Rect(p - _templateCenter, Size(_template.cols, _template.rows))), _template, diff);
            int score = countNonZero(diff);
            if (score < bestScore)
            {
                bestScore = score;
                loc = p + Point(0, 5);
            }
        }
    }

    if (bestScore < SCORE_THRESH)
        circle(frame, loc, 2, Scalar(0, 255, 0), -1);
}

void HaloAimBotAppManager::_updateStateMachine(const Mat& frame)
{
    switch (_state)
    {
    case HUNTING:
    {
        Point2f target(0.0f, 0.0f);
        if (_hunter.findTarget(frame, target))
        {
            if (_pursuitController.startPursuing(target - _motionTracker.motionHistory[-FRAME_DELAY]))
                _state = PURSUIT;
        }

        break;
    }

    case PURSUIT:
    {
        Point2f newTarget(0.0f, 0.0f);
        if (_hunter.findTarget(frame, newTarget))
        {
            if (!_pursuitController.updateTarget(newTarget - _motionTracker.motionHistory[-FRAME_DELAY]))
            {
                _pursuitController.reset();
                _state = HUNTING;
            }
        }

        break;
    }

    default:
        break;
    }
}
