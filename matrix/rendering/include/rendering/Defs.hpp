#pragma once

#include "Color.hpp"
#include <array>

namespace matrix
{
namespace rendering
{
static constexpr size_t NUM_ROWS{64};
static constexpr size_t NUM_COLS{64};

using FrameBufferT = std::array<std::array<Color, NUM_COLS>, NUM_ROWS>;
// using AvgOctavePowersT = std::array<double, NUM_OCTAVES>;
} // namespace rendering
} // namespace matrix