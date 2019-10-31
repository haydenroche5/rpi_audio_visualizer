#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <memory>

namespace matrix
{
namespace rendering
{
class GradientCycler
{
private:
    static constexpr size_t NUM_COLORS{4};
    static constexpr size_t COLOR_COL_WIDTH{NUM_COLS / NUM_COLORS};
    static constexpr size_t NUM_GRADIENTS{2};
    using GradientT = std::array<Color, NUM_COLORS>;
    static constexpr std::array<GradientT, NUM_GRADIENTS> GRADIENTS{
        createGradient<0, 0, 0, 0, 255, 0, NUM_COLORS>(),
        createGradient<255, 0, 0, 0, 255, 0, NUM_COLORS>()};

    FrameBufferT *theNextFrameBuffer;
    size_t theNextGradientIdx;

public:
    GradientCycler();
    GradientCycler(FrameBufferT *aFrameBuffer);
    void draw();
};
} // namespace rendering
} // namespace matrix