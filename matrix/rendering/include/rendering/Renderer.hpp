#pragma once

#include "Defs.hpp"
#include "FrameBufferManager.hpp"
#include "led_matrix/threaded-canvas-manipulator.h"
#include <functional>
#include <memory>

namespace matrix
{
namespace rendering
{
class Renderer : public rgb_matrix::ThreadedCanvasManipulator
{
private:
    // std::function<std::shared_ptr<const FrameBufferT>()>
    // theGetFrameBufferFunc;
    FrameBufferManager &theFrameBufferManager;

public:
    Renderer(rgb_matrix::Canvas *aCanvas,
             FrameBufferManager &aFrameBufferManager);
    void Run() override;
};
} // namespace rendering
} // namespace matrix
