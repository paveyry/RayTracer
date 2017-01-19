#include <opencv2/core/core.hpp>
#include <assimp/mesh.h>

struct cvaiVec3 : public cv::Vec3d
{
    cvaiVec3(aiVector3D a)
        : cv::Vec3d{a.x, a.y, a.z}
    {}
};
