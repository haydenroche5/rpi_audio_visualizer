#pragma once

#include "ColorUtils.hpp"
#include "Defs.hpp"
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
template <size_t NUM_BARS, size_t NUM_COLORS_PER_GRADIENT,
          size_t ANIMATION_SPEED>
class FreqBarsRandom
{
    static constexpr size_t NUM_GRADIENTS{4};

    static_assert(NUM_COLS % NUM_BARS == 0,
                  "FreqBarsRandom instantiated with a NUM_BARS that doesn't "
                  "evenly divide NUM_COLS.");
    static_assert(NUM_ROWS % (NUM_COLORS_PER_GRADIENT * NUM_GRADIENTS) == 0,
                  "FreqBarsRandom instantiated with a NUM_COLORS_PER_GRADIENT "
                  "that doesn't "
                  "evenly divide NUM_ROWS.");
    static_assert(
        NUM_ROWS % ANIMATION_SPEED == 0,
        "FreqBarsRandom instantiated with a ANIMATION_SPEED that doesn't "
        "evenly divide NUM_ROWS.");

private:
    static constexpr size_t BAR_WIDTH{NUM_COLS / NUM_BARS};
    static constexpr size_t COLOR_HEIGHT{NUM_ROWS / NUM_COLORS_PER_GRADIENT};

    static constexpr auto ROW_COLORS{
        createRowColors<NUM_GRADIENTS, NUM_COLORS_PER_GRADIENT>()};

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
    FrameBufferT *theNextFrameBuffer{nullptr};

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

    void draw()
    {
        if (theNextFrameBuffer == nullptr)
        {
            throw std::runtime_error("FreqBarsRandom::draw called while "
                                     "next frame buffers is nullptr.");
        }
        if (!theAnimating)
        {
            if (theUpdateQueue.read_available())
            {
                theNextPositions = theUpdateQueue.front();
                theUpdateQueue.pop();
                theAnimating = true;
                theBarsAnimating.fill(true);
            }
            else
            {
                return; // TODO: Do we need to do anything to keep the old
                        // frame? I think we do...
            }
        }

        if (theAnimating)
        {
            auto myRowColorIdx{0};
            auto myRowColor{ROW_COLORS[myRowColorIdx]};
            for (int y{0}; y < NUM_ROWS; y += COLOR_HEIGHT)
            {
                for (int y2{y}; y2 < COLOR_HEIGHT; ++y2)
                {
                    size_t myBarIdx{0};
                    auto myCurrentBarPos{theCurrentBarPositions[myBarIdx]};
                    auto myNextBarPos{theNextPositions[myBarIdx]};
                    for (int x{0}; x < NUM_COLS; x += BAR_WIDTH)
                    {
                        int myDistance{myNextBarPos - myCurrentBarPos};
                        if (myDistance == 0)
                        {
                            theBarsAnimating[myBarIdx] = false;
                            // turn pixel on?
                            continue;
                        }
                        else if (myDistance < 0)
                        {
                            for (int x2{x}; x2 < BAR_WIDTH; ++x2)
                            {
                                theNextFrameBuffer->SetPixel(x2, y2, 0, 0, 0);
                            }
                            --theCurrentBarPositions[myBarIdx];
                        }
                        else
                        {
                            for (int x2{x}; x2 < BAR_WIDTH; ++x2)
                            {
                                theNextFrameBuffer->SetPixel(
                                    x2, y2, myRowColor.getRed(),
                                    myRowColor.getGreen(),
                                    myRowColor.getBlue());
                            }
                            ++theCurrentBarPositions[myBarIdx];
                        }

                        ++myBarIdx; // We can determine these without using the
                                    // extra x2/y2 loops
                        myCurrentBarPos = theCurrentBarPositions[myBarIdx];
                        myNextBarPos = theNextPositions[myBarIdx];
                    }
                }
                ++myRowColorIdx; // We can determine these without using the
                                 // extra x2/y2 loops
                myRowColor = ROW_COLORS[myRowColorIdx];
            }
        }

        if (doneAnimating())
        {
            theAnimating = false;
        }
    }

    void setNextFrameBuffer(FrameBufferT *aBuffer)
    {
        theNextFrameBuffer = aBuffer;
    }
};
} // namespace rendering
} // namespace matrix