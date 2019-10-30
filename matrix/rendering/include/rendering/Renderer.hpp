#pragma once

#include "Defs.hpp"
#include "led_matrix/threaded-canvas-manipulator.h"
#include <memory>

namespace matrix
{
namespace rendering
{
class Renderer : public rgb_matrix::ThreadedCanvasManipulator
{
private:
    std::shared_ptr<const FrameBufferT> theFrameBuffer;

public:
    Renderer(rgb_matrix::Canvas *aCanvas,
             std::shared_ptr<const FrameBufferT> aFrameBuffer);
    void Run() override;
};
} // namespace rendering
} // namespace matrix
