#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <memory>

namespace matrix
{
namespace rendering
{
class FrameBufferManager
{
public:
    FrameBufferManager();
    void drawNextGradient();
    void swapBuffers();
    std::shared_ptr<const FrameBufferT> getCurrentFrameBuffer() const;

private:
    static constexpr size_t NUM_COLORS{4};
    static constexpr size_t COLOR_COL_WIDTH{NUM_COLS / NUM_COLORS};
    static constexpr size_t NUM_GRADIENTS{2};
    using GradientT = std::array<Color, NUM_COLORS>;
    static constexpr std::array<GradientT, NUM_GRADIENTS> GRADIENTS{
        createGradient<0, 0, 0, 0, 255, 0, NUM_COLORS>(),
        createGradient<255, 0, 0, 0, 255, 0, NUM_COLORS>()};

    std::array<FrameBufferT, 2> theFrameBuffers;
    std::shared_ptr<FrameBufferT> theCurrentFrameBuffer;
    std::shared_ptr<FrameBufferT> theNextFrameBuffer;
    size_t theNextGradientIdx;
};
} // namespace rendering
} // namespace matrix
