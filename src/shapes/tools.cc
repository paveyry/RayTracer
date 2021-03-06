#include "tools.hh"

namespace shapes
{

cv::Vec3d rotateVec(const cv::Vec3d& point, double angleX, double angleY, double angleZ, const cv::Vec3d& center)
{
    cv::Matx44d xrot = {1.,           0.,           0.,           0.,
                        0.,           cos(angleX),  -sin(angleX), 0.,
                        0.,           sin(angleX),  cos(angleX),  0.,
                        0.,           0.,           0.,           1.};

    cv::Matx44d yrot = {cos(angleY),  0.,           sin(angleY),  0.,
                        0.,           1.,           0.,           0.,
                        -sin(angleY), 0.,           cos(angleY),  0.,
                        0.,           0.,           0.,           1.};

    cv::Matx44d zrot = {cos(angleZ),  -sin(angleZ), 0.,           0.,
                        sin(angleZ),  cos(angleZ),  0.,           0.,
                        0.,           0.,           1.,           0.,
                        0.,           0.,           0.,           1.};

    cv::Matx44d rot = xrot * yrot * zrot;
    cv::Vec4d center4(center(0), center(1), center(2), 1);
    cv::Vec4d res = rot * (cv::Vec4d(point(0), point(1), point(2), 1) - center4) + center4;
    return cv::Vec3d(res(0), res(1), res(2));
}

}