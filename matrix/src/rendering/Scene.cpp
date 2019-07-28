#include "matrix/include/rendering/Scene.hpp"
#include "matrix/include/rendering/Pixel.hpp"

#include <sstream>
#include <string>

namespace matrix
{
namespace rendering
{
Scene::Scene(Scene::CanvasT *aCanvas)
    : rgb_matrix::ThreadedCanvasManipulator(aCanvas)
{
}

void Scene::addShape(const shapes::Shape &aShape)
{
    theShapes.push_back(aShape);
}

void Scene::addShape(const shapes::Shape::PixelsT &aPixels)
{
    theShapes.emplace_back(aPixels);
}

void Scene::Run()
{
    while (running())
    {
        std::vector<std::vector<bool>> myOccupiedPixels(
            canvas()->height(), std::vector<bool>(canvas()->width(), false));
        for (const auto &myShape : theShapes)
        {
            for (auto myPixel : myShape.getPixels())
            {
                auto myOccupied{
                    myOccupiedPixels[myPixel.getX()][myPixel.getY()]};

                if (myOccupied)
                {
                    std::stringstream myErrorStringStream;
                    myErrorStringStream
                        << "Scene::draw: A shape attempted to draw a pixel in "
                           "an occupied space. X = "
                        << myPixel.getX() << ", Y = " << myPixel.getY() << ".";
                    throw std::runtime_error(myErrorStringStream.str());
                }

                canvas()->SetPixel(myPixel.getX(), myPixel.getY(),
                                   myPixel.getColor().getRed(),
                                   myPixel.getColor().getGreen(),
                                   myPixel.getColor().getBlue());
                myOccupiedPixels[myPixel.getX()][myPixel.getY()] = true;
            }
        }
    }
}
} // namespace rendering
} // namespace matrix
