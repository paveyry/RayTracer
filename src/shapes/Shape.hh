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
    Shape(cv::Vec3d color, double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const = 0;

    virtual cv::Vec3d getNormalVect(const cv::Vec3d pt) const = 0;

    virtual void translate(const cv::Vec3d& vec) = 0;

    virtual void scale(double factor) = 0;

    virtual void rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin) = 0;

public:
    cv::Vec3d color_;
    double alpha_;
    ReflectionType reflectionType_;
};

}