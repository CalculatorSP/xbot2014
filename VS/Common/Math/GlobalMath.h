#pragma once

#include <math.h>
#include "opencv2/opencv.hpp"

#define PI      (3.14159265358979323846)
#define PI_F    (3.14159265359f)

// Converts an angle from degrees to radians
float radians(float degrees);

// Converts an angle from degrees to radians
double radians(double degrees);

// Converts an angle from radians to degrees
float degrees(float radians);

// Converts an angle from radians to degrees
double degrees(double radians);

// Converts a point from spherical coordinates (radius, azimuth, polar)
//  to Cartesian coordinates (x, y, z)
cv::Point3f fromSpherical(float r, float theta, float phi);

// Converts a point from spherical coordinates (radius, azimuth, polar) in radians
//  to Cartesian coordinates (x, y, z)
cv::Point3d fromSpherical(double r, double theta, double phi);


class GlobalRadius
{
public:
    // Radius in pixels
    const float radius;

    GlobalRadius(int screenWidth, float fieldOfViewRadians);

    float toRadians(float pixels) const;
    double toRadians(double pixels) const;

    float toDegrees(float pixels) const;
    double toDegrees(double pixels) const;

    cv::Point3f toCartesian(float screenX, float screenY) const;
    cv::Point3d toCartesian(double screenX, double screenY) const;
};