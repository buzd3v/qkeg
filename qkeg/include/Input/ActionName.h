#pragma once

#include <cstddef>
#include <limits>

using ActionName                             = std::size_t;
static constexpr ActionName ACTION_NONE_HASH = std::numeric_limits<ActionName>::max();
