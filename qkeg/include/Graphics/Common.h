#pragma once

#include <cstdint>

namespace graphics
{

inline constexpr std::uint32_t FRAMES_IN_FLIGHT = 2;
} // namespace graphics

namespace qColor
{
struct RGBA8_Int
{
    std::uint8_t red{0};
    std::uint8_t green{0};
    std::uint8_t blue{0};
    std::uint8_t alpha{255};

    // RGBA8_Int(std::uint8_t)
};

struct LinearColor
{
    float red{0.f};
    float green{0.f};
    float blue{0.f};
    float alpha{0.f};

    LinearColor fromRGBA(RGBA8_Int color)
    {
        return LinearColor{
            .red   = (float)color.red / 255.f,
            .green = (float)color.green / 255.f,
            .blue  = (float)color.blue / 255.f,
            .alpha = (float)color.alpha / 255.f,
        };
    }
};

struct Color
{
    float red{0.f};
    float green{0.f};
    float blue{0.f};
    Color(LinearColor color) : red(color.red), green(color.green), blue(color.blue) {}
};

}; // namespace qColor

namespace qConstant
{
constexpr inline int ANIMATION_FPS = 60;
constexpr inline int ROOT_JOINT_ID = 0;
constexpr inline int MAX_LIGHT     = 1000;

constexpr inline qColor::LinearColor DEFAUT_AMBIENT_COLOR = qColor::LinearColor{0.051f, 0.051f, 0.051f, 1.f};
constexpr inline float               DEFAULT_AMBIENT_INTENSITY{1.0f};

} // namespace qConstant