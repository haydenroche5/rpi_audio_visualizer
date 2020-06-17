#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <boost/thread.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <stdexcept>

template <size_t NUM_BARS, size_t NUM_COLS> constexpr size_t computeBarWidth()
{
    if (NUM_COLS % NUM_BARS == 0)
    {
        return NUM_COLS / NUM_BARS;
    }
    else
    {
        size_t myNumBorderCols{NUM_COLS % NUM_BARS};
        return (NUM_COLS - myNumBorderCols) / NUM_BARS;
    }
}

namespace matrix
{
namespace rendering
{
// TODO: Border on each side of visualizer
template <size_t NUM_BARS> class Visualizer
{
    static constexpr size_t NUM_GRADIENTS{4};
    static constexpr size_t NUM_COLORS_PER_GRADIENT{16};
    static constexpr float SMOOTHING_FACTOR{0.6};

    static_assert((NUM_COLS % NUM_BARS == 0) ||
                      ((NUM_COLS % NUM_BARS) & ((NUM_COLS % NUM_BARS) - 1)) ==
                          0,
                  "Visualizer instantiated with a NUM_BARS that doesn't "
                  "evenly divide NUM_COLS.");

private:
    static constexpr size_t BAR_WIDTH{computeBarWidth<NUM_BARS, NUM_COLS>()};
    static constexpr size_t X_BORDER{(NUM_COLS - NUM_BARS * BAR_WIDTH) / 2};
    static constexpr auto ROW_COLORS{
        createRowColors<NUM_GRADIENTS, NUM_COLORS_PER_GRADIENT>()};

    std::unique_ptr<rgb_matrix::RGBMatrix> theMatrix;
    VisualizerBarPositionsT theCurrentBarPositions{};
    VisualizerBarPositionsT theNextPositions{};
    VisualizerUpdateQueueT &theUpdateQueue;
    FrameBufferT *theNextFrameBuffer{nullptr};
    std::bitset<NUM_BARS> theBarsAnimating{};
    int theAnimationSpeed;
    bool theEnableProfiling;
    std::chrono::microseconds theWortCaseDuration{
        std::chrono::microseconds::min()};

public:
    Visualizer(rgb_matrix::RGBMatrix::Options aMatrixOptions,
               rgb_matrix::RuntimeOptions aRuntimeOptions,
               VisualizerUpdateQueueT &aUpdateQueue, int aAnimationSpeed,
               bool aEnableProfiling = false)
        : theMatrix{rgb_matrix::CreateMatrixFromOptions(aMatrixOptions,
                                                        aRuntimeOptions)},
          theUpdateQueue{aUpdateQueue}, theAnimationSpeed{aAnimationSpeed},
          theEnableProfiling{aEnableProfiling}
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

                auto myDelta{std::min(std::abs(myDistance), theAnimationSpeed)};
                auto myXStart{i * BAR_WIDTH + X_BORDER};
                auto myXEnd{i * BAR_WIDTH + BAR_WIDTH + X_BORDER};

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
