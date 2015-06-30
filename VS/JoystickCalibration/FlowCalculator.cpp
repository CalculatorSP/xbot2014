#include "Math/GlobalMath.h"
#include "FlowCalculator.h"
#include <stdint.h>

FlowCalculator::FlowCalculator(int screenWidth, float fieldOfViewDegrees)
    : _globalRadius(getGlobalRadius(screenWidth, radians(fieldOfViewDegrees))),
    
    _maxCorners(100),
    _qualityLevel(0.3),
    _minDistance(7),
    _blockSize(7),

    _winSize(15, 7),
    _maxLevel(2),
    _termCrit(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 10, 0.03)
{ }

void FlowCalculator::depositFrame(const Mat& frame)
{
    _frames.push_back(frame.clone());
}

void FlowCalculator::calculate()
{
    vector<Point2f> p0;
    vector<Point2f> p1;
    vector<uint8_t> status;
    vector<float> err;

    vector<Point3f> r0;
    vector<Point3f> r1;

    Mat rot, rod;

    // Iterate through frames
    for (int i = 0; i < _frames.size() - 1; ++i)
    {
        // Find distinctive points in the current frame
        goodFeaturesToTrack(_frames[i], p0, _maxCorners, _qualityLevel, _minDistance, Mat(), _blockSize, false, 0.04);
        cornerSubPix(_frames[i], p0, _winSize, Size(-1, -1), _termCrit);

        // Track them into the next frame
        calcOpticalFlowPyrLK(_frames[i], _frames[i + 1], p0, p1, status, err, _winSize, _maxLevel, _termCrit);

        // Compute 3D coordinates of the points using constant radius in spherical coordinates
        for (int j = 0; j < p0.size(); ++j)
        {
            // Skip points that were not found in the second frame
            if (!status[j])
                continue;

            // Convert from spherical to Cartesian coordinates
            //  (double y since screen was cut in half to handle interlacing)
            r0.push_back(fromSpherical(_globalRadius, p0[j].x, 2.0f * p0[j].y));
            r1.push_back(fromSpherical(_globalRadius, p1[j].x, 2.0f * p1[j].y));

            // Compute optimal rotation between point clouds in Rodrigues form
            _computeRotationMatrix(r0, r1, rot);
            Rodrigues(rot, rod);
            rodriguesVectors.push_back(rod.clone());
        }
    }
}

void FlowCalculator::_computeRotationMatrix(const vector<Point3f>& r0, const vector<Point3f>& r1, Mat& r)
{
    // Compute H
    Mat H = Mat::zeros(Size(3, 3), CV_32F);
    Mat rm, rt;
    for (int i = 0; i < r0.size(); ++i)
    {
        rm = Mat(r0);
        transpose(rm, rt);
        H += rm * rt;
    }

    // Compute Singular Value Decomposition of H
    _svd(H, SVD::MODIFY_A);

    // R = V * UT = (U * VT)T
    transpose(_svd.u * _svd.vt, r);
    
    // Handle relfection case
    if (determinant(r) < 0)
        r.col(2) *= -1.0f;
}

