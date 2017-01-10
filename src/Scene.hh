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
    Scene(std::string file_name);

    void compute_image();
    cv::Vec3d send_ray(cv::Vec3d rayOrigin, cv::Vec3d rayDirection, int recursion);

    template <class T>
    std::pair<T, double> find_intersection(T shape, cv::Vec3d rayOrigin, cv::Vec3d rayDirection);

    cv::Vec3d applyTransform(cv::Vec3d input, cv::Mat t);

    void show_image(cv::Mat image);
    void save_image(std::string filename, cv::Mat image);

    void load_scene(std::string file_name);

public:
    Camera* camera_;
    std::vector<Light> lights_;
    std::vector<shapes::Sphere> spheres_;
    std::vector<shapes::Triangle> triangles_;
};

template <class T>
std::pair<T, double> Scene::find_intersection(T shape, cv::Vec3d rayOrigin, cv::Vec3d rayDirection)
{
    std::cout << "lol" << std::endl;
}