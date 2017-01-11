#include "Scene.hh"

Scene::Scene(const std::string& file_name)
{
    load_scene(file_name);
}

void Scene::compute_image()
{
    cv::Mat3b image = cv::Mat3b(camera_->width_, camera_->height_, cv::Vec3d{0,0,0});
    for (int y = 0; y < camera_->height_; ++y)
        for (int x = 0; x < camera_->width_; ++x)
        {
            double pX = (2 * ((x + 0.5) / camera_->width_) - 1) * camera_->angle_ * camera_->aspectRatio_;
            double pY = (1 - 2 * (y + 0.5) / camera_->height_) * camera_->angle_;

            // - rayOrigin in the formula but here its {0, 0, 0} ...
            cv::Vec3d rayDirection = cv::Vec3d{pX, pY, -1};

            //rayDirection = applyTransform(rayDirection, camera_->transformView_);
            rayDirection = cv::normalize(rayDirection);

            image.at<cv::Vec3b>(x, y) = send_ray(cv::Vec3d{0, 0, 0}, rayDirection, 0);
        }
    show_image(image);
    //save_image("yolo", image);
}

cv::Vec3b Scene::send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection, int recursion)
{
    std::pair<const shapes::Shape*, double> result;
    result.first = NULL;
    result.second = std::numeric_limits<double>::max();

    for (std::vector<shapes::Sphere>::iterator it = spheres_.begin() ; it != spheres_.end(); ++it)
        result = find_intersection((*it), result, rayOrigin, rayDirection);

    //std::cout << (result.first == NULL) << std::endl;
    /*for (std::vector<shapes::Triangle>::iterator it = triangles_.begin() ; it != triangles_.end(); ++it)
        result = find_intersection((*it), result, rayOrigin, rayDirection);*/

    // Case no intersection
    if (result.first == NULL and result.second == std::numeric_limits<double>::max()) {
        return cv::Vec3b{0, 0, 0};
    }

    cv::Vec3d intersectionPoint = rayOrigin + rayDirection * result.second;
    cv::Vec3d normal = result.first->getNormalVect(intersectionPoint);

    cv::Vec3b color = cv::Vec3b{0, 0, 0};

    // Case if diffused shape or recursion depth reached
    for (std::vector<Light>::iterator it = lights_.begin(); it != lights_.end(); ++it)
    {
        /*checking if ray from light to object is intervened then shadow else light*/
        cv::Vec3d lightOrigin = (*it).position_;
        cv::Vec3d lightDirection = cv::normalize(intersectionPoint - lightOrigin);
        result.second = std::numeric_limits<double>::max();

        for (std::vector<shapes::Sphere>::iterator it = spheres_.begin() ; it != spheres_.end(); ++it)
            result = find_intersection((*it), result, lightOrigin, lightDirection);
        for (std::vector<shapes::Triangle>::iterator it = triangles_.begin() ; it != triangles_.end(); ++it)
            result = find_intersection((*it), result, lightOrigin, lightDirection);

        cv::Vec3d lip = (lightOrigin + lightDirection * result.second);
        if (intersectionPoint.val[0] >= lip.val[0] - 0.00001 && intersectionPoint.val[0] <= lip.val[0] + 0.00001
            && intersectionPoint.val[1] >= lip.val[1] - 0.00001 && intersectionPoint.val[1] <= lip.val[1] + 0.00001
            && intersectionPoint.val[2] >= lip.val[2] - 0.00001 && intersectionPoint.val[2] <= lip.val[2] + 0.00001)
        {
            double colorFactor = normal.dot(lightDirection) * 10;
            if (colorFactor > 0) {
                cv::Vec3d distance = (intersectionPoint - lightOrigin);
                double d = distance.dot(distance);
                color(0) += std::min(255., ((static_cast<double>(result.first->color_(0)) / 255.)
                                            * (static_cast<double>(it->color_(0))) * colorFactor * (1 / d)));
                color(1) += std::min(255., ((static_cast<double>(result.first->color_(1)) / 255.)
                                            * (static_cast<double>(it->color_(1))) * colorFactor * (1 / d)));
                color(2) += std::min(255., ((static_cast<double>(result.first->color_(2)) / 255.)
                                            * (static_cast<double>(it->color_(2))) * colorFactor * (1 / d))); }
        }
    }
    return color;
}

cv::Vec3d Scene::applyTransform(const cv::Vec3d& input, const cv::Mat& t)
{
    cv::Vec3d result;
    result.val[0] = input.dot(cv::Vec3d{t.at<double>(0, 0), t.at<double>(0, 1), t.at<double>(0, 2)});
    result.val[1] = input.dot(cv::Vec3d{t.at<double>(1, 0), t.at<double>(1, 1), t.at<double>(1, 2)});
    result.val[2] = input.dot(cv::Vec3d{t.at<double>(2, 0), t.at<double>(2, 1), t.at<double>(2, 2)});
    return result;
}

void Scene::show_image(const cv::Mat& image)
{
    cv::namedWindow("generated", cv::WINDOW_NORMAL);
    cv::imshow("generated", image);
    cv::waitKey(0);
}

void Scene::save_image(const std::string& filename, const cv::Mat& image)
{
    cv::imwrite(filename, image);
}

void Scene::load_scene(const std::string& file_name)
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
            cv::Vec3d e;
            cv::Vec3d lk;
            cv::Vec3d u;
            int fov;
            int w;
            int h;
            iss >> e.val[0] >> e.val[1] >> e.val[2] >> lk.val[0] >> lk.val[1] >> lk.val[2];
            iss >> u.val[0] >> u.val[1] >> u.val[2] >> fov >> w >> h;
            camera_ = new Camera(e, lk, u, fov, w, h);
        }
        else if (word.compare("Light") == 0)
        {
            cv::Vec3d pos;
            cv::Vec3d col;
            iss >> pos.val[0] >> pos.val[1] >> pos.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            lights_.push_back(Light(pos, col));
        }
        else if (word.compare("Sphere") == 0)
        {
            cv::Vec3d c;
            double r;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            iss >> c.val[0] >> c.val[1] >> c.val[2] >> r >> col.val[0] >> col.val[1] >> col.val[2] >> alpha;
            iss >> word;
            if (word.compare("SPECULAR") == 0)
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            spheres_.push_back(shapes::Sphere(c, r, col, alpha, ref));
        }
        else if (word.compare("Triangle") == 0)
        {
            cv::Vec3d p1;
            cv::Vec3d p2;
            cv::Vec3d p3;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            iss >> p1.val[0] >> p1.val[1] >> p1.val[2] >> p2.val[0] >> p2.val[1] >> p2.val[2];
            iss >> p3.val[0] >> p3.val[1] >> p3.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            iss >> alpha >> word;
            if (word.compare("SPECULAR") == 0)
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            triangles_.push_back(shapes::Triangle(p1, p2, p3, col, alpha, ref));
        }
    }
    file.close();
}