#include "GlobalMath.h"

using namespace cv;

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

Point3f fromSpherical(float r, float theta, float phi)
{
    return Point3f(
        r * cosf(theta) * sinf(phi),
        r * sinf(theta) * sinf(phi),
        r * cosf(phi)
        );
}

Point3d fromSpherical(double r, double theta, double phi)
{
    return Point3d(
        r * cos(theta) * sin(phi),
        r * sin(theta) * sin(phi),
        r * cos(phi)
        );
}

float angleDifferenceDegrees(float finalAngle, float initialAngle)
{
    float diff = fmodf(finalAngle - initialAngle, 360.0f);

    if (diff > 180.0f)
        return diff - 360.0f;
    if (diff < -180.0f)
        return diff + 360.0f;

    return diff;
}

GlobalRadius::GlobalRadius(int screenWidth, float fieldOfViewRadians)
    : radius(screenWidth / fieldOfViewRadians)
{ }

float GlobalRadius::toRadians(float pixels) const
{
    return pixels / radius;
}

double GlobalRadius::toRadians(double pixels) const
{
    return pixels / radius;
}

float GlobalRadius::toDegrees(float pixels) const
{
    return degrees(pixels / radius);
}

double GlobalRadius::toDegrees(double pixels) const
{
    return degrees(pixels / radius);
}

cv::Point3f GlobalRadius::toCartesian(float screenX, float screenY) const
{
    return fromSpherical(radius, toRadians(screenX), toRadians(screenY));
}

cv::Point3d GlobalRadius::toCartesian(double screenX, double screenY) const
{
    return fromSpherical((double)radius, toRadians(screenX), toRadians(screenY));
}

float GlobalRadius::toPixels(float radians) const
{
    return radians * radius;
}

double GlobalRadius::toPixels(double radians) const
{
    return radians * radius;
}