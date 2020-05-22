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

// TODO: This NUM_GRADIENTS param serves no purpose
template <size_t NUM_GRADIENTS, size_t NUM_COLORS_PER_GRADIENT>
constexpr std::array<Color, NUM_GRADIENTS * NUM_COLORS_PER_GRADIENT>
createRowColors()
{
    std::array<GradientT<NUM_COLORS_PER_GRADIENT>, NUM_GRADIENTS> myGradients{
        createGradient<255, 0, 0, 255, 127, 0, NUM_COLORS_PER_GRADIENT>(),
        createGradient<255, 128, 0, 255, 254, 0, NUM_COLORS_PER_GRADIENT>(),
        createGradient<255, 255, 0, 1, 255, 0, NUM_COLORS_PER_GRADIENT>(),
        createGradient<0, 255, 0, 0, 0, 0, NUM_COLORS_PER_GRADIENT>()};

    std::array<Color, NUM_GRADIENTS * NUM_COLORS_PER_GRADIENT> myRowColors{};
    for (size_t i{0}; i < NUM_GRADIENTS; ++i)
    {
        for (size_t j{0}; j < NUM_COLORS_PER_GRADIENT; ++j)
        {
            myRowColors[i * NUM_COLORS_PER_GRADIENT + j] = myGradients[i][j];
        }
    }

    return myRowColors;
}
} // namespace rendering
} // namespace matrix
