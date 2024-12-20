#pragma once

#include <vector>

#include "Math/Transform.h"
#include "Mesh/GPUMaterial.h"
#include "Mesh/GPUMesh.h"

struct MeshComponent
{
    std::vector<qTypes::MeshId>     meshes;
    std::vector<qTypes::MaterialId> materials;
    std::vector<Transform>          transform;
    bool                            castShadow{true};
};