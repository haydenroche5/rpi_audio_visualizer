#pragma once

#include "dependencies/led_matrix/include/threaded-canvas-manipulator.h"

#include "matrix/include/shapes/Shape.hpp"

#include <memory>
#include <vector>

namespace matrix
{
namespace rendering
{
class Scene : public rgb_matrix::ThreadedCanvasManipulator
{
public:
    using CanvasT = rgb_matrix::Canvas;

private:
    std::vector<shapes::Shape> theShapes{};

public:
    Scene(CanvasT *aCanvas);
    void addShape(const shapes::Shape &aShape);
    void addShape(const shapes::Shape::PixelsT &aPixels);
    void Run() override;
};
} // namespace rendering
} // namespace matrix
