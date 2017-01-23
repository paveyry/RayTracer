#pragma once
#include <opencv2/core/core.hpp>

class Light
{
public:
    // CONSTRUCTOR
    Light(cv::Vec3d position, cv::Vec3d color, double intensity);

public:
    cv::Vec3d position_;
    cv::Vec3d color_;
    double intensity_;
};