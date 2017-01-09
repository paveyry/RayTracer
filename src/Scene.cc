#include "Scene.hh"

Scene::Scene(std::string file_name)
{
    load_scene(file_name);
}

void Scene::compute_image()
{
    cv::Mat3i image = cv::Mat3i(camera_->width_, camera_->height_, cv::Vec3i{0,0,0});
    for (int y = 0; y < camera_->height_; ++y)
        for (int x = 0; x < camera_->width_; ++x)
        {
            float pX = (2 * ((x + 0.5) / camera_->width_) - 1) * camera_->angle_ * camera_->aspectRatio_;
            float pY = (1 - 2 * (y + 0.5) / camera_->height_) * camera_->angle_;

            cv::Vec3i rayDirection = cv::Vec3i{pX, pY, -1}; // - rayOrigin in the formula but here its {0, 0, 0} ...

            rayDirection = applyTransform(rayDirection, camera_->transformView_);
            rayDirection = cv::normalize(rayDirection);

            image.at<cv::Vec3i>(x, y) = send_ray(cv::Vec3i{0, 0, 0}, rayDirection, 0);
        }
    //show_image(image);
    //save_image("yolo", image);
}

cv::Vec3i Scene::send_ray(cv::Vec3i rayOrigin, cv::Vec3i rayDirection, int recursion)
{
    std::cout << "FIXME" << std::endl;
    return cv::Vec3i{0, 0, 0};
}

cv::Vec3i Scene::applyTransform(cv::Vec3i input, cv::Mat t)
{
    cv::Vec3i result;
    result.val[0] = input.dot(cv::Vec3i{t.at<int>(0, 0), t.at<int>(0, 1), t.at<int>(0, 2)});
    result.val[1] = input.dot(cv::Vec3i{t.at<int>(1, 0), t.at<int>(1, 1), t.at<int>(1, 2)});
    result.val[2] = input.dot(cv::Vec3i{t.at<int>(2, 0), t.at<int>(2, 1), t.at<int>(2, 2)});
    return result;
}

void Scene::show_image(cv::Mat image)
{
    cv::namedWindow("generated", cv::WINDOW_NORMAL);
    cv::imshow("generated", image);
    cv::waitKey(0);
}

void Scene::save_image(std::string filename, cv::Mat image)
{
    cv::imwrite(filename, image);
}

void Scene::load_scene(std::string file_name)
{
    std::ifstream file;
    file.open(file_name);
    if (!file.is_open())
        std::cerr << "Error: File cannot be found or does not exist" << std::endl;

    std::string line;
    std::string word;
    while (getline(file, line))
    {
        std::istringstream iss(line);
        iss >> word;
        if (word.compare("Camera") == 0)
        {
            cv::Vec3i e;
            cv::Vec3i lk;
            cv::Vec3i u;
            int fov;
            int w;
            int h;
            iss >> e.val[0] >> e.val[1] >> e.val[2] >> lk.val[0] >> lk.val[1] >> lk.val[2];
            iss >> u.val[0] >> u.val[1] >> u.val[2] >> fov >> w >> h;
            camera_ = new Camera(e, lk, u, fov, w, h);
        }
        else if (word.compare("Light") == 0)
        {
            cv::Vec3i pos;
            cv::Vec3i col;
            iss >> pos.val[0] >> pos.val[1] >> pos.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            lights_.push_back(new Light(pos, col));
        }
        else if (word.compare("Sphere") == 0)
        {
            cv::Vec3i c;
            double r;
            cv::Vec3i col;
            double alpha;
            shapes::ReflectionType ref;
            cv::Vec3i t; // Transpose Matrix
            iss >> c.val[0] >> c.val[1] >> c.val[2] >> r >> col.val[0] >> col.val[1] >> col.val[2] >> alpha;
            iss >> word >> t.val[0] >> t.val[1] >> t.val[2];
            if (word.compare("SPECULAR") == 0)
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            spheres_.push_back(new shapes::Sphere(c, r, col, alpha, ref));
        }
        else if (word.compare("Triangle") == 0)
        {
            cv::Vec3i p1;
            cv::Vec3i p2;
            cv::Vec3i p3;
            cv::Vec3i col;
            double alpha;
            shapes::ReflectionType ref;
            cv::Vec3i t;
            iss >> p1.val[0] >> p1.val[1] >> p1.val[2] >> p2.val[0] >> p2.val[1] >> p2.val[2];
            iss >> p3.val[0] >> p3.val[1] >> p3.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            iss >> alpha >> word >> t.val[0] >> t.val[1] >> t.val[2];
            if (word.compare("SPECULAR") == 0)
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            triangles_.push_back(new shapes::Triangle(p1, p2, p3, col, alpha, ref));
        }
    }
    file.close();
}