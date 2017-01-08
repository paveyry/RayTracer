#pragma once

#include <iostream>
#include <fstream>
#include <sstream>

#include "Camera.hh"
#include "light/Light.hh"
#include "shapes/Sphere.hh"
#include "shapes/Triangle.hh"

class Scene
{
public:
    Scene(std::string file_name);
    void load_scene(std::string file_name);

public:
    Camera* camera_;
    std::vector<Light*> lights_;
    std::vector<shapes::Sphere*> spheres_;
    std::vector<shapes::Triangle*> triangles_;
};
