#pragma once
#include <opencv2/core/core.hpp>

class Camera
{
public:
    // CONSTRUCTOR
    Camera(cv::Vec3i pos, cv::Vec3i lookAt, cv::Vec3i up, double fov, int width, int height);

public:
    double aspectRatio_;
    double angle_;
    cv::Mat transformView_;
    int width_;
    int height_;

private:
    cv::Vec3i pos_;
    cv::Vec3i lookAt_;
    cv::Vec3i up_;
    double fov_;

    // Attributes computed using the previous ones.
    cv::Vec3i viewDirection_;
    cv::Vec3i U_;
    cv::Vec3i V_;
};