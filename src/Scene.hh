#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "Camera.hh"
#include "light/Light.hh"
#include "shapes/Sphere.hh"
#include "shapes/Triangle.hh"
#include "shapes/Cylinder.hh"
#include "shapes/Mesh.hh"

class Scene
{
public:
    Scene(const std::string& file_name);

    cv::Mat3b compute_image(double samplingNumber);
    cv::Vec3d send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection);

    template <class T>
    std::pair<const shapes::Shape*, double>
    find_intersection(const T& shape, std::pair<const shapes::Shape*, double> result,
                      const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection);

    cv::Vec3d applyTransform(const cv::Vec3d& input, const cv::Mat& t);

    void show_image(const cv::Mat& image);
    void save_image(const std::string& filename, const cv::Mat& image);

    void import3Dasset(const std::string& file, shapes::ReflectionType reflectionType,
                       const cv::Vec3d& phongCoeff, double alpha, const cv::Vec3d& pos = {0, 0, 0},
                       bool triangulate = true);

private:
    void load_scene(const std::string& file_name);


public:
    std::shared_ptr<Camera> camera_;
    std::vector<Light> lights_;
    std::vector<shapes::Sphere> spheres_;
    std::vector<shapes::Triangle> triangles_;
    std::vector<shapes::Cylinder> cylinders_;
    std::vector<shapes::Mesh> meshes_;
};

template <class T>
std::pair<const shapes::Shape*, double>
Scene::find_intersection(const T& shape, std::pair<const shapes::Shape*, double> result,
                         const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection)
{
    auto res = shape.intersect(rayOrigin, rayDirection);
    if (res.second > 0 and result.second > res.second)
    {
        result.second = res.second;
        result.first = res.first;
    }
    return result;
}
