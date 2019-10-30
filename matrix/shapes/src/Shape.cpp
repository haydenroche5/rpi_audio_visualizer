#include "matrix/include/shapes/Shape.hpp"

#include <algorithm>

namespace matrix
{
namespace shapes
{
Shape::Shape(const std::vector<rendering::Pixel> &aPixels) : thePixels{aPixels}
{
    std::sort(thePixels.begin(), thePixels.end());
}

const Shape::PixelsT &Shape::getPixels() const { return thePixels; }
} // namespace shapes
} // namespace matrix
