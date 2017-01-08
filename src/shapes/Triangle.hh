#pragma once

#include "Shape.hh"

namespace shapes
{

class Triangle : public Shape
{
public:
    Triangle(cv::Vec3i p1, cv::Vec3i p2, cv::Vec3i p3, cv::Vec3b color, double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3i& raySource, const cv::Vec3i rayDir) const override;

    virtual cv::Vec3i getNormalVect(const cv::Vec3i) const override;

public:
    cv::Vec3i p1_;
    cv::Vec3i p2_;
    cv::Vec3i p3_;
};

}
