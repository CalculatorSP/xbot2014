#include "FlowCalculator.h"
#include <stdint.h>

FlowCalculator::FlowCalculator(int screenWidth, float fieldOfViewDegrees)
    : _globalRadius(screenWidth, radians(fieldOfViewDegrees)),
    
    _maxCorners(50),
    _qualityLevel(0.3),
    _minDistance(15),
    _blockSize(15),

    _winSize(127, 63),
    _maxLevel(5),
    _termCrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 40, 0.001)
{ }

void FlowCalculator::depositFrame(const Mat& frame)
{
    Mat gray;
    cvtColor(frame, gray, CV_BGR2GRAY);
    _frames.push_back(gray.clone());
}

void FlowCalculator::calculate()
{
    vector<Point2f> p0;
    vector<Point2f> p1;
    vector<uint8_t> status;
    vector<float> err;

    vector<Point3f> r0;
    vector<Point3f> r1;

    Mat rot, copy1, copy2;

    // Clear old rotations
    gammaBetaAlphaRotations.clear();

    // Iterate through frames
    for (int i = 0; i < _frames.size() - 1; ++i)
    {
        // Clear old points
        r0.clear();
        r1.clear();

        // Find distinctive points in the current frame
        goodFeaturesToTrack(_frames[i], p0, _maxCorners, _qualityLevel, _minDistance, Mat(), _blockSize, false, 0.04);
        //cornerSubPix(_frames[i], p0, _winSize, Size(-1, -1), _termCrit);

        // Track them into the next frame
        calcOpticalFlowPyrLK(_frames[i], _frames[i + 1], p0, p1, status, err, _winSize, _maxLevel, _termCrit, 0, 0.01);


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

        // Compute 3D coordinates of the points using constant radius in spherical coordinates
        for (int j = 0; j < p0.size(); ++j)
        {
            // Skip points that were not found in the second frame
            if (!status[j])
                continue;

            // Convert from spherical to Cartesian coordinates
            //  (double y since screen was cut in half to handle interlacing)
            r0.push_back(_globalRadius.toCartesian(p0[j].x, 2.0f * p0[j].y));
            r1.push_back(_globalRadius.toCartesian(p1[j].x, 2.0f * p1[j].y));
        }

        // Compute optimal rotation between point clouds
        _computeRotationMatrix(r0, r1, rot);

        float beta = asinf(rot.at<float>(0, 2));

        float tmp = rot.at<float>(0, 0) / cosf(beta);
        if (tmp > 1.0f)
            tmp = 1.0f;
        if (tmp < -1.0f)
            tmp = 1.0f;
        float gamma = acosf(tmp);

        tmp = rot.at<float>(2, 2) / cosf(beta);
        if (tmp > 1.0f)
            tmp = 1.0f;
        if (tmp < -1.0f)
            tmp = 1.0f;
        float alpha = acosf(tmp);

        printf("%f, %f, %f\n", _globalRadius.toPixels(gamma), _globalRadius.toPixels(beta), _globalRadius.toPixels(alpha));

        // Store average rotation angles
        gammaBetaAlphaRotations.push_back(Point3f(gamma, beta, alpha));

        waitKey();
    }

    // Clear old frames
    _frames.clear();
}

void FlowCalculator::_computeRotationMatrix(const vector<Point3f>& r0, const vector<Point3f>& r1, Mat& r)
{
    // Compute H
    Mat H = Mat::zeros(Size(3, 3), CV_32F);
    Mat r0Mat, r1Mat;
    for (int i = 0; i < r0.size(); ++i)
    {
        r0Mat = Mat(r0[i]);
        r1Mat = Mat(r1[i]);
        transpose(r1Mat, r1Mat);
        H += r0Mat * r1Mat;
    }

    // Compute Singular Value Decomposition of H
    _svd(H, SVD::MODIFY_A);

    // R = V * UT = (U * VT)T
    transpose(_svd.u * _svd.vt, r);
    
    // Handle relfection case
    if (determinant(r) < 0)
        r.col(2) *= -1.0f;

    // Clip at +-1.0f
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (r.at<float>(i, j) > 1.0f)
                r.at<float>(i, j) = 1.0f;
            if (r.at<float>(i, j) < -1.0f)
                r.at<float>(i, j) = -1.0f;
        }
    }
}

