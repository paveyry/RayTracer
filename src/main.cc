#include <opencv2/core/core.hpp>
#include <iostream>
#include <memory>

#include "scenarios/demovideo.hh"
#include "Camera.hh"
#include "Scene.hh"

int main()
{
    std::cout << "Main launched" << std::endl;
    std::shared_ptr<Scene> scene = std::make_shared<Scene>("inputs/input.txt");

/*
     The following commented block is for video synthesis...


    std::string outputfile = "image_synthesis.avi";

    // Remove previous file
    std::remove(outputfile.c_str());

    // Open video write stream
    cv::VideoWriter video(outputfile, CV_FOURCC('M', 'P', '4', '2'), 30,
                          cv::Size(scene->camera_->height_, scene->camera_->width_), true);

    // Check for opening errors
    if (!video.isOpened())
    {
        std::cerr << "Error in opening video writer feed!" << std::endl;
        getchar();
        return 1;
    }

    // Launch scenario rendering
    scenarios::demovideo(video, scene);
*/
    cv::Mat3b image = scene->compute_image(16);
    scene->save_image("generated_image.png", image);
    scene->show_image(image);

    return 0;
}
