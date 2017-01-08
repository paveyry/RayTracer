#pragma once
#include <opencv2/core/core.hpp>

class Camera
{
public:
    // CONSTRUCTOR
    Camera(cv::Vec3i pos, cv::Vec3i lookAt, cv::Vec3i up, double fov, int width, int height);

    // This function does not exist in OPENCV for Vectors.(fixme: change function location ...)
    cv::Vec3i cross(cv::Vec3i a, cv::Vec3i b);

public:
    cv::Vec3i pos_;
    cv::Vec3i lookAt_;
    cv::Vec3i up_;
    double fov_;
    int width_;
    int height_;

    // Attributes computed using the previous ones.
    cv::Vec3i viewDirection_;
    cv::Vec3i U_;
    cv::Vec3i V_;
    double aspectRatio_;
    double angle_;
};