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
    cv::Vec3i up = new cv::Vec3i(0, 1, 0);
    U_ = viewDirection_ * up / cv::normalize(viewDirection_ * up);
    V_ = up * viewDirection_ / cv::normalize(up * viewDirection_);
    aspectRatio_ = (double)width / (double)height;
    angle_ = tan((M_PI/2)*fov_/180);
}
