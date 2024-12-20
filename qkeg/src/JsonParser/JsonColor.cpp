#include "JsonParser/JsonColor.h"
#include <nlohmann/json.hpp>
namespace qColor
{
void from_json(const nlohmann::json &j, qColor::LinearColor &c)
{
    if (j.is_array())
    {
        if (j.size() == 3)
        {
            c = qColor::LinearColor{j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), 1.f};
        }
        else
        {
            assert(j.size() == 4);
            c = qColor::LinearColor{j[0].get<float>(), j[1].get<float>(), j[2].get<float>(), j[3].get<float>()};
        }
    }
    else
    {
        // special case for RGB colors e.g. "color": { "rgb": [255, 80, 10] }
        const auto &rgb = j.at("rgb");
        if (rgb.size() == 3)
        {
            const auto rgbColor = qColor::RGBA8_Int{
                rgb[0].get<std::uint8_t>(), rgb[1].get<std::uint8_t>(), rgb[2].get<std::uint8_t>(), 255};
            c = qColor::LinearColor().fromRGBA(rgbColor);
        }
        else
        {
            assert(rgb.size() == 4);
            const auto rgbColor = qColor::RGBA8_Int{rgb[0].get<std::uint8_t>(),
                                                    rgb[1].get<std::uint8_t>(),
                                                    rgb[2].get<std::uint8_t>(),
                                                    rgb[3].get<std::uint8_t>()};

            c = qColor::LinearColor().fromRGBA(rgbColor);
        }
    }
}
} // namespace qColor