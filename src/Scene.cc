#include "Scene.hh"

Scene::Scene(std::string file_name)
{
    load_scene(file_name);
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
            else if (word.compare("DIFFUSED") == 0)
                ref = shapes::ReflectionType::DIFFUSED;
            spheres_.push_back(new shapes::Sphere(c, r, col, alpha, ref, cv::Mat(t)));
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
            else if (word.compare("DIFFUSED") == 0)
                ref = shapes::ReflectionType::DIFFUSED;
            triangles_.push_back(new shapes::Triangle(p1, p2, p3, col, alpha, ref, cv::Mat(t)));
        }
    }
    file.close();
}