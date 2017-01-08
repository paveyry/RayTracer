#include <limits>

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
    std::vector<double> intersections;
    cv::Vec3i dist = raySource - center_;

    // Solve second degree equation
    double a = rayDir.dot(rayDir);
    double b = 2 * rayDir.dot(dist);
    double c = dist.dot(dist) - radius_ * radius_;
    double delta = b * b - 4 * a * c;
    if (delta < 0)
        return -1;
    intersections.push_back(-b + sqrt(delta) / (2 * a));
    intersections.push_back(-b + sqrt(delta) / (2 * a));

    // Find closest intersection
    double closestIntersection = std::numeric_limits<double>::max();
    for (size_t i = 0; i < intersections.size(); ++i)
        if (intersections[i] > 0 && intersections[i] < closestIntersection)
            closestIntersection = intersections[i];

    // Return the closest intersection if there was a valid intersection
    return (closestIntersection != std::numeric_limits<double>::max()) ? closestIntersection : -1;

}

cv::Vec3i Sphere::getNormalVect(const cv::Vec3i pt) const
{
    return cv::normalize(pt - center_);
}

}
