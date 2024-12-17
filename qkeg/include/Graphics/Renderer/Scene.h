#pragma once

#include "Animation.h"
#include "Light.h"
#include "Math/Transform.h"
#include "Mesh/MaterialPool.h"
#include "Mesh/MeshPool.h"
#include "Skeleton.h"
struct ModelPool
{
    std::vector<qTypes::MeshId>     primitiveMeshes;
    std::vector<qTypes::MaterialId> primitiveMaterials;
};

struct SceneNode
{
    std::string name{};
    int         meshIndex{-1};
    int         materialIndex{-1};
    int         lightIndex{-1};
    int         cameraIndex{-1};
    int         skeletonIndex{-1};
    Transform   transform;

    SceneNode                              *parent;
    std::vector<std::unique_ptr<SceneNode>> children;
};

struct Scene
{
    std::filesystem::path path;

    std::vector<std::unique_ptr<SceneNode>> nodes; // list of root node
    std::vector<Light>                      lights;

    std::vector<ModelPool>                        models;
    std::unordered_map<qTypes::MeshId, MeshProps> meshProps;
    std::vector<Skeleton>                         skeletons;
    std::unordered_map<std::string, Animation>    animations;
    // skeletal animations
};