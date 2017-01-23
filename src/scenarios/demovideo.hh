#pragma once

#include <opencv2/highgui/highgui.hpp>
#include <Scene.hh>

namespace scenarios
{

void demovideo(cv::VideoWriter& video, const std::shared_ptr<Scene>& scene);

}