#pragma once
#include <opencv2/core/core.hpp>

namespace shapes
{

enum ReflectionType
{
    SPECULAR,
    DIFFUSED
};


class Shape
{
public:
    Shape(cv::Vec3b color, double alpha, ReflectionType reflectionType, const cv::Mat& transformMatrix);

    virtual double intersect(const cv::Vec3i& raySource, const cv::Vec3i rayDir) const = 0;

    virtual cv::Vec3i getNormalVect(const cv::Vec3i pt) const = 0;

public:
    cv::Vec3i color_;
    double alpha_;
    ReflectionType reflectionType_;
    cv::Mat transformMatrix_;
};

}