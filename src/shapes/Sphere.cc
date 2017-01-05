#include "Sphere.hh"

namespace shapes
{

Sphere::Sphere(cv::Vec3i center, double radius, cv::Vec3b color, double alpha, ReflectionType reflectionType,
               const cv::Mat& transformMatrix)
    : Shape{color, alpha, reflectionType, transformMatrix}
    , center_{center}
    , radius_{radius}
{}

double Sphere::intersect(const cv::Vec3i& raySource, const cv::Vec3i rayDir) const
{
    return 0.;
}

cv::Vec3i Sphere::getNormalVect(const cv::Vec3i pt) const
{
    return cv::Vec3i{0, 0, 0};
}

}
