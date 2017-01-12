#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Camera.hh"
#include "light/Light.hh"
#include "shapes/Sphere.hh"
#include "shapes/Triangle.hh"

class Scene
{
public:
    Scene(const std::string& file_name);

    cv::Mat3b compute_image(double samplingNumber);
    cv::Vec3d send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection, int recursion);
    cv::Vec3d compute_diffuse_component(std::pair<const shapes::Shape*, double> result, cv::Vec3d color,
                                        const cv::Vec3d &intersectionPoint, const cv::Vec3d &normal,
                                        const cv::Vec3d rayOrigin);

    template <class T>
    std::pair<const shapes::Shape*, double> find_intersection(const T& shape, std::pair<const shapes::Shape*, double> result,
                                            const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection);

    cv::Vec3d applyTransform(const cv::Vec3d& input, const cv::Mat& t);

    void show_image(const cv::Mat& image);
    void save_image(const std::string& filename, const cv::Mat& image);

    void load_scene(const std::string& file_name);

public:
    Camera* camera_;
    std::vector<Light> lights_;
    std::vector<shapes::Sphere> spheres_;
    std::vector<shapes::Triangle> triangles_;
};

template <class T>
std::pair<const shapes::Shape*, double> Scene::find_intersection(const T& shape, std::pair<const shapes::Shape*, double> result,
                                                                 const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection)
{
    double point = shape.intersect(rayOrigin, rayDirection);
    if (point > 0 and result.second > point)
    {
        result.second = point;
        result.first = &shape;

    }
    return result;
}