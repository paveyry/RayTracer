#include <limits>

#include "Sphere.hh"
#include "tools.hh"
#include <iostream>

namespace shapes
{

Sphere::Sphere(cv::Vec3d center, double radius, cv::Vec3d color, double alpha,
               ReflectionType reflectionType, cv::Vec3d phongCoeff)
    : Shape{color, alpha, reflectionType, phongCoeff}
    , center_{center}
    , radius_{radius}
{}

std::pair<const Shape*, double> Sphere::intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const
{
    std::vector<double> intersections;
    cv::Vec3d dist = raySource - center_;

    cv::Vec3d rd = cv::normalize(rayDir);
    // Solve second degree equation
    double a = rd.dot(rd);
    double b = 2 * rd.dot(dist);
    double c = dist.dot(dist) - radius_ * radius_;
    double delta = b * b - 4 * a * c;
    if (delta < 0)
        return std::pair<const Shape*, double>(this, -1);

    intersections.push_back((-b + sqrt(delta)) / (2 * a));
    intersections.push_back((-b - sqrt(delta)) / (2 * a));

    // Find closest intersection
    double closestIntersection = std::numeric_limits<double>::max();
    for (size_t i = 0; i < intersections.size(); ++i)
    {
        if (intersections[i] > 0 && intersections[i] < closestIntersection)
        {
            closestIntersection = intersections[i];
        }
    }

    // Return the closest intersection if there was a valid intersection
    return (closestIntersection != std::numeric_limits<double>::max())
           ? std::pair<const Shape*, double>(this, closestIntersection)
           : std::pair<const Shape*, double>(this, -1);
}

cv::Vec3d Sphere::getNormalVect(const cv::Vec3d pt) const
{
    return cv::normalize(pt - center_);
}

void Sphere::translate(const cv::Vec3d& vec)
{
    center_ += vec;
}

void Sphere::scale(double factor)
{
    if (factor <= 0)
        return;
    radius_ *= factor;
}

void Sphere::rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin)
{
    center_ = rotateVec(center_, angleX, angleY, angleZ, origin);
}


}
