#include "FrameBufferManager.hpp"

namespace matrix
{
namespace rendering
{
FrameBufferManager::FrameBufferManager()
    : theFrameBuffers{},
      theCurrentFrameBuffer{&theFrameBuffers[0]}, // TODO: Can these initial
                                                  // values be put at the
                                                  // declaration?
      theNextFrameBuffer{&theFrameBuffers[1]}, theNextGradientIdx{0}
{
}

void FrameBufferManager::drawNextGradient()
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
            (*theNextFrameBuffer)[i][j] = myNextGradient[myColorIdx];
        }
    }
    ++theNextGradientIdx;
}

void FrameBufferManager::swapBuffers()
{
    // auto myTempFrameBuffer{theCurrentFrameBuffer};
    // theCurrentFrameBuffer = theNextFrameBuffer;
    // theNextFrameBuffer = myTempFrameBuffer;
    theCurrentFrameBuffer.swap(theNextFrameBuffer);
}

std::shared_ptr<const FrameBufferT>
FrameBufferManager::getCurrentFrameBuffer() const
{
    return theCurrentFrameBuffer;
}
} // namespace rendering
} // namespace matrix