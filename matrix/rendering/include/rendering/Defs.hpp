#pragma once

#include "Color.hpp"
#include "led_matrix/led-matrix.h"
#include <array>

namespace matrix
{
namespace rendering
{
static constexpr size_t NUM_ROWS{64};
static constexpr size_t NUM_COLS{64};

using FrameBufferT = rgb_matrix::FrameCanvas;
// using AvgOctavePowersT = std::array<double, NUM_OCTAVES>;
} // namespace rendering
} // namespace matrix