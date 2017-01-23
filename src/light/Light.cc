#include <shapes/tools.hh>

#include "Light.hh"

Light::Light(cv::Vec3d position, cv::Vec3d color, double intensity)
    : position_{position}
    , color_{color}
    , intensity_{intensity}
{}

void Light::translate(const cv::Vec3d& vec)
{
    position_ += vec;
}

void Light::rotate(double angleX, double angleY, double angleZ, const cv::Vec3d& center)
{
    position_ = shapes::rotateVec(position_, angleX, angleY, angleZ, center);
}





