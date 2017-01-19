#include <opencv2/core/core.hpp>
#include <iostream>
#include <assimp/scene.h>
#include <assimp/importerdesc.h>

#include "shapes/Sphere.hh"
#include "Camera.hh"
#include "light/Light.hh"
#include "Scene.hh"

int main()
{
    std::cout << "Main launched" << std::endl;
    Scene* scene = new Scene("inputs/input.txt");
    //scene->triangles_[0].translate(cv::Vec3d(-1, 1, -1.4));
    cv::Mat3b image = scene->compute_image(4);
    scene->show_image(image);
    scene->save_image("lol.png", image);
    return 0;
}
