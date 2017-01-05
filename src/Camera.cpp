#include <iostream>
#include "Camera.hpp"

Camera::Camera(cv::Vec3i eye, cv::Vec3i lookAt, cv::Vec3i up, double fov, int width, int height)
    : eye_(eye)
    , lookAt_(lookAt)
    , up_(up)
    , fov_(fov)
    , width_(width)
    , height_(height)
{
    viewDirection_ = lookAt_ - eye_;
    U_ = cv::normalize(cross(viewDirection_, up_));
    V_ = cv::normalize(cross(up_, viewDirection_));
    aspectRatio_ = (double)width / (double)height;
    angle_ = tan((M_PI/2)*fov_/180);
}

cv::Vec3i Camera::cross(cv::Vec3i a, cv::Vec3i b)
{
    int x = a.val[1] * b.val[2] - a.val[2] * b.val[1];
    int y = a.val[2] * b.val[0] - a.val[0] * b.val[2];
    int z = a.val[0] * b.val[1] - a.val[1] * b.val[0];
    return cv::Vec3i{x, y, z};
}
