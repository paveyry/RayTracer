#include "Shape.hh"

namespace shapes
{

Shape::Shape(cv::Vec3b color, double alpha, ReflectionType reflectionType)
    : color_{color}
    , alpha_{alpha}
    , reflectionType_{reflectionType}
{}

}
