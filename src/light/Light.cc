#include "Light.hh"

Light::Light(cv::Vec3d position, cv::Vec3d color, double intensity)
    : position_{position}
    , color_{color}
    , intensity_{intensity}
{}