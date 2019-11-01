#pragma once

#include "Color.hpp"
#include <array>

namespace matrix
{
namespace rendering
{
template <size_t NUM_COLORS> using GradientT = std::array<Color, NUM_COLORS>;

template <uint8_t R1, uint8_t G1, uint8_t B1, uint8_t R2, uint8_t G2,
          uint8_t B2, size_t NUM_COLORS>
static constexpr GradientT<NUM_COLORS> createGradient()
{
    auto myRedStep{std::abs(R1 - R2) / (NUM_COLORS - 1)};
    auto myGreenStep{std::abs(G1 - G2) / (NUM_COLORS - 1)};
    auto myBlueStep{std::abs(B1 - B2) / (NUM_COLORS - 1)};

    std::array<Color, NUM_COLORS> myGradientColors{};
    myGradientColors[0] = Color{R1, G1, B1};
    uint8_t myRed{R1};
    uint8_t myGreen{G1};
    uint8_t myBlue{B1};
    for (size_t i{1}; i < NUM_COLORS - 1; ++i)
    {
        if (R1 < R2)
        {
            myRed += myRedStep;
        }
        else if (R1 > R2)
        {
            myRed -= myRedStep;
        }

        if (G1 < G2)
        {
            myGreen += myGreenStep;
        }
        else if (G1 > G2)
        {
            myGreen -= myGreenStep;
        }

        if (B1 < B2)
        {
            myBlue += myBlueStep;
        }
        else if (B1 > B2)
        {
            myBlue -= myBlueStep;
        }
        myGradientColors[i] = Color{myRed, myGreen, myBlue};
    }
    myGradientColors[NUM_COLORS - 1] = Color{R2, G2, B2};

    return myGradientColors;
}

static constexpr Color CLEAR_MASK{0x00, 0x00, 0x00};
static constexpr Color KEEP_MASK{0xFF, 0xFF, 0xFF};
} // namespace rendering
} // namespace matrix
