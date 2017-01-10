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

    void compute_image();
    cv::Vec3b send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection, int recursion);

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
    //std::cout << "Test" << std::endl;
    if (point > 0 and result.second > point)
    {
        result.second = point;
        result.first = &shape;
        std::cout << result.second << std::endl;
        std::cout << "Test1" << std::endl;
    }
    //std::cout << "Test2" << std::endl;
    return result;
}