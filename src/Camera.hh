#pragma once
#include <opencv2/core/core.hpp>

class Camera
{
public:
    // CONSTRUCTOR
    Camera(cv::Vec3d pos, cv::Vec3d lookAt, cv::Vec3d up, double fov, int width, int height);

public:
    double aspectRatio_;
    double angle_;
    cv::Mat transformView_;
    int width_;
    int height_;
    cv::Vec3d pos_;

private:
    cv::Vec3d lookAt_;
    cv::Vec3d up_;
    double fov_;

    // Attributes computed using the previous ones.
    cv::Vec3d viewDirection_;
    cv::Vec3d U_;
    cv::Vec3d V_;
};