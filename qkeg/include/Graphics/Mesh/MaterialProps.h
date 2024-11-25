#pragma once

#include "Common.h"
#include "Vulkan/GPUImage.h"
#include <glm/vec4.hpp>
#include <limits>

namespace qTypes
{
using MaterialId                   = std::uint32_t;
static MaterialId NULL_MATERIAL_ID = std::numeric_limits<MaterialId>::max();
} // namespace qTypes

struct MaterialProps
{
    qColor::LinearColor baseColor;
    glm::vec4           metalRoughnessEmissive;
    std::uint32_t       diffuseTex;
    std::uint32_t       normalTex;
    std::uint32_t       metallicRoughnessTex;
    std::uint32_t       emissiveTex;
};
