#pragma once

#include "Defs.hpp"
#include "led_matrix/led-matrix.h"
#include <boost/thread.hpp>
#include <memory>
#include <stdexcept>

namespace matrix
{
namespace rendering
{
template <size_t NUM_FRAME_BUFFERS> class Renderer
{
private:
    std::unique_ptr<rgb_matrix::RGBMatrix> theMatrix;
    std::array<FrameBufferT *, NUM_FRAME_BUFFERS> theNextFrameBuffers;
    size_t theNextFrameBufferIdx{0};
    FrameBufferT *theSpecialFrame{nullptr};

public:
    Renderer(rgb_matrix::RGBMatrix::Options aMatrixOptions,
             rgb_matrix::RuntimeOptions aRuntimeOptions)
        : theMatrix{rgb_matrix::CreateMatrixFromOptions(aMatrixOptions,
                                                        aRuntimeOptions)},
          theNextFrameBuffers{}
    {
        if (theMatrix == nullptr)
        {
            throw std::runtime_error(
                "Error constructing rgb_matrix::RGBMatrix.");
        }

        for (size_t i{0}; i < NUM_FRAME_BUFFERS; ++i)
        {
            theNextFrameBuffers[i] = theMatrix->CreateFrameCanvas();
        }

        theSpecialFrame = theMatrix->CreateFrameCanvas();
    }

    std::array<FrameBufferT *, NUM_FRAME_BUFFERS> getNextFrameBuffers()
    {
        return theNextFrameBuffers;
    }

    void renderNextFrame()
    {
        static bool myCalled{false};

        if (!myCalled)
        {
            std::cout << "i got called" << std::endl;
            myCalled = true;
            theSpecialFrame->SetPixel(0, 5, 0, 255, 255);
            theMatrix->SwapOnVSync(theSpecialFrame);
        }
        // theMatrix->SwapOnVSync(theNextFrameBuffers[theNextFrameBufferIdx]);
        // ++theNextFrameBufferIdx;
        // if (theNextFrameBufferIdx == NUM_FRAME_BUFFERS)
        // {
        //     theNextFrameBufferIdx = 0;
        // }
    }
}; // namespace rendering
} // namespace rendering
} // namespace matrix
