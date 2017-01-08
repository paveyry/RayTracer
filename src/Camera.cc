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

    cv::Vec3i eye;
    eye.val[0] = U_.dot(pos_) * -1;
    eye.val[1] = V_.dot(pos_) * -1;
    eye.val[2] = viewDirection_.dot(pos_) * -1;
    
    float matrix_values[16] = {U_.val[0], U_.val[1], U_.val[2], eye.val[0],
                               V_.val[0], V_.val[1], V_.val[2], eye.val[2],
                               viewDirection_.val[0], viewDirection_.val[1], viewDirection_.val[2], eye.val[2],
                               0, 0, 0, 1};
    transformView_ = cv::Mat(4, 4, CV_64F, matrix_values);

}
