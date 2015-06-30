#include "GlobalMath.h"

float radians(float degrees)
{
    return degrees * PI_F / 180.0f;
}

double radians(double degrees)
{
    return degrees * PI / 180.0;
}

float degrees(float radians)
{
    return radians * 180.0f / PI_F;
}

double degrees(double radians)
{
    return radians * 180.0 / PI;
}

float getGlobalRadius(int screenWidth, float fieldOfViewRadians)
{
    return screenWidth / fieldOfViewRadians;
}

cv::Point3f fromSpherical(float r, float theta, float phi)
{
    return cv::Point3f(
        r * cosf(theta) * sinf(phi),
        r * sinf(theta) * sinf(phi),
        r * cosf(phi)
        );
}