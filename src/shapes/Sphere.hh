#pragma once

#include "Shape.hh"

namespace shapes
{

class Sphere : public Shape
{
public:
    Sphere(cv::Vec3i center, double radius, cv::Vec3b color, double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3i& raySource, const cv::Vec3i rayDir) const override;

    virtual cv::Vec3i getNormalVect(const cv::Vec3i pt) const override;

public:
    cv::Vec3i center_;
    double radius_;
};

}

