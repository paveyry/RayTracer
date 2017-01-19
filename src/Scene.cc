#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <array>
#include <tbb/tbb.h>

#include "Scene.hh"
#include "util.hh"

Scene::Scene(const std::string& file_name)
{
    load_scene(file_name);
}

tbb::atomic<size_t> itercount;
cv::Mat3b Scene::compute_image(double samplingNumber)
{
    size_t numTriang = triangles_.size();
    for (auto& mesh : meshes_)
        numTriang += mesh.triangles_.size();
    std::cout << "The scene contains " << spheres_.size() << " spheres, " << cylinders_.size() << " cylinders, and "
              << numTriang << " triangles (" << meshes_.size() << " meshes)."  << std::endl;


    cv::Mat3b image = cv::Mat3b(camera_->width_, camera_->height_, cv::Vec3d{0,0,0});
    tbb::parallel_for(0, camera_->height_ * camera_->width_,
                      1, [&](int pos)
                      {
                          int x = pos % camera_->width_;
                          int y = pos / camera_->width_;
                          ++itercount;
                          if (itercount % 5000 == 0)
                              std::cerr << (static_cast<double>(itercount)
                                           / static_cast<double>(camera_->width_ * camera_->height_)) * 100 << "%\n";

                          cv::Vec3d color = cv::Vec3d{0, 0, 0};
                          for (int i = 0; i < sqrt(samplingNumber); ++i)
                          {
                              double sx = (double) x + ((double) i / sqrt(samplingNumber));
                              double sy = (double) y + ((double) i / sqrt(samplingNumber));
                              double pX = (2 * ((sx + 0.5) / camera_->width_) - 1) * camera_->angle_ *
                                          camera_->aspectRatio_;
                              double pY = (1 - 2 * (sy + 0.5) / camera_->height_) * camera_->angle_;
                              cv::Vec3d rayDirection = cv::Vec3d{pX, pY, -1};
                              rayDirection = cv::normalize(rayDirection);
                              color += send_ray(camera_->pos_, rayDirection, 0);
                          }
                          color /= sqrt(samplingNumber);
                          color(0) = std::min(255., color(0));
                          color(1) = std::min(255., color(1));
                          color(2) = std::min(255., color(2));

                          image.at<cv::Vec3b>(x, y) = color;
                      });
    return image;
}

cv::Vec3d Scene::send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection, int recursion)
{
    std::pair<const shapes::Shape*, double> result;
    result.first = nullptr;
    result.second = std::numeric_limits<double>::max();

    for (const auto& sphere : spheres_)
        result = find_intersection(sphere, result, rayOrigin, rayDirection);
    for (const auto& triangle : triangles_)
        result = find_intersection(triangle, result, rayOrigin, rayDirection);
    for (const auto& cylinder : cylinders_)
        result = find_intersection(cylinder, result, rayOrigin, rayDirection);
    for (const auto& mesh : meshes_)
        result = find_intersection(mesh, result, rayOrigin, rayDirection);

    // Case no intersection
    if (!result.first && result.second == std::numeric_limits<double>::max())
        return cv::Vec3d{0., 0., 0.};

    cv::Vec3d intersectionPoint = rayOrigin + rayDirection * result.second;
    cv::Vec3d normal = result.first->getNormalVect(intersectionPoint);

    cv::Vec3d color = cv::Vec3d{0., 0., 0.};

    bool isIn = false;
    if (normal.dot(rayDirection) > 0)
    {
        isIn = true;
        normal *= -1;
    }

    if(result.first->reflectionType_ == shapes::SPECULAR && recursion < 3)
    {
        cv::Vec3d reflectionDirection = cv::normalize(rayDirection - (2 * rayDirection.dot(normal)) * normal);
        double facingRatio = -1 * rayDirection.dot(normal);
        double fresnelEffect = 0.1 + pow(1 - facingRatio, 3) * 0.9;

        cv::Vec3d reflectionColor = send_ray(intersectionPoint + reflectionDirection * 0.01, reflectionDirection, recursion + 1);
        cv::Vec3d refractionColor = cv::Vec3d{1, 1, 1};
        if (result.first->alpha_ > 0) {
            double n = isIn ? result.first->alpha_ : (1 / result.first->alpha_);
            double c1 = -rayDirection.dot(normal);
            double c2 = 1 - (n * n * (1 - c1 * c1));

            if (c2 > 0) {
                cv::Vec3d refractionDirection = cv::normalize((rayDirection * n) + (normal * (n * c1 - sqrt(c2))));
                cv::Vec3d refractionColor = send_ray(intersectionPoint + refractionDirection * 0.01, refractionDirection, recursion + 1);
            }
        }
        color(0) += (reflectionColor(0) * fresnelEffect) +
                    (refractionColor(0) * (1 - fresnelEffect)) * result.first->color_(0);
        color(1) += (reflectionColor(1) * fresnelEffect) +
                    (refractionColor(1) * (1 - fresnelEffect)) * result.first->color_(1);
        color(2) += (reflectionColor(2) * fresnelEffect) +
                    (refractionColor(2) * (1 - fresnelEffect)) * result.first->color_(2);
    }
    else {
        color = compute_diffuse_component(result, color, intersectionPoint, normal, rayOrigin);
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

        for (const auto& sphere : spheres_)
            result = find_intersection(sphere, result, lightOrigin, lightDirection);
        for (const auto& triangle : triangles_)
            result = find_intersection(triangle, result, lightOrigin, lightDirection);
        for (const auto& cylinder : cylinders_)
            result = find_intersection(cylinder, result, lightOrigin, lightDirection);
        for (const auto& mesh : meshes_)
            result = find_intersection(mesh, result, lightOrigin, lightDirection);

        cv::Vec3d lip = (lightOrigin + lightDirection * result.second);
        //if (result.first->reflectionType_ == 0)
        //    std::cout << intersectionPoint << "      " << lip << std::endl;
        //if (result.first->reflectionType_ == shapes::SPECULAR && result.second != std::numeric_limits<double>::max())
        //    std::cout << "merde" << std::endl;

        if ((intersectionPoint.val[0] >= lip.val[0] - 0.00001 && intersectionPoint.val[0] <= lip.val[0] + 0.00001
            && intersectionPoint.val[1] >= lip.val[1] - 0.00001 && intersectionPoint.val[1] <= lip.val[1] + 0.00001
            && intersectionPoint.val[2] >= lip.val[2] - 0.00001 && intersectionPoint.val[2] <= lip.val[2] + 0.00001)
            || (result.second == std::numeric_limits<double>::max()))
        {
            lightDirection *= -1;
            double colorFactor = normal.dot(lightDirection) * 8;
            cv::Vec3d distance = (intersectionPoint - lightOrigin);
            double d = sqrt(distance.dot(distance));
            if (colorFactor > 0)
            {
                double phong = result.first->phongCoeff_;
                color(0) += (result.first->color_(0) / 255.) * phong * it->color_(0) * colorFactor / d;
                color(1) += (result.first->color_(1) / 255.) * phong * it->color_(1) * colorFactor / d;
                color(2) += (result.first->color_(2) / 255.) * phong * it->color_(2) * colorFactor / d;
            }
            if(result.first->reflectionType_ == shapes::SPECULAR)
            {
                cv::Vec3d V = cv::normalize(rayOrigin - intersectionPoint);
                double shininess = normal.dot(cv::normalize(V + lightDirection));
                 if(shininess > 0)
                 {
                     shininess = pow(shininess, 10) / d;
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
        if (word == "Camera")
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
        else if (word == "Light")
        {
            cv::Vec3d pos;
            cv::Vec3d col;
            iss >> pos.val[0] >> pos.val[1] >> pos.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            lights_.push_back(Light(pos, col));
        }
        else if (word == "Sphere")
        {
            cv::Vec3d c;
            double r;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            double phongCoeff;
            iss >> c.val[0] >> c.val[1] >> c.val[2] >> r >> col.val[0] >> col.val[1] >> col.val[2] >> alpha;
            iss >> word >> phongCoeff;
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            spheres_.push_back(shapes::Sphere(c, r, col, alpha, ref, phongCoeff));
        }
        else if (word == "Triangle")
        {
            cv::Vec3d p1;
            cv::Vec3d p2;
            cv::Vec3d p3;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            double phongCoeff;
            iss >> p1.val[0] >> p1.val[1] >> p1.val[2] >> p2.val[0] >> p2.val[1] >> p2.val[2];
            iss >> p3.val[0] >> p3.val[1] >> p3.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            iss >> alpha >> word >> phongCoeff;
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            triangles_.push_back(shapes::Triangle(p1, p2, p3, col, alpha, ref, phongCoeff));
        }
        else if (word == "Cylinder")
        {
            cv::Vec3d c;
            double r;
            double h;
            cv::Vec3d updir;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            double phongCoeff;
            iss >> c(0) >> c(1) >> c(2) >> r >> h >> updir(0) >> updir(1) >> updir(2);
            iss >> col(0) >> col(1) >> col(2) >> alpha >> word >> phongCoeff;
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            cylinders_.push_back(shapes::Cylinder(c, r, h, updir, col, alpha, ref, phongCoeff));
        }
        else if (word == "Asset")
        {
            std::string file;
            cv::Vec3d pos;
            shapes::ReflectionType ref;
            double phongCoeff;
            bool triangulate;
            iss >> file >> pos(0) >> pos(1) >> pos(2) >> word >> phongCoeff >> triangulate;
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            import3Dasset(file, ref, phongCoeff, pos, triangulate);
        }
    }
    file.close();
}


void Scene::import3Dasset(const std::string& file, shapes::ReflectionType reflectionType,
                          double phongCoeff, const cv::Vec3d& position, bool triangulate)
{
    Assimp::Importer importer;
    importer.ReadFile(file, triangulate ? aiPostProcessSteps::aiProcess_Triangulate : 0);
    const aiScene* assimpScene = importer.GetScene();
    aiMesh** meshes = assimpScene->mMeshes;
    size_t nbMeshes = assimpScene->mNumMeshes;
    for (size_t i = 0; i < nbMeshes; ++i)
    {
        size_t nbFaces = meshes[i]->mNumFaces;
        aiFace* faces = meshes[i]->mFaces;
        aiVector3D* vertices = meshes[i]->mVertices;
        std::vector<shapes::Triangle> triangles;
        for (size_t j = 0; j < nbFaces; ++j)
        {
            if (faces[j].mNumIndices != 3)
                continue;
            std::vector<cvaiVec3> vert;
            for (size_t k = 0; k < 3; ++k)
                vert.push_back(vertices[faces[j].mIndices[k]]);
            triangles.emplace_back(vert[0], vert[1], vert[2], cv::Vec3d(230, 220, 230), 255,
                                   reflectionType, phongCoeff);

        }
        shapes::Mesh mesh(std::move(triangles), cv::Vec3d{0, 0, 0, });
        mesh.rotate(0, 0, M_PI / 2., mesh.center_);
        mesh.translate(position);
        meshes_.push_back(std::move(mesh));
        auto& m = meshes_[meshes_.size() - 1];
    }
    std::cout << file << " was successfully imported" << std::endl;
}
