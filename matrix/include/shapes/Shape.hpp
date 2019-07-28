#pragma once

#include "matrix/include/rendering/Pixel.hpp"

#include <vector>

namespace matrix
{
namespace shapes
{
class Shape
{
public:
    using PixelsT = std::vector<rendering::Pixel>;

private:
    PixelsT thePixels;

public:
    Shape(const std::vector<rendering::Pixel> &aPixels);
    const PixelsT &getPixels() const;
};
} // namespace shapes
} // namespace matrix
