#pragma once
#include "Common.h"
#include "Vulkan/ImagePool.h"
#include <glm/glm.hpp>

enum class LightType
{
    None,
    Directional,
    Point,
    Spot,
};

namespace qConstant
{
inline constexpr int TYPE_LIGHT_DIRECTIONAL = 0;
inline constexpr int TYPE_LIGHT_POINT       = 1;
inline constexpr int TYPE_LIGHT_SPOT        = 2;

}; // namespace qConstant

struct Light
{
    std::string         name{};
    LightType           type{LightType::None};
    qColor::LinearColor color;
    float               intensity{0.f};
    float               range{0.f};
    glm::vec2           scaleOffset; // for spot light;
};

// matching light.glsl
struct LightProps
{
    glm::vec3           position;
    uint32_t            type;
    glm::vec3           direction;
    float               range;
    qColor::LinearColor color;
    float               intensity;
    glm::vec2           scaleOffset;
    ImageId             shadowMap;
    float               unused;
};