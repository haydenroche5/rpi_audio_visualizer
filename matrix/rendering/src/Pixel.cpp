#include "matrix/include/rendering/Pixel.hpp"

namespace matrix
{
namespace rendering
{
Pixel::Pixel(CoordinateT aX, CoordinateT aY, Color aColor)
    : theX{aX}, theY{aY}, theColor{aColor}
{
}

CoordinateT Pixel::getX() const { return theX; }

CoordinateT Pixel::getY() const { return theY; }

Color Pixel::getColor() const { return theColor; }

void Pixel::setX(CoordinateT aX) { theX = aX; }

void Pixel::setY(CoordinateT aY) { theY = aY; }

void Pixel::setColor(Color aColor) { theColor = aColor; }
} // namespace rendering
} // namespace matrix
