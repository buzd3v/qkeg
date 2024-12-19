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
inline constexpr int   TYPE_LIGHT_DIRECTIONAL    = 0;
inline constexpr int   TYPE_LIGHT_POINT          = 1;
inline constexpr int   TYPE_LIGHT_SPOT           = 2;
inline constexpr float DEFAULT_POINT_LIGHT_RANGE = 25.f;
inline constexpr float DEFAULT_SPOT_LIGHT_RANGE  = 64.f;

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

    uint32_t toType()
    {
        switch (type)
        {
        case (LightType::Directional): {
            return qConstant::TYPE_LIGHT_DIRECTIONAL;
        }
        case LightType::Point: {
            return qConstant::TYPE_LIGHT_POINT;
        }
        case LightType::Spot: {
            return qConstant::TYPE_LIGHT_SPOT;
        }
        default:
            return -1;
        }
        return -1;
    }
};

// matching light.glsl
struct LightProps
{
    glm::vec3     position;
    uint32_t      type;
    glm::vec3     direction;
    float         range;
    qColor::Color color;
    float         intensity;
    glm::vec2     scaleOffset;
    ImageId       shadowMap;
    float         unused;
};