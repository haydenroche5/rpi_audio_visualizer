#include "Renderer.hpp"
#include <iostream>
#include <unistd.h>

namespace matrix
{
namespace rendering
{
Renderer::Renderer(rgb_matrix::Canvas *aCanvas,
                   FrameBufferManager &aFrameBufferManager)
    : rgb_matrix::ThreadedCanvasManipulator(aCanvas), theFrameBufferManager{
                                                          aFrameBufferManager}
{
}

void Renderer::Run()
{
    while (running())
    {
        auto myFrameBuffer{theFrameBufferManager.getCurrentFrameBuffer()};
        // std::cout << "myFrameBuffer: 0x" << std::hex << myFrameBuffer.get()
        //           << std::dec << std::endl;
        for (size_t i{0}; i < NUM_ROWS; ++i)
        {
            for (size_t j{0}; j < NUM_COLS; ++j)
            {
                canvas()->SetPixel(i, j, (*myFrameBuffer)[i][j].getRed(),
                                   (*myFrameBuffer)[i][j].getGreen(),
                                   (*myFrameBuffer)[i][j].getBlue());
            }
        }
        theFrameBufferManager.drawNextGradient();
        theFrameBufferManager.swapBuffers();
        sleep(1);
    }
}
} // namespace rendering
} // namespace matrix
