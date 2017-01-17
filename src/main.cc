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
    //for (auto& sp : scene->spheres_)
        //sp.(0.4, 0, 0, cv::Vec3d(0,0,0));
        //sp.translate(cv::Vec3d(1., 1., 0));
        //sp.scale(2.);
   // std::cout << scene->triangles_[0].barycenter();
    //scene->triangles_[0].translate(cv::Vec3d(-1.6, 1, -1.4));
    //scene->triangles_[0].scale(0.5);
    //scene->triangles_[0].rotate(0.5, 0., 0., scene->triangles_[0].barycenter());
    //scene->triangles_[0].rotate(0.2, 0, 0, scene->triangles_[0].barycenter());
    //scene->triangles_ = std::vector<shapes::Triangle>();
    //scene->spheres_[0].rotate(0, 0, M_PI, scene->spheres_[0].center_);
    //scene->triangles_[0].rotate(0, 0.5, 0, scene->triangles_[0].barycenter());
   // std::cout << scene->triangles_[0].barycenter();
    //scene->triangles_[0].rotate(0, M_PI / 5, 0., scene->triangles_[0].barycenter());
   // std::cout << scene->triangles_.size();
    cv::Mat3b image = scene->compute_image(1);
    scene->show_image(image);
    scene->save_image("lol.png", image);
    return 0;
}
