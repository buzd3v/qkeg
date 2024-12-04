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
    bool                shadow{true};

    void setConeAngle(float innerCone, float outerCone)
    {
        // Reference:
        // https://github.com/KhronosGroup/glTF/blob/main/extensions/2.0/Khronos/KHR_lights_punctual/README.md
        float lightAngleScale  = 1.0f / std::max(0.001f, cos(innerCone) - cos(outerCone));
        float lightAngleOffset = -cos(innerCone) * lightAngleScale;
        this->scaleOffset      = glm::vec2{lightAngleScale, lightAngleOffset};
    }
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