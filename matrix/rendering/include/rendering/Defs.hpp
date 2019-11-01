#pragma once

#include "Color.hpp"
#include "led_matrix/led-matrix.h"
#include <array>
#include <boost/lockfree/spsc_queue.hpp>

namespace matrix
{
namespace rendering
{
static constexpr size_t NUM_ROWS{64};
static constexpr size_t NUM_COLS{64};

template <size_t NUM_BARS> using FreqBarPositionsT = std::array<int, NUM_BARS>;
static constexpr size_t FREQ_BAR_UPDATE_QUEUE_DEPTH{4};
template <size_t NUM_BARS>
using FreqBarsUpdateQueueT =
    boost::lockfree::spsc_queue<FreqBarPositionsT<NUM_BARS>>;
using FrameBufferT = rgb_matrix::FrameCanvas;
// using AvgOctavePowersT = std::array<double, NUM_OCTAVES>;
} // namespace rendering
} // namespace matrix