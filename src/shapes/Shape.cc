#include "Shape.hh"

namespace shapes
{

Shape::Shape(cv::Vec3b color, double alpha, ReflectionType reflectionType, const cv::Mat& transformMatrix)
    : color_{color}
    , alpha_{alpha}
    , reflectionType_{reflectionType}
    , transformMatrix_{transformMatrix}
{}

}
