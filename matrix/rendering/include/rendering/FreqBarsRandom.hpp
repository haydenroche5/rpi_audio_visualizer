#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
#include <iostream>
#include <stdexcept>

namespace matrix
{
namespace rendering
{
// TODO: This guy should have a boost lockfree queue that it drains. An element
// in the queue is one of NUM_BARS updates to the bar positions. Maintain an
// animating flag. In draw, when this flag is false, check the
// update queue for new updates. If there's a new update, set the animating flag
// to true and draw the next frame based on the update. Continue this for each
// draw call until the animation is complete, and set the animating flag back to
// false.
// TODO: Scratch that. Max number of frames per animation is 64. Maintain a
// frame buffer queue of max depth 64.
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

    // constexpr unsigned log2(unsigned aVal)
    // {
    //     return aVal ? (1 + log2(aVal >> 1)) : -1;
    // }

    // static constexpr size_t LG_BAR_WIDTH{log2(BAR_WIDTH)};

    // static constexpr int getBarIdxMask()
    // {
    //     return (NUM_BARS - 1) << LG_BAR_WIDTH;
    // }

    // static constexpr int BAR_IDX_MASK{getBarIdxMask()};

    // Color getBarIdx(int aX) { return (aX & BAR_IDX_MASK) >> LG_BAR_WIDTH; };

    FreqBarPositionsT<NUM_BARS> theCurrentBarPositions{};
    FreqBarPositionsT<NUM_BARS> theNextPositions{};
    FreqBarsUpdateQueueT<NUM_BARS> &theUpdateQueue;
    std::array<bool, NUM_BARS>
        theBarsAnimating{}; // could implement with a bit vector
    bool theAnimating{false};
    std::array<FrameBufferT *, MAX_ANIMATION_FRAMES> theNextFrameBuffers{};
    size_t theNextFrameBufferIdx{0};

public:
    FreqBarsRandom(FreqBarsUpdateQueueT<NUM_BARS> &aUpdateQueue)
        : theUpdateQueue{aUpdateQueue}
    {
    }

    bool doneAnimating()
    {
        for (const auto myBarAnimating : theBarsAnimating)
        {
            if (myBarAnimating)
            {
                return false;
            }
        }

        return true;
    }

    void animate()
    {
        if (doneAnimating())
        {
            auto myNumUpdatesAvailable{theUpdateQueue.read_available()};
            if (myNumUpdatesAvailable)
            {
                std::cout << "myNumUpdatesAvailable: " << myNumUpdatesAvailable
                          << std::endl;
                // Many of these member variables can become locals with this
                // new scheme
                theNextPositions = theUpdateQueue.front();
                theUpdateQueue.pop();
                theBarsAnimating.fill(true);
            }
        }

        if (!doneAnimating())
        {
            std::array<int, NUM_BARS> myDistances{};
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                auto myDistance{theNextPositions[i] -
                                theCurrentBarPositions[i]};
                myDistances[i] = myDistance;

                if (myDistance == 0)
                {
                    theBarsAnimating[i] = false;
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
            }
        }
        // else
        // {
        //     return; // TODO: Do we need to do anything to keep the old
        //             // frame? I think we do...
        // }

        // std::cout << "Start:\n";
        // for (size_t i{0}; i < NUM_BARS; ++i)
        // {
        //     std::cout << theCurrentBarPositions[i] << std::endl;
        // }
        // std::cout << "------------" << std::endl;

        // std::array<int, NUM_BARS> myDistances{};
        // for (size_t i{0}; i < NUM_BARS; ++i)
        // {
        //     auto myDistance{theNextPositions[i] -
        //                     theCurrentBarPositions[i]};
        //     myDistances[i] = myDistance;

        //     if (myDistance == 0)
        //     {
        //         theBarsAnimating[i] = false;
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

        // std::cout << "Now:\n";
        // for (size_t i{0}; i < NUM_BARS; ++i)
        // {
        //     std::cout << theCurrentBarPositions[i] << std::endl;
        // }

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
                            theNextFrameBuffers[theNextFrameBufferIdx]
                                ->SetPixel(x + myXOffset, y + myYOffset, 0, 0,
                                           0);
                        }
                    }
                    else
                    {
                        for (int myXOffset{0}; myXOffset < BAR_WIDTH;
                             ++myXOffset)
                        {
                            theNextFrameBuffers[theNextFrameBufferIdx]
                                ->SetPixel(x + myXOffset, y + myYOffset,
                                           myRowColor.getRed(),
                                           myRowColor.getGreen(),
                                           myRowColor.getBlue());
                        }
                    }

                    ++myBarIdx; // We can determine these without using
                                // the extra x2/y2 loops
                }
            }
            ++myRowColorIdx; // We can determine these without using the
                             // extra x2/y2 loops
            myRowColor = ROW_COLORS[myRowColorIdx];
        }
        ++theNextFrameBufferIdx;

        if (theNextFrameBufferIdx == MAX_ANIMATION_FRAMES)
        {
            theNextFrameBufferIdx = 0;
        }
    }

    void setNextFrameBuffers(
        const std::array<FrameBufferT *, MAX_ANIMATION_FRAMES> &aBuffers)
    {
        theNextFrameBuffers = aBuffers;
    }
};
} // namespace rendering
} // namespace matrix