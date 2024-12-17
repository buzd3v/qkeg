#pragma once

#include "Mesh/GPUMaterial.h"
#include "Mesh/GPUMesh.h"

struct MeshDrawProps
{
    glm::mat4          transform;
    qTypes::MaterialId materialID{qTypes::NULL_MATERIAL_ID};
    qTypes::MeshId     meshID{qTypes::NULL_MESH_ID};
    bool               shadow{true};
    // skinning mesh
    const SkinningMesh *skinningMesh{nullptr};
    std::uint32_t       jointMatrixIndex;
};
