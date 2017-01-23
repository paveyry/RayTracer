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
    //auto start = std::chrono::high_resolution_clock::now();

    tbb::parallel_for(0, camera_->height_ * camera_->width_, 1, [&](int pos)
    {
        size_t x = pos % static_cast<size_t>(camera_->width_);
        size_t y = pos / static_cast<size_t>(camera_->width_);
//        if (++itercount % 10000 == 0)
//            std::cerr << (static_cast<double>(itercount)
//                          / static_cast<double>(camera_->width_ * camera_->height_)) * 100 << "%\n";

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
            color += send_ray(camera_->pos_, rayDirection);
        }
        color /= sqrt(samplingNumber);

        color(0) = std::min(255., color(0));
        color(1) = std::min(255., color(1));
        color(2) = std::min(255., color(2));

        image.at<cv::Vec3b>(x, y) = color;
    });

    //auto end = std::chrono::high_resolution_clock::now();
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    return image;
}

cv::Vec3d Scene::send_ray(const cv::Vec3d& rayOrigin, const cv::Vec3d& rayDirection)
{
    std::pair<const shapes::Shape*, double> result{nullptr, std::numeric_limits<double>::max()};

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
        return cv::Vec3d{0, 0, 0};

    cv::Vec3d intersectionPoint = rayOrigin + rayDirection * result.second;
    cv::Vec3d normal = result.first->getNormalVect(intersectionPoint);

    // Ambient light
    cv::Vec3d color = result.first->phongCoeff_(0) * 0.1 * result.first->color_;

    if (normal.dot(rayDirection) > 0)
        normal *= -1;

    for (const Light& light : lights_)
    {
        /*checking if ray from light to object is intervened then shadow else light*/
        cv::Vec3d lightOrigin = light.position_;
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

        // Diffused light
        if ((intersectionPoint.val[0] >= lip.val[0] - 0.0000001 && intersectionPoint.val[0] <= lip.val[0] + 0.0000001
            && intersectionPoint.val[1] >= lip.val[1] - 0.0000001 && intersectionPoint.val[1] <= lip.val[1] + 0.0000001
            && intersectionPoint.val[2] >= lip.val[2] - 0.0000001 && intersectionPoint.val[2] <= lip.val[2] + 0.0000001)
            || (result.second == std::numeric_limits<double>::max()))
        {
            lightDirection *= -1;
            double lightAngle = normal.dot(lightDirection);
            if (lightAngle > 0)
                color += (result.first->color_ / 255.).mul(light.color_)
                         * light.intensity_ * lightAngle * result.first->phongCoeff_(1);

            // Specular light
            if(result.first->reflectionType_ == shapes::SPECULAR)
            {
                cv::Vec3d R = cv::normalize(2. * lightAngle * normal - lightDirection);
                cv::Vec3d V = cv::normalize(rayOrigin - intersectionPoint);
                double shininess = pow(R.dot(V), result.first->alpha_);
                if(shininess > 0)
                    color += (result.first->color_ / 255.).mul(light.color_)
                             * shininess * result.first->phongCoeff_(2) * light.intensity_;
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
            iss >> u.val[0] >> u.val[1] >> u.val[2] >> fov >> h >> w;
            camera_ = std::make_shared<Camera>(e, lk, u, fov, w, h);
        }
        else if (word == "Light")
        {
            cv::Vec3d pos;
            cv::Vec3d col;
            double intensity;
            iss >> pos.val[0] >> pos.val[1] >> pos.val[2] >> col.val[0] >> col.val[1] >> col.val[2] >> intensity;
            lights_.emplace_back(pos, col, intensity);
        }
        else if (word == "Sphere")
        {
            cv::Vec3d c;
            double r;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            cv::Vec3d phongCoeff;
            iss >> c.val[0] >> c.val[1] >> c.val[2] >> r >> col.val[0] >> col.val[1] >> col.val[2] >> alpha;
            iss >> word >> phongCoeff(0) >> phongCoeff(1) >> phongCoeff(2);
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            spheres_.emplace_back(c, r, col, alpha, ref, phongCoeff);
        }
        else if (word == "Triangle")
        {
            cv::Vec3d p1;
            cv::Vec3d p2;
            cv::Vec3d p3;
            cv::Vec3d col;
            double alpha;
            shapes::ReflectionType ref;
            cv::Vec3d phongCoeff;
            iss >> p1.val[0] >> p1.val[1] >> p1.val[2] >> p2.val[0] >> p2.val[1] >> p2.val[2];
            iss >> p3.val[0] >> p3.val[1] >> p3.val[2] >> col.val[0] >> col.val[1] >> col.val[2];
            iss >> alpha >> word >> phongCoeff(0) >> phongCoeff(1) >> phongCoeff(2);
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            triangles_.emplace_back(p1, p2, p3, col, alpha, ref, phongCoeff);
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
            cv::Vec3d phongCoeff;
            iss >> c(0) >> c(1) >> c(2) >> r >> h >> updir(0) >> updir(1) >> updir(2);
            iss >> col(0) >> col(1) >> col(2) >> alpha >> word >> phongCoeff(0) >> phongCoeff(1) >> phongCoeff(2);
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            cylinders_.emplace_back(c, r, h, updir, col, alpha, ref, phongCoeff);
        }
        else if (word == "Asset")
        {
            std::string file;
            cv::Vec3d pos;
            shapes::ReflectionType ref;
            double alpha;
            cv::Vec3d phongCoeff;
            bool triangulate;
            iss >> file >> pos(0) >> pos(1) >> pos(2) >> alpha >> word >> phongCoeff(0) >> phongCoeff(1) >> phongCoeff(2)
                >> triangulate;
            if (word == "SPECULAR")
                ref = shapes::ReflectionType::SPECULAR;
            else
                ref = shapes::ReflectionType::DIFFUSED;
            import3Dasset(file, ref, phongCoeff, alpha, pos, triangulate);
        }
        else if (word == "#")
            continue;
    }
    file.close();
}


void Scene::import3Dasset(const std::string& file, shapes::ReflectionType reflectionType,
                          const cv::Vec3d& phongCoeff, double alpha, const cv::Vec3d& position, bool triangulate)
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
            triangles.emplace_back(vert[0], vert[1], vert[2], cv::Vec3d(230, 220, 230), alpha,
                                   reflectionType, phongCoeff);

        }
        shapes::Mesh mesh(std::move(triangles), cv::Vec3d{0, 0, 0, });
        mesh.rotate(0, 0, M_PI / 2., mesh.center_);
        mesh.translate(position);
        meshes_.push_back(std::move(mesh));
    }
    std::cout << file << " was successfully imported" << std::endl;
}
