#include "Triangle.hh"

namespace shapes
{

Triangle::Triangle(cv::Vec3d p1, cv::Vec3d p2, cv::Vec3d p3, cv::Vec3b color, double alpha,
                   ReflectionType reflectionType)
    : Shape{color, alpha, reflectionType}
    , p1_{p1}
    , p2_{p2}
    , p3_{p3}
{}

double Triangle::intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const
{
    auto e1 = p2_ - p1_;
    auto e2 = p3_ - p1_;
    auto h = rayDir.cross(e2);
    auto a = e1.dot(h);
    if (a > -0.00001 && a < 0.00001)
        return -1;

    auto s = raySource - p1_;
    double f = 1 / a;
    double u = f * s.dot(h);
    if (u < 0. || u > 1.)
        return -1;

    auto q = s.cross(e1);
    double v = f * rayDir.dot(q);
    if (v < 0. || u + v > 1.)
        return -1;
    double t = f * e2.dot(q);
    if (t > 0.00001)
        return t * sqrt(rayDir.dot(rayDir));
    return -1;
}

cv::Vec3d Triangle::getNormalVect(const cv::Vec3d) const
{
    return cv::normalize((p2_ - p1_).cross(p3_ - p1_));
}

void Triangle::translate(const cv::Vec3d& vec)
{

}

void Triangle::scale(double factor)
{

}


}
