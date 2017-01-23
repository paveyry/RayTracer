#include <opencv2/core/core.hpp>
#include <iostream>

#include "scenarios/demovideo.hh"
#include "Camera.hh"
#include "Scene.hh"

int main()
{
    std::cout << "Main launched" << std::endl;
    std::shared_ptr<Scene> scene = std::make_shared<Scene>("inputs/input.txt");

    std::string outputfile = "bite.avi";

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

    //scene->show_image(image);
    //scene->save_image("lol.png", image);

    return 0;
}
