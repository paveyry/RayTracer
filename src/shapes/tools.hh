#pragma once

#include <opencv2/core/core.hpp>

namespace shapes
{

cv::Vec3d rotateVec(const cv::Vec3d& point, double angleX, double angleY, double angleZ, const cv::Vec3d& center);

}