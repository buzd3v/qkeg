#pragma once

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
    Transform   transform;

    SceneNode                              *parent;
    std::vector<std::unique_ptr<SceneNode>> children;
};

struct Scene
{
    std::filesystem::path path;

    std::vector<std::unique_ptr<SceneNode>> nodes; // list of root node
    std::vector<Skeleton>                   skeletons;
    std::vector<Light>                      lights;

    // for meshes render
    std::vector<ModelPool>                        models;
    std::unordered_map<qTypes::MeshId, MeshProps> meshProps;
    // skeletal animations
    //  std::vector<ModelPool>                  models;
};