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
static constexpr size_t NUM_BARS{28};
static constexpr size_t FREQ_BAR_UPDATE_QUEUE_DEPTH{1};

using VisualizerBarPositionsT = std::array<size_t, NUM_BARS>;
using VisualizerUpdateQueueT =
    boost::lockfree::spsc_queue<VisualizerBarPositionsT>;
using FrameBufferT = rgb_matrix::FrameCanvas;
using FrameBufferQueueT = boost::lockfree::spsc_queue<FrameBufferT>;
} // namespace rendering
} // namespace matrix