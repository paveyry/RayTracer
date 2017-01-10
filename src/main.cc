#include <opencv2/core/core.hpp>
#include <iostream>

#include "shapes/Sphere.hh"
#include "Camera.hh"
#include "light/Light.hh"
#include "Scene.hh"

int main()
{
    std::cout << "Main launched" << std::endl;
    Scene* scene = new Scene("inputs/input.txt");
    scene->compute_image();
    return 0;
}
