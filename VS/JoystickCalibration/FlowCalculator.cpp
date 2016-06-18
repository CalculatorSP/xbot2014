#include <stdint.h>

#include "FlowCalculator.h"

#define KEY_ENTER   (13)
#define KEY_ESC     (27)

using namespace cv;

FlowCalculator::FlowCalculator(int screenWidth, float fieldOfViewDegrees)
    : _globalRadius(screenWidth, radians(fieldOfViewDegrees)),
    
    _maxCorners(50),
    _qualityLevel(0.2),
    _minDistance(15),
    _blockSize(15),

    _winSize(31, 15),
    _maxLevel(6),
    _termCrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 80, 0.0001)
{ }

void FlowCalculator::depositFrame(const Mat& frame)
{
    Mat gray;
    cvtColor(frame, gray, CV_BGR2GRAY);
    _frames.push_back(gray.clone());
}

void FlowCalculator::calculate()
{
    std::vector<Point2f> p0, p1;
    std::vector<float> rx, ry;
    std::vector<uint8_t> status;
    std::vector<float> err;

    Mat copy1, copy2;

    // Clear old rotations
    gammaAlphaRotations.clear();

    // Iterate through frames
    for (int i = 0; i < _frames.size() - 1; ++i)
    {
        // Clear old points
        rx.clear();
        ry.clear();

        // Find distinctive points in the current frame
        goodFeaturesToTrack(_frames[i], p0, _maxCorners, _qualityLevel, _minDistance, Mat(), _blockSize, false, 0.04);
        if (p0.size() == 0)
            continue;

        // Track them into the next frame
        calcOpticalFlowPyrLK(_frames[i], _frames[i + 1], p0, p1, status, err, _winSize, _maxLevel, _termCrit, 0, 0.00001);

        _frames[i].copyTo(copy1);
        _frames[i + 1].copyTo(copy2);

        // Display 2 frames with keypoints
        for (int j = 0; j < p0.size(); ++j)
        {
            if (!status[j])
                continue;

            circle(copy1, p0[j], 4, Scalar(255, 255, 255), -1, 8, 0);
            circle(copy2, p1[j], 4, Scalar(255, 255, 255), -1, 8, 0);
        }
        imshow("flow1", copy1);
        imshow("flow2", copy2);

        // Remove points that were not found
        for (int j = 0; j < p0.size(); ++j)
        {
            if (!status[j])
                continue;

            rx.push_back(p1[j].x - p0[j].x);
            ry.push_back(p1[j].y - p0[j].y);
        }

        float gamma = _getMedian(rx);
        float alpha = _getMedian(ry);

        printf("%f, %f\n", gamma, alpha);

        switch (waitKey())
        {
        // Store average rotation angles if supervisor approves...
        case KEY_ENTER:
            // Convert pixels to radians
            gammaAlphaRotations.push_back(Point2f(_globalRadius.toRadians(gamma), _globalRadius.toRadians(alpha)));
            break;

        // Give up :(
        case KEY_ESC:
            _frames.clear();
            return;

        default:
            break;
        }

    }

    // Clear old frames
    _frames.clear();
}

template <typename T>
T FlowCalculator::_getMedian(std::vector<T>& vals)
{
    size_t size = vals.size();
    if (size == 0)
        return (T)0;

    sort(vals.begin(), vals.end());

    if (size % 2 == 0)
        return (vals[size / 2 - 1] + vals[size / 2]) / 2;
    else
        return vals[size / 2];
}
