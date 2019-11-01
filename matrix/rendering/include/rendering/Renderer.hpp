#pragma once

#include "Defs.hpp"
#include "led_matrix/led-matrix.h"
#include <memory>
#include <stdexcept>

namespace matrix
{
namespace rendering
{
template <typename DrawerT> class Renderer
{
private:
    std::unique_ptr<rgb_matrix::RGBMatrix> theMatrix;
    FrameBufferT *theNextFrameBuffer;
    DrawerT theDrawer;

public:
    template <typename... DrawerArgsT>
    Renderer(rgb_matrix::RGBMatrix::Options aMatrixOptions,
             rgb_matrix::RuntimeOptions aRuntimeOptions,
             DrawerArgsT &&... aDrawerArgs)
        : theMatrix{rgb_matrix::CreateMatrixFromOptions(aMatrixOptions,
                                                        aRuntimeOptions)},
          theNextFrameBuffer{nullptr}, theDrawer{
                                           DrawerT(std::forward<DrawerArgsT>(
                                               aDrawerArgs)...)}
    {
        if (theMatrix == nullptr)
        {
            throw std::runtime_error(
                "Error constructing rgb_matrix::RGBMatrix.");
        }

        theNextFrameBuffer = theMatrix->CreateFrameCanvas();
        theDrawer.setNextFrameBuffer(theNextFrameBuffer);
    }

    void renderNextFrame()
    {
        theDrawer.draw();
        theMatrix->SwapOnVSync(theNextFrameBuffer);
    }
};
} // namespace rendering
} // namespace matrix
