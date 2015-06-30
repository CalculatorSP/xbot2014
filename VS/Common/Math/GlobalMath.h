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

// Computes the radius of the view sphere in pixels based on screen resolution and
//  field of view angle (in radians)
float getGlobalRadius(int screenWidth, float fieldOfViewRadians);

// Converts a point from spherical coordinates (radius, azimuth, polar)
//  to Cartesian coordinates (x, y, z)
cv::Point3f fromSpherical(float r, float theta, float phi);