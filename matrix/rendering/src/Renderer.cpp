#include "Renderer.hpp"

namespace matrix
{
namespace rendering
{
Renderer::Renderer(rgb_matrix::Canvas *aCanvas,
                   std::shared_ptr<const FrameBufferT> aFrameBuffer)
    : rgb_matrix::ThreadedCanvasManipulator(aCanvas), theFrameBuffer{
                                                          aFrameBuffer}
{
}

void Renderer::Run()
{
    while (running())
    {
        for (size_t i{0}; i < NUM_ROWS; ++i)
        {
            for (size_t j{0}; j < NUM_COLS; ++j)
            {
                canvas()->SetPixel(i, j, (*theFrameBuffer)[i][j].getRed(),
                                   (*theFrameBuffer)[i][j].getGreen(),
                                   (*theFrameBuffer)[i][j].getBlue());
            }
        }
    }
}
} // namespace rendering
} // namespace matrix
