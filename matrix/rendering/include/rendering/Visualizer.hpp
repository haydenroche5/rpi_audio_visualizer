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
    static constexpr float SMOOTHING_FACTOR{0.6};
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
    std::bitset<NUM_BARS> theBarsAnimating{};

    bool theEnableProfiling;
    std::chrono::microseconds theWortCaseDuration{
        std::chrono::microseconds::min()};

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

        theCurrentBarPositions.fill(NUM_ROWS - 1);
        theNextFrameBuffer = theMatrix->CreateFrameCanvas();
    }

    ~Visualizer()
    {
        if (theEnableProfiling)
        {
            std::cout << "Worst case animation duration: "
                      << theWortCaseDuration.count() << "[us]" << std::endl;
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

        theNextPositions = theUpdateQueue.front();
        theUpdateQueue.pop();

        smoothNextPositions();

        theBarsAnimating.set();

        while (theBarsAnimating.any())
        {
            std::chrono::steady_clock::time_point myStartTime{};
            if (theEnableProfiling)
            {
                myStartTime = std::chrono::steady_clock::now();
            }

            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                int myDistance{static_cast<int>(theNextPositions[i]) -
                               static_cast<int>(theCurrentBarPositions[i])};

                if (myDistance == 0)
                {
                    theBarsAnimating[i] = false;
                    continue;
                }

                auto myDelta{std::min(std::abs(myDistance), ANIMATION_SPEED)};
                auto myXStart{i * BAR_WIDTH};
                auto myXEnd{i * BAR_WIDTH + BAR_WIDTH};

                // Bar is moving up
                if (myDistance < 0)
                {
                    auto myYStart{theCurrentBarPositions[i] - myDelta};
                    auto myYEnd{theCurrentBarPositions[i]};

                    for (size_t y{myYStart}; y < myYEnd; ++y)
                    {
                        for (size_t x{myXStart}; x < myXEnd; ++x)
                        {
                            theNextFrameBuffer->SetPixel(
                                x, y, ROW_COLORS[y].getRed(),
                                ROW_COLORS[y].getGreen(),
                                ROW_COLORS[y].getBlue());
                        }
                    }

                    theCurrentBarPositions[i] -= myDelta;
                }
                // Bar is moving down
                else
                {
                    auto myYStart{theCurrentBarPositions[i]};
                    auto myYEnd{theCurrentBarPositions[i] + myDelta};

                    for (size_t y{myYStart}; y < myYEnd; ++y)
                    {
                        for (size_t x{myXStart}; x < myXEnd; ++x)
                        {
                            theNextFrameBuffer->SetPixel(x, y, 0, 0, 0);
                        }
                    }

                    theCurrentBarPositions[i] += myDelta;
                }
            }

            if (theEnableProfiling)
            {
                auto myEndTime{std::chrono::steady_clock::now()};
                auto myDuration{
                    std::chrono::duration_cast<std::chrono::microseconds>(
                        myEndTime - myStartTime)};

                std::cout << "Animation duration: " << myDuration.count()
                          << "[us]" << std::endl;

                if (myDuration > theWortCaseDuration)
                {
                    theWortCaseDuration = myDuration;
                }
            }

            theMatrix->SwapOnVSync(theNextFrameBuffer);
        }
    }
};
} // namespace rendering
} // namespace matrix
