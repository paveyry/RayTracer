#include "Camera.hh"

Camera::Camera(cv::Vec3i pos, cv::Vec3i lookAt, cv::Vec3i up, double fov, int width, int height)
    : pos_{pos}
    , lookAt_{lookAt}
    , up_{up}
    , fov_{fov}
    , width_{width}
    , height_{height}
    , viewDirection_{lookAt - pos}
    , aspectRatio_{static_cast<double>(width) / static_cast<double>(height)}
{
    U_ = cv::normalize(viewDirection_.cross(up_));
    V_ = cv::normalize(up_.cross(viewDirection_));
    angle_ = tan((M_PI/2)*fov_/180);
}
