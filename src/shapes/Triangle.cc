#include "Triangle.hh"
#include "tools.hh"

#include <iostream>

namespace shapes
{

Triangle::Triangle(cv::Vec3d p1, cv::Vec3d p2, cv::Vec3d p3, cv::Vec3d color, double alpha,
                   ReflectionType reflectionType, double phongCoeff)
    : Shape{color, alpha, reflectionType, phongCoeff}
    , p1_{p1}
    , p2_{p2}
    , p3_{p3}
{}

std::pair<const Shape*, double> Triangle::intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const
{
    double rdl = cv::norm(rayDir);
    auto rd = cv::normalize(rayDir);
    auto e1 = p2_ - p1_;
    auto e2 = p3_ - p1_;
    auto h = rd.cross(e2);
    auto a = e1.dot(h);
    if (a > -0.00001 && a < 0.00001)
        return std::pair<const Shape*, double>(this, -1);

    auto s = raySource - p1_;
    double f = 1 / a;
    double u = f * s.dot(h);
    if (u < 0. || u > 1.)
        return std::pair<const Shape*, double>(this, -1);

    auto q = s.cross(e1);
    double v = f * rd.dot(q);
    if (v < 0. || u + v > 1.)
        return std::pair<const Shape*, double>(this, -1);
    double t = f * e2.dot(q);
    if (t > 0.00001)
        return std::pair<const Shape*, double>(this, t / rdl);
    return std::pair<const Shape*, double>(this, -1);
}

cv::Vec3d Triangle::getNormalVect(const cv::Vec3d) const
{
    return cv::normalize((p2_ - p1_).cross(p3_ - p1_));
}

void Triangle::translate(const cv::Vec3d& vec)
{
    p1_ += vec;
    p2_ += vec;
    p3_ += vec;
}

void Triangle::scale(double factor)
{
    if (factor <= 0)
        return;

    cv::Vec3d b = barycenter();
    p1_ = b + (p1_ - b) * factor;
    p2_ = b + (p2_ - b) * factor;
    p3_ = b + (p3_ - b) * factor;
}

void Triangle::rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin)
{
    p1_ = rotateVec(p1_, angleX, angleY, angleZ, origin);
    p2_ = rotateVec(p2_, angleX, angleY, angleZ, origin);
    p3_ = rotateVec(p3_, angleX, angleY, angleZ, origin);
}

cv::Vec3d Triangle::barycenter() const
{
    return p1_ + (2. / 3.) * ((p2_ + p3_) / 2. - p1_);
}

}
