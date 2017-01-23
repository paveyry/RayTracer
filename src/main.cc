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

    cv::Mat3b image;

    cv::VideoWriter video("bite2.avi", 10, CV_FOURCC('D','I','V','X'), cv::Size(800, 600), true);
    for (int i = 0; i < 60; ++i)
    {
        scene->spheres_[0].translate(cv::Vec3d(0, 0, 0.016667));
        image = scene->compute_image(1);
        video << image;
    }
    //scene->show_image(image);
    //scene->save_image("lol.png", image);

    return 0;
}
