#include "GradientCycler.hpp"

namespace matrix
{
namespace rendering
{
GradientCycler::GradientCycler()
    : theNextFrameBuffer{nullptr}, theNextGradientIdx{0}
{
}

GradientCycler::GradientCycler(FrameBufferT *aFrameBuffer)
    : theNextFrameBuffer{aFrameBuffer}, theNextGradientIdx{0}
{
}

void GradientCycler::draw()
{
    if (theNextGradientIdx == NUM_GRADIENTS)
    {
        theNextGradientIdx = 0;
    }
    auto myNextGradient{GRADIENTS[theNextGradientIdx]};

    for (size_t i{0}; i < NUM_ROWS; ++i)
    {
        int myColorIdx{-1};
        for (size_t j{0}; j < NUM_COLS; ++j)
        {
            if (j % COLOR_COL_WIDTH == 0)
            {
                ++myColorIdx;
            }
            theNextFrameBuffer->SetPixel(i, j,
                                         myNextGradient[myColorIdx].getRed(),
                                         myNextGradient[myColorIdx].getGreen(),
                                         myNextGradient[myColorIdx].getBlue());
        }
    }
    ++theNextGradientIdx;
}
} // namespace rendering
} // namespace matrix