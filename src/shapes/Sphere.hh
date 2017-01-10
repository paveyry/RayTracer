#pragma once

#include "Shape.hh"

namespace shapes
{

class Sphere : public Shape
{
public:
    Sphere(cv::Vec3d center, double radius, cv::Vec3b color, double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const override;

    virtual cv::Vec3d getNormalVect(const cv::Vec3d pt) const override;

    virtual void translate(const cv::Vec3d& vec) override;

    virtual void scale(double factor) override;

public:
    cv::Vec3d center_;
    double radius_;
};

}

