#pragma once

#include "Shape.hh"
#include "Triangle.hh"

namespace shapes
{

class Mesh
{
public:
    Mesh(std::vector<Triangle>&& triangles, const cv::Vec3d& center, const cv::Vec3d& color, double alpha,
         ReflectionType reflectionType);

    //Mesh(std::move(vector));

    std::pair<const Triangle*, double> intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const;

    void translate(const cv::Vec3d& vec);

    void scale(double factor);

    void rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin);

public:
    std::vector<Triangle> triangles_;
    cv::Vec3d center_;
};

}