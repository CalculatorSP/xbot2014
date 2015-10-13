#include <sstream>
#include "Processing.h"

#define KEY_ESC	                (27)
#define MAX_RECORDING_FRAMES    (900)

HaloAimBotAppManager::HaloAimBotAppManager(Scheduler* scheduler, XboxController* controller)
    :
    _scheduler(scheduler),
    _pursuitController(scheduler, controller),
    _state(HUNTING),
    _autoAim(false),
    _aaCounter(0),
    _eDetect(false),
    _screenshot(false),
    _ssCounter(0),
    _recording(false),
    _frames(),
    _crosshairLocation(318, 294)
{ }

HaloAimBotAppManager::~HaloAimBotAppManager()
{
    _quit();
}

static float getVal(int t)
{
    int s = t % 24;
    if (s >= 12)
        s = 24 - s;

    return 300.0f * (s / 12.0f - 0.5f);
}

void HaloAimBotAppManager::processFrame(Mat& frame)
{
    ++_aaCounter;
    Point2f target(getVal(_aaCounter), getVal(_aaCounter));
    Point2f target1(getVal(_aaCounter + 1), getVal(_aaCounter + 1));
    Point2f curTarget(getVal(_aaCounter + 2), getVal(_aaCounter + 2));

    Point2f c(25.0f, 25.0f);

    //_updateStateMachine(frame);

    resize(frame, frame, Size(), 1.0, 0.5, INTER_NEAREST);
    resize(frame, frame, Size(), 1.0, 2.0, INTER_NEAREST);

    //circle(frame, _crosshairLocation, 8, Scalar(0, 255, 255));

    circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + target, 4, Scalar(0, 255, 0));
    circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + target1, 4, Scalar(0, 255, 0));
    circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + curTarget, 4, Scalar(0, 255, 0));

    if (_autoAim)
    {
        _pursuitController._control.at<float>(0) = c.x;
        _pursuitController._control.at<float>(1) = c.y;
        _pursuitController.updateWithTarget(target);
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curPrediction.at<float>(0), _pursuitController._curPrediction.at<float>(1)), 4, Scalar(0, 0, 255));
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curPrediction.at<float>(2), _pursuitController._curPrediction.at<float>(3)), 4, Scalar(0, 0, 255));
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curPrediction.at<float>(4), _pursuitController._curPrediction.at<float>(5)), 4, Scalar(0, 0, 255));
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curEstimate.at<float>(0), _pursuitController._curPrediction.at<float>(1)), 4, Scalar(255, 0, 0));
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curEstimate.at<float>(2), _pursuitController._curPrediction.at<float>(3)), 4, Scalar(255, 0, 0));
        circle(frame, Point2f(_crosshairLocation.x, _crosshairLocation.y) + Point2f(_pursuitController._curEstimate.at<float>(4), _pursuitController._curPrediction.at<float>(5)), 4, Scalar(255, 0, 0));
    }

    imshow("result", frame);

    if (_recording)
        _frames.push_back(frame.clone());

    if (_frames.size() > MAX_RECORDING_FRAMES)
        _saveRecording();

    if (_screenshot)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), frame);
        _screenshot = false;
    }
}

void HaloAimBotAppManager::handleKey(int key)
{
    switch (key)
    {
    case KEY_ESC:
        _quit();
        break;

    case 'a':
        _autoAim = !_autoAim;
        if (_autoAim)
        {
            _pursuitController.startPursuing(Point2f(100, 100));
            _aaCounter = 0;
        }
        else
            _pursuitController.reset();
        break;

    case 'e':
        _eDetect = !_eDetect;
        break;

    case 's':
        _screenshot = true;
        break;

    case 'r':
        _recording = !_recording;
        if (!_recording)
            _saveRecording();
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

void HaloAimBotAppManager::_saveRecording()
{
    for (int i = 0; i < _frames.size(); ++i)
    {
        std::stringstream filename;
        filename << "C:/Users/John/Desktop/cap/" << _ssCounter++ << ".png";
        imwrite(filename.str(), _frames[i]);
    }
    
    _frames.clear();
}

void HaloAimBotAppManager::_updateStateMachine(Mat& frame)
{
    switch (_state)
    {
    case HUNTING:
    {
        Point target;
        if (_hunter.findTarget(frame, target, _eDetect))
        {
            circle(frame, target, 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint((float)(target.x - _crosshairLocation.x), (float)(target.y - _crosshairLocation.y));

            if (!_autoAim)
                break;

            if (_pursuitController.startPursuing(aimPoint))
            {
                _state = PURSUIT;
                printf("TARGET ACQUIRED\n");
            }
        }
        break;
    }

    case PURSUIT:
    {
        Point newTarget;
        if (_hunter.findTarget(frame, newTarget, _eDetect))
        {
            circle(frame, newTarget, 3, Scalar(0, 255, 0), -1);
            Point2f aimPoint((float)(newTarget.x - _crosshairLocation.x), (float)(newTarget.y - _crosshairLocation.y));

            if (!_pursuitController.updateWithTarget(aimPoint) || !_autoAim)
            {
                _pursuitController.reset();
                _state = HUNTING;
                printf("TARGET LOST\n");
            }
        }
        else if (!_pursuitController.updateWithoutTarget() || !_autoAim)
        {
            _pursuitController.reset();
            _state = HUNTING;
            printf("TARGET LOST\n");
        }
        break;
    }

    default:
        break;
    }
}
