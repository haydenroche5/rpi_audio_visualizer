#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <boost/thread.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace matrix
{
namespace rendering
{
template <size_t NUM_BARS, size_t NUM_COLORS_PER_GRADIENT, int ANIMATION_SPEED>
class Visualizer
{
    static constexpr float SMOOTHING_FACTOR{0.2};
    static constexpr size_t NUM_GRADIENTS{4};
    static constexpr size_t TOTAL_COLORS{NUM_COLORS_PER_GRADIENT *
                                         NUM_GRADIENTS};

    static_assert(NUM_COLS % NUM_BARS == 0,
                  "Visualizer instantiated with a NUM_BARS that doesn't "
                  "evenly divide NUM_COLS.");
    static_assert(NUM_ROWS % TOTAL_COLORS == 0,
                  "Visualizer instantiated with a TOTAL_COLORS "
                  "that doesn't "
                  "evenly divide NUM_ROWS.");
    static_assert(NUM_ROWS % ANIMATION_SPEED == 0,
                  "Visualizer instantiated with a ANIMATION_SPEED that doesn't "
                  "evenly divide NUM_ROWS.");

private:
    static constexpr size_t BAR_WIDTH{NUM_COLS / NUM_BARS};
    static constexpr size_t COLOR_HEIGHT{
        NUM_ROWS / (NUM_COLORS_PER_GRADIENT * NUM_GRADIENTS)};
    static constexpr auto ROW_COLORS{
        createRowColors<NUM_GRADIENTS, NUM_COLORS_PER_GRADIENT>()};

    static constexpr size_t MAX_ANIMATION_FRAMES{NUM_ROWS / ANIMATION_SPEED};

    std::unique_ptr<rgb_matrix::RGBMatrix> theMatrix;
    VisualizerBarPositionsT<NUM_BARS> theCurrentBarPositions{};
    VisualizerBarPositionsT<NUM_BARS> theNextPositions{};
    VisualizerUpdateQueueT<NUM_BARS> &theUpdateQueue;
    FrameBufferT *theNextFrameBuffer{nullptr};
    std::array<bool, NUM_BARS> theBarsAnimating{};

    bool theEnableProfiling;
    std::chrono::milliseconds theWortCaseDuration{
        std::chrono::milliseconds::min()};

public:
    Visualizer(rgb_matrix::RGBMatrix::Options aMatrixOptions,
               rgb_matrix::RuntimeOptions aRuntimeOptions,
               VisualizerUpdateQueueT<NUM_BARS> &aUpdateQueue,
               bool aEnableProfiling = false)
        : theMatrix{rgb_matrix::CreateMatrixFromOptions(aMatrixOptions,
                                                        aRuntimeOptions)},
          theUpdateQueue{aUpdateQueue}, theEnableProfiling{aEnableProfiling}
    {
        if (theMatrix == nullptr)
        {
            throw std::runtime_error(
                "Error constructing rgb_matrix::RGBMatrix.");
        }

        theNextFrameBuffer = theMatrix->CreateFrameCanvas();
    }

    ~Visualizer()
    {
        if (theEnableProfiling)
        {
            std::cout << "Worst case animation duration: "
                      << theWortCaseDuration.count() << "[ms]" << std::endl;
        }
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

    void updateBarPositions()
    {
        for (size_t i{0}; i < NUM_BARS; ++i)
        {
            int myDistance{theNextPositions[i] - theCurrentBarPositions[i]};
            if (myDistance == 0)
            {
                continue;
            }

            if (myDistance < 0)
            {
                theCurrentBarPositions[i] -=
                    std::min(-myDistance, ANIMATION_SPEED);
            }
            else
            {
                theCurrentBarPositions[i] +=
                    std::min(myDistance, ANIMATION_SPEED);
            }

            theBarsAnimating[i] =
                (theCurrentBarPositions[i] != theNextPositions[i]);
        }
    }

    void smoothNextPositions()
    {
        for (size_t i{0}; i < NUM_BARS; ++i)
        {
            theNextPositions[i] = theCurrentBarPositions[i] * SMOOTHING_FACTOR +
                                  theNextPositions[i] * (1 - SMOOTHING_FACTOR);
        }
    }

    void animate()
    {
        if (theUpdateQueue.read_available() < 1)
        {
            return;
        }

        std::chrono::steady_clock::time_point myStart{};
        if (theEnableProfiling)
        {
            myStart = std::chrono::steady_clock::now();
        }

        theNextPositions = theUpdateQueue.front();
        theUpdateQueue.pop();

        smoothNextPositions();
        updateBarPositions();

        while (animating())
        {
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
                                    x + myXOffset, y + myYOffset,
                                    myRowColor.getRed(), myRowColor.getGreen(),
                                    myRowColor.getBlue());
                            }
                        }
                        else
                        {
                            for (int myXOffset{0}; myXOffset < BAR_WIDTH;
                                 ++myXOffset)
                            {
                                theNextFrameBuffer->SetPixel(
                                    x + myXOffset, y + myYOffset, 0, 0, 0);
                            }
                        }

                        ++myBarIdx;
                    }
                }
                ++myRowColorIdx;
                myRowColor = ROW_COLORS[myRowColorIdx];
            }
            updateBarPositions();
            theNextFrameBuffer = theMatrix->SwapOnVSync(theNextFrameBuffer);
        }

        if (theEnableProfiling)
        {
            auto myEnd{std::chrono::steady_clock::now()};
            auto myDuration{
                std::chrono::duration_cast<std::chrono::milliseconds>(myEnd -
                                                                      myStart)};

            if (myDuration > theWortCaseDuration)
            {
                theWortCaseDuration = myDuration;
            }

            std::cout << "Animation duration: " << myDuration.count() << "[ms]"
                      << std::endl;
        }
    }
};
} // namespace rendering
} // namespace matrix