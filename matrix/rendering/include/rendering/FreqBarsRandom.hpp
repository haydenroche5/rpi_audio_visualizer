#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <boost/thread.hpp>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace matrix
{
namespace rendering
{
template <size_t NUM_BARS, size_t NUM_COLORS_PER_GRADIENT, int ANIMATION_SPEED>
class FreqBarsRandom
{
    static constexpr size_t NUM_GRADIENTS{4};
    static constexpr size_t TOTAL_COLORS{NUM_COLORS_PER_GRADIENT *
                                         NUM_GRADIENTS};

    static_assert(NUM_COLS % NUM_BARS == 0,
                  "FreqBarsRandom instantiated with a NUM_BARS that doesn't "
                  "evenly divide NUM_COLS.");
    static_assert(NUM_ROWS % TOTAL_COLORS == 0,
                  "FreqBarsRandom instantiated with a TOTAL_COLORS "
                  "that doesn't "
                  "evenly divide NUM_ROWS.");
    static_assert(
        NUM_ROWS % ANIMATION_SPEED == 0,
        "FreqBarsRandom instantiated with a ANIMATION_SPEED that doesn't "
        "evenly divide NUM_ROWS.");

private:
    static constexpr size_t BAR_WIDTH{NUM_COLS / NUM_BARS};
    static constexpr size_t COLOR_HEIGHT{
        NUM_ROWS / (NUM_COLORS_PER_GRADIENT * NUM_GRADIENTS)};
    static constexpr auto ROW_COLORS{
        createRowColors<NUM_GRADIENTS, NUM_COLORS_PER_GRADIENT>()};

    static constexpr size_t MAX_ANIMATION_FRAMES{NUM_ROWS / ANIMATION_SPEED};

    std::unique_ptr<rgb_matrix::RGBMatrix> theMatrix;
    FreqBarPositionsT<NUM_BARS> theCurrentBarPositions{};
    FreqBarPositionsT<NUM_BARS> theNextPositions{};
    FreqBarsUpdateQueueT<NUM_BARS> &theUpdateQueue;
    FrameBufferT *theNextFrameBuffer{nullptr};
    std::array<bool, NUM_BARS> theBarsAnimating{};

public:
    FreqBarsRandom(rgb_matrix::RGBMatrix::Options aMatrixOptions,
                   rgb_matrix::RuntimeOptions aRuntimeOptions,
                   FreqBarsUpdateQueueT<NUM_BARS> &aUpdateQueue)
        : theMatrix{rgb_matrix::CreateMatrixFromOptions(aMatrixOptions,
                                                        aRuntimeOptions)},
          theUpdateQueue{aUpdateQueue}
    {
        if (theMatrix == nullptr)
        {
            throw std::runtime_error(
                "Error constructing rgb_matrix::RGBMatrix.");
        }

        theNextFrameBuffer = theMatrix->CreateFrameCanvas();
    }

    bool animating() const
    {
        for (auto myAnimating : theBarsAnimating)
        {
            if (myAnimating)
            {
                return true;
            }
        }

        return false;
    }

    void animate()
    {
        auto myNumUpdatesAvailable{theUpdateQueue.read_available()};
        if (myNumUpdatesAvailable < 1)
        {
            return;
        }
        std::cout << "myNumUpdatesAvailable = " << myNumUpdatesAvailable
                  << std::endl;

        theNextPositions = theUpdateQueue.front();
        theUpdateQueue.pop();

        // int myMaxDistance{0};
        // std::array<int, NUM_BARS> myDistances{};
        // for (size_t i{0}; i < NUM_BARS; ++i)
        // {
        //     auto myDistance{theNextPositions[i] - theCurrentBarPositions[i]};
        //     myDistances[i] = myDistance;

        //     if (myDistance > myMaxDistance)
        //     {
        //         myMaxDistance = myDistance;
        //     }

        //     if (myDistance == 0)
        //     {
        //         continue;
        //     }

        //     if (myDistance < 0)
        //     {
        //         theCurrentBarPositions[i] -=
        //             std::max(myDistance, ANIMATION_SPEED);
        //     }
        //     else
        //     {
        //         theCurrentBarPositions[i] +=
        //             std::min(myDistance, ANIMATION_SPEED);
        //     }
        // }

        // int myNumFrames{std::ceil(1.0 * myMaxDistance / ANIMATION_SPEED)};
        while (animating())
        {
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                auto myDistance{theNextPositions[i] -
                                theCurrentBarPositions[i]};

                if (myDistance == 0)
                {
                    continue;
                }

                if (myDistance < 0)
                {
                    theCurrentBarPositions[i] -=
                        std::max(myDistance, ANIMATION_SPEED);
                }
                else
                {
                    theCurrentBarPositions[i] +=
                        std::min(myDistance, ANIMATION_SPEED);
                }

                theBarsAnimating[i] =
                    (theCurrentBarPositions[i] != theNextPositions[i]);
            }

            auto myRowColorIdx{0};
            auto myRowColor{ROW_COLORS[myRowColorIdx]};
            for (int y{0}; y < NUM_ROWS; y += COLOR_HEIGHT)
            {
                for (int myYOffset{0}; myYOffset < COLOR_HEIGHT; ++myYOffset)
                {
                    size_t myBarIdx{0};
                    for (int x{0}; x < NUM_COLS; x += BAR_WIDTH)
                    {
                        if ((y + myYOffset) >= theCurrentBarPositions[myBarIdx])
                        {
                            for (int myXOffset{0}; myXOffset < BAR_WIDTH;
                                 ++myXOffset)
                            {
                                theNextFrameBuffer->SetPixel(
                                    x + myXOffset, y + myYOffset, 0, 0, 0);
                            }
                        }
                        else
                        {
                            for (int myXOffset{0}; myXOffset < BAR_WIDTH;
                                 ++myXOffset)
                            {
                                theNextFrameBuffer->SetPixel(
                                    x + myXOffset, y + myYOffset,
                                    myRowColor.getRed(), myRowColor.getGreen(),
                                    myRowColor.getBlue());
                            }
                        }

                        ++myBarIdx;
                    }
                }
                ++myRowColorIdx;
                myRowColor = ROW_COLORS[myRowColorIdx];
            }
            theNextFrameBuffer = theMatrix->SwapOnVSync(theNextFrameBuffer);
        }
    }
};
} // namespace rendering
} // namespace matrix