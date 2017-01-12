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

            cv::Vec3d color = send_ray(cv::Vec3d{0, 0, 0}, rayDirection, 0);
            color(0) = std::min(255., color(0));
            color(1) = std::min(255., color(1));
            color(2) = std::min(255., color(2));

            image.at<cv::Vec3b>(x, y) = color;
        }
    show_image(image);
    //save_image("yolo", image);
}

cv::Vec3d Scene::send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection, int recursion)
{
    std::pair<const shapes::Shape*, double> result;
    result.first = nullptr;
    result.second = std::numeric_limits<double>::max();

    for (std::vector<shapes::Sphere>::iterator it = spheres_.begin() ; it != spheres_.end(); ++it)
        result = find_intersection((*it), result, rayOrigin, rayDirection);
    for (std::vector<shapes::Triangle>::iterator it = triangles_.begin() ; it != triangles_.end(); ++it)
        result = find_intersection((*it), result, rayOrigin, rayDirection);

    // Case no intersection
    if (result.first == nullptr and result.second == std::numeric_limits<double>::max()) {
        return cv::Vec3d{0., 0., 0.};
    }

    cv::Vec3d intersectionPoint = rayOrigin + rayDirection * result.second;
    cv::Vec3d normal = result.first->getNormalVect(intersectionPoint);

    cv::Vec3d color = cv::Vec3d{0., 0., 0.};

    bool isIn = false;
    if (normal.dot(rayDirection) > 0)
    {
        isIn = true;
        normal *= -1;
    }
    // Case if diffused shape or recursion depth reached
    if (result.first->reflectionType_ == shapes::DIFFUSED || recursion >= 3)
        color = compute_diffuse_component(result, color, intersectionPoint, normal, rayOrigin);
    else if (result.first->reflectionType_ == shapes::SPECULAR)
    {
        cv::Vec3d reflectionDirection = cv::normalize(rayDirection - (normal * 2 * rayDirection.dot(normal)));
        double fresnelEffect = 0.1 + pow(1 - (-1 * rayDirection.dot(normal)), 3) * 0.9;

        cv::Vec3d reflectionColor = send_ray(intersectionPoint + reflectionDirection * 0.01, reflectionDirection, recursion + 1);
        cv::Vec3d refractionColor = cv::Vec3d{0, 0, 0};

        if (result.first->alpha_ > 0) {
            double n = isIn ? result.first->alpha_ : (1 / result.first->alpha_);
            double c1 = -rayDirection.dot(normal);
            double c2 = 1 - (n * n * (1 - c1 * c1));

            if (c2 > 0) {
                cv::Vec3d refractionDirection = cv::normalize((rayDirection * n) + (normal * (n * c1 - sqrt(c2))));
                cv::Vec3d refractionColor = send_ray(intersectionPoint + refractionDirection * 0.01,
                                                     refractionDirection, recursion + 1);
            }
        }
        color(0) += (reflectionColor(0) * fresnelEffect) +
                    (refractionColor(0) * (1 - fresnelEffect)) * result.first->color_(0);
        color(1) += (reflectionColor(1) * fresnelEffect) +
                    (refractionColor(1) * (1 - fresnelEffect)) * result.first->color_(1);
        color(2) += (reflectionColor(2) * fresnelEffect) +
                    (refractionColor(2) * (1 - fresnelEffect)) * result.first->color_(2);
    }
    return color;
}

cv::Vec3d Scene::compute_diffuse_component(std::pair<const shapes::Shape*, double> result, cv::Vec3d color,
                                           const cv::Vec3d &intersectionPoint, const cv::Vec3d &normal,
                                           const cv::Vec3d rayOrigin)
{
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
        //if (result.first->reflectionType_ == 0)
        //    std::cout << intersectionPoint << "      " << lip << std::endl;

        if ((intersectionPoint.val[0] >= lip.val[0] - 0.00001 && intersectionPoint.val[0] <= lip.val[0] + 0.00001
            && intersectionPoint.val[1] >= lip.val[1] - 0.00001 && intersectionPoint.val[1] <= lip.val[1] + 0.00001
            && intersectionPoint.val[2] >= lip.val[2] - 0.00001 && intersectionPoint.val[2] <= lip.val[2] + 0.00001)
            || (result.second == std::numeric_limits<double>::max()))
        {
            lightDirection *= -1;
            double colorFactor = normal.dot(lightDirection) * 8;
            cv::Vec3d distance = (intersectionPoint - lightOrigin);
            double d = sqrt(distance.dot(distance));
            if (colorFactor > 0) {
                color(0) += (result.first->color_(0) / 255.) * it->color_(0) * colorFactor / d;
                color(1) += (result.first->color_(1) / 255.) * it->color_(1) * colorFactor / d;
                color(2) += (result.first->color_(2) / 255.) * it->color_(2) * colorFactor / d;
            }
            if(result.first->reflectionType_ == shapes::SPECULAR){
                cv::Vec3d V = cv::normalize(rayOrigin - intersectionPoint);
                double shininess = normal.dot(cv::normalize(V + lightDirection));
                 if(shininess > 0){
                    shininess = pow(shininess, 50) / d;
                    color(0) += result.first->color_(0) * it->color_(0) * shininess;
                    color(1) += result.first->color_(1) * it->color_(1) * shininess;
                    color(2) += result.first->color_(2) * it->color_(2) * shininess;
                }
            }
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