#pragma once
#include "Common.h"
#include <nlohmann/json_fwd.hpp>

namespace qColor
{
void from_json(const nlohmann::json &j, qColor::LinearColor &c);
}