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
    cv::Vec3i send_ray(cv::Vec3i rayOrigin, cv::Vec3i rayDirection, int recursion);

    cv::Vec3i applyTransform(cv::Vec3i input, cv::Mat t);

    void show_image(cv::Mat image);
    void save_image(std::string filename, cv::Mat image);

    void load_scene(std::string file_name);

public:
    Camera* camera_;
    std::vector<Light> lights_;
    std::vector<shapes::Sphere> spheres_;
    std::vector<shapes::Triangle> triangles_;
};
