#include "Cylinder.hh"
#include "tools.hh"

#include <iostream>

namespace shapes
{
Cylinder::Cylinder(const cv::Vec3d& center, double radius, double height, const cv::Vec3d& upDir,
                   cv::Vec3d color, double alpha, ReflectionType reflectionType)
    : Shape{color, alpha, reflectionType}
    , center_{center}
    , radius_{radius}
    , height_{height}
    , upDir_{cv::normalize(upDir)}
{}

double Cylinder::intersect(const cv::Vec3d& raySource, const cv::Vec3d& rayDir) const
{
    double rdl = cv::norm(rayDir);
    cv::Vec3d rd = cv::normalize(rayDir);


    std::vector<double> intersections;

    cv::Vec3d alpha = upDir_ * rd.dot(upDir_);
    cv::Vec3d dP = raySource - center_;
    cv::Vec3d beta = upDir_ * dP.dot(upDir_);
    cv::Vec3d upcenter = center_ + upDir_ * height_;

    cv::Vec3d rdminalph = rd - alpha;
    double a = rdminalph.dot(rdminalph);
    double b = 2 * rdminalph.dot(dP - beta);
    double c = (dP - beta).dot(dP - beta) - radius_ * radius_;

    double delta = b * b - 4 * a * c;

    if (delta < 0)
        return -1;

    double root1 = (-b + sqrt(delta)) / (2 * a);
    double root2 = (-b - sqrt(delta)) / (2 * a);

    if (root1 >= 0
        && upDir_.dot(raySource - center_ + rd * root1) > 0
        && upDir_.dot(raySource - upcenter + rd * root1) < 0)
        intersections.push_back(root1);
    if (root2 >= 0
        && upDir_.dot(raySource - center_ + rd * root2) > 0
        && upDir_.dot(raySource - upcenter + rd * root2) < 0)
        intersections.push_back(root2);


    double dirdot = rd.dot(upDir_);
    cv::Vec3d co;

    if (dirdot > 0.000001)
        co = center_ - raySource;
    else if (dirdot < 0.000001)
        co = upcenter - raySource;

    double inter = co.dot(upDir_) / dirdot;
    if (inter > 0 && (rd * inter - co).dot(rd * inter - co) <= radius_ * radius_)
        intersections.push_back(inter);

    double closestIntersection = std::numeric_limits<double>::max();
    for (double intersection : intersections)
        if (closestIntersection > intersection && intersection >= 0)
            closestIntersection = intersection;

    return (closestIntersection != std::numeric_limits<double>::max()) ? closestIntersection / rdl : -1;
}

cv::Vec3d Cylinder::getNormalVect(const cv::Vec3d pt) const
{
    cv::Vec3d co = pt - center_;
    cv::Vec3d coUp = co - upDir_ * height_;
    if (co.dot(co) <= radius_ * radius_ || coUp.dot(coUp) <= radius_ * radius_)
        return upDir_;

    return cv::normalize(co - upDir_ * co.dot(upDir_ / upDir_.dot(upDir_)));
}

void Cylinder::translate(const cv::Vec3d& vec)
{
    center_ += vec;
}

void Cylinder::scale(double factor)
{
    height_ *= factor;
    radius_ *= factor;
}

void Cylinder::rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& origin)
{
    center_ = rotateVec(center_, angleX, angleY, angleZ, origin);
    upDir_ = cv::normalize(rotateVec(upDir_, angleX, angleY, angleZ, origin));
}


}



