#pragma once

#include "MeshProps.h"
#include "Vulkan/GPUBuffer.h"
#include <limits>
namespace qTypes
{
using MeshId               = std::uint32_t;
static MeshId NULL_MESH_ID = std::numeric_limits<MeshId>::max();
}; // namespace qTypes

struct GPUMesh
{
    GPUBuffer vertexBuffer;
    GPUBuffer indexBuffer;

    uint32_t numIndices  = 0;
    uint32_t numVertices = 0;

    bool hasSkeleton{false};
    // skinned meshes only
    GPUBuffer skinningDataBuffer;
};

struct SkinningMesh
{
    GPUBuffer skinnedVertexBuffer;
};