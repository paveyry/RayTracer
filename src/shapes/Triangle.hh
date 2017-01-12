#pragma once

#include "Shape.hh"

namespace shapes
{

class Triangle : public Shape
{
public:
    Triangle(cv::Vec3d p1, cv::Vec3d p2, cv::Vec3d p3, cv::Vec3d color, double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const override;

    virtual cv::Vec3d getNormalVect(const cv::Vec3d) const override;

    virtual void translate(const cv::Vec3d& vec) override;

    virtual void scale(double factor) override;

    virtual void rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin) override;

    cv::Vec3d barycenter() const;
public:
    cv::Vec3d p1_;
    cv::Vec3d p2_;
    cv::Vec3d p3_;
};

}
