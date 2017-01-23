#pragma once
#include <opencv2/core/core.hpp>

class Light
{
public:
    // CONSTRUCTOR
    Light(cv::Vec3d position, cv::Vec3d color, double intensity);
    void translate(const cv::Vec3d& vec);
    void rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& center);

public:
    cv::Vec3d position_;
    cv::Vec3d color_;
    double intensity_;
};