#pragma once

#include "Shape.hh"

namespace shapes
{

class Cylinder : public Shape
{
public:
    Cylinder(const cv::Vec3d& center, double radius, double height, const cv::Vec3d& upDir, cv::Vec3d color,
             double alpha, ReflectionType reflectionType);

    virtual double intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const override;

    virtual cv::Vec3d getNormalVect(const cv::Vec3d pt) const override;

    virtual void translate(const cv::Vec3d& vec) override;

    virtual void scale(double factor) override;

    virtual void rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin) override;

public:
    cv::Vec3d center_;
    double radius_;
    double height_;
    cv::Vec3d upDir_;
};

}