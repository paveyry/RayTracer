#pragma once
#include <opencv2/core/core.hpp>

class Light
{
public:
    // CONSTRUCTOR
    Light(cv::Vec3i position, cv::Vec3i color);

public:
    cv::Vec3i position_;
    cv::Vec3i color_;
};