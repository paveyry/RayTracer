#pragma once
#include <opencv2/core/core.hpp>

class Camera
{
public:
    Camera(cv::Vec3i eye, cv::Vec3i lookAt, cv::Vec3i up, double fov, int width, int height);

public:
    cv::Vec3i eye_;
    cv::Vec3i lookAt_;
    cv::Vec3i up_;
    double fov_;
    int width_;
    int height_;

    cv::Vec3i viewDirection_;
    cv::Vec3i U_;
    cv::Vec3i V_;
    double aspectRatio_;
    double angle_;
};