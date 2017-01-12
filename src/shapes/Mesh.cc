#include "Mesh.hh"
#include "tools.hh"

namespace shapes
{

Mesh::Mesh(std::vector<Triangle>&& triangles, const cv::Vec3d& center, const cv::Vec3d& color,
           double alpha, ReflectionType reflectionType)
    : Shape(color, alpha, reflectionType)
    , triangles_{triangles}
    , center_{center}
{}

std::pair<const Triangle*, double> Mesh::intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const
{
    int closestTriangleIdx = -1;
    double closestDist = std::numeric_limits<double>::max();
    for (int i = 0; i < triangles_.size(); ++i)
    {
        double dist = triangles_[i].intersect(raySource, rayDir);
        if (dist < closestDist)
        {
            closestDist = dist;
            closestTriangleIdx = i;
        }
    }
    if (closestTriangleIdx >= 0)
        return std::pair<const Triangle*, double>(&triangles_[closestTriangleIdx], closestDist);
    return std::pair<const Triangle*, double>(nullptr, -1);
}

void Mesh::translate(const cv::Vec3d& vec)
{
    center_ += vec;
    for (auto& triangle : triangles_)
        triangle.translate(vec);
}

void Mesh::scale(double factor)
{
    for (auto& t : triangles_)
    {
        t.p1_ = center_ + (t.p1_ - center_) * factor;
        t.p2_ = center_ + (t.p2_ - center_) * factor;
        t.p3_ = center_ + (t.p3_ - center_) * factor;
    }
}

void Mesh::rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin)
{
    center_ = rotateVec(center_, angleX, angleY, angleZ, origin);
    for (auto& t : triangles_)
        t.rotate(angleX, angleY, angleZ, origin);
}


}