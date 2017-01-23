#include <opencv2/core/core.hpp>
#include <iostream>
#include <algorithm>

#include "scenarios/demovideo.hh"
#include "Camera.hh"
#include "Scene.hh"

namespace scenarios
{

void demovideo(cv::VideoWriter& video, const std::shared_ptr<Scene>& scene)
{
    cv::Mat3b image;

    int framecount = 0;

    // Empty mesh
    std::vector<shapes::Triangle> mesh_elts = std::move(scene->meshes_[0].triangles_);
    scene->meshes_[0].triangles_ = std::vector<shapes::Triangle>();
    std::cerr << mesh_elts.size() << '\n';
    for (int i = 0; i < 180; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        image = scene->compute_image(4);
        video << image;
        scene->spheres_[0].translate(cv::Vec3d(0, 0, -0.03));
        scene->lights_[0].rotate(M_PI / 30., 0., 0., cv::Vec3d(0, 0, 0));
        scene->spheres_[1].rotate(M_PI / 30., 0., 0., cv::Vec3d(0, 0, 0));

        // Add triangles 8 by 8
        if (mesh_elts.size() > 0)
        {
            auto it = std::next(mesh_elts.begin(), static_cast<int>(fmin(8ULL, mesh_elts.size())));
            std::move(mesh_elts.begin(), it, std::back_inserter(scene->meshes_[0].triangles_));
            mesh_elts.erase(mesh_elts.begin(), it);
        }
    }
    for (int i = 0; i < 60; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        video << image;
    }
    for (int i = 0; i < 180; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        image = scene->compute_image(4);
        video << image;
        scene->lights_[0].rotate(M_PI / 30., 0., 0., cv::Vec3d(0, 0, 0));
        scene->spheres_[1].rotate(M_PI / 30., 0., 0., cv::Vec3d(0, 0, 0));
        if (mesh_elts.size() > 0)
            mesh_elts.erase(mesh_elts.begin(), mesh_elts.begin() + fmin(8ULL, mesh_elts.size()));
    }
    for (int i = 0; i < 180; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        image = scene->compute_image(4);
        video << image;
        scene->camera_->pos_ += cv::Vec3d(0, 0, 0.018);
        scene->cylinders_[0].rotate(0, M_PI / 60., 0, scene->cylinders_[0].center_);
    }
    for (int i = 0; i < 60; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        video << image;
    }
    for (int i = 0; i < 180; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        image = scene->compute_image(4);
        video << image;
        scene->triangles_[0].translate(cv::Vec3d(0, 0.01, 0.017));
    }
    for (int i = 0; i < 120; ++i)
    {
        if (framecount++ % 10 == 0)
            std::cerr << "Computing frames " << framecount << " to " << framecount + 10 << std::endl;
        image = scene->compute_image(4);
        video << image;
        scene->triangles_[0].scale(0.98);
        scene->spheres_[0].scale(0.98);
        scene->cylinders_[0].scale(0.98);
    }
}

}