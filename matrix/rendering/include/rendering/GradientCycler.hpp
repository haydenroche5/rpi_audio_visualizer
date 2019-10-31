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
    static constexpr size_t NUM_COLORS{8};
    static constexpr size_t COLOR_COL_WIDTH{NUM_COLS / NUM_COLORS};
    static constexpr size_t NUM_GRADIENTS{16};
    using GradientT = std::array<Color, NUM_COLORS>;
    static constexpr std::array<GradientT, NUM_GRADIENTS> GRADIENTS{
        createGradient<95, 134, 255, 149, 39, 64, NUM_COLORS>(),
        createGradient<49, 40, 172, 91, 82, 4, NUM_COLORS>(),
        createGradient<105, 46, 208, 226, 51, 9, NUM_COLORS>(),
        createGradient<189, 61, 113, 253, 255, 18, NUM_COLORS>(),
        createGradient<91, 120, 17, 34, 235, 210, NUM_COLORS>(),
        createGradient<23, 40, 112, 147, 193, 102, NUM_COLORS>(),
        createGradient<117, 95, 64, 39, 155, 226, NUM_COLORS>(),
        createGradient<177, 109, 158, 23, 8, 216, NUM_COLORS>(),
        createGradient<112, 202, 197, 13, 111, 188, NUM_COLORS>(),
        createGradient<77, 211, 226, 142, 135, 225, NUM_COLORS>(),
        createGradient<116, 63, 195, 236, 193, 134, NUM_COLORS>(),
        createGradient<76, 12, 114, 66, 133, 254, NUM_COLORS>(),
        createGradient<181, 4, 23, 214, 80, 244, NUM_COLORS>(),
        createGradient<104, 168, 162, 233, 206, 43, NUM_COLORS>(),
        createGradient<214, 250, 58, 93, 22, 127, NUM_COLORS>(),
        createGradient<246, 71, 41, 201, 198, 100, NUM_COLORS>()};

    FrameBufferT *theNextFrameBuffer;
    size_t theNextGradientIdx;

public:
    GradientCycler();
    GradientCycler(FrameBufferT *aFrameBuffer);
    void draw();
};
} // namespace rendering
} // namespace matrix