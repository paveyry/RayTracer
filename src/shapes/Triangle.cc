#include "Triangle.hh"

namespace shapes
{

Triangle::Triangle(cv::Vec3i p1, cv::Vec3i p2, cv::Vec3i p3, cv::Vec3b color, double alpha,
                   ReflectionType reflectionType)
    : Shape{color, alpha, reflectionType}
    , p1_{p1}
    , p2_{p2}
    , p3_{p3}
{}

double Triangle::intersect(const cv::Vec3i& raySource, const cv::Vec3i rayDir) const
{
    return 0.;
}

cv::Vec3i Triangle::getNormalVect(const cv::Vec3i pt) const
{
    return cv::Vec3i{0, 0, 0};
}
}
