#pragma once

#include "matrix/include/rendering/Color.hpp"
#include "matrix/include/rendering/Coordinate.hpp"

namespace matrix
{
namespace rendering
{

class Pixel
{
private:
    CoordinateT theX;
    CoordinateT theY;
    Color theColor;

public:
    Pixel(CoordinateT aX, CoordinateT aY, Color aColor);

    CoordinateT getX() const;
    CoordinateT getY() const;
    Color getColor() const;

    void setX(CoordinateT aX);
    void setY(CoordinateT aY);
    void setColor(Color aColor);
};

inline bool operator<(const Pixel &aLhs, const Pixel &aRhs)
{
    if (aLhs.getY() < aRhs.getY())
    {
        return true;
    }
    else if (aLhs.getY() > aRhs.getY())
    {
        return false;
    }
    else
    {
        return aLhs.getX() < aRhs.getX();
    }
}
} // namespace rendering
} // namespace matrix
