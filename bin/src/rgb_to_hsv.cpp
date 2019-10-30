#include <algorithm>
#include <array>
#include <exception>
#include <iostream>
#include <string>

struct HSV
{
    double theHue;
    double theSaturation;
    double theValue;

    HSV(double aHue, double aSaturation, double aValue)
        : theHue{aHue}, theSaturation{aSaturation}, theValue{aValue}
    {
    }
};

HSV rgbToHsv(double aRed, double aGreen, double aBlue)
{
    auto myNormalizedRed{aRed / 255};
    auto myNormalizedGreen{aGreen / 255};
    auto myNormalizedBlue{aBlue / 255};

    auto myMax{
        std::max({myNormalizedRed, myNormalizedGreen, myNormalizedBlue})};
    auto myMin{
        std::min({myNormalizedRed, myNormalizedGreen, myNormalizedBlue})};
    auto myMaxMinDiff{myMax - myMin};

    double myHue{0};
    if (myMaxMinDiff == 0)
    {
        myHue = 0;
    }
    else if (myMax == myNormalizedRed)
    {
        myHue = 60 * ((myNormalizedGreen - myNormalizedBlue) / myMaxMinDiff);
    }
    else if (myMax == myNormalizedGreen)
    {
        myHue =
            60 * (((myNormalizedBlue - myNormalizedRed) / myMaxMinDiff) + 2);
    }
    else if (myMax == myNormalizedBlue)
    {
        myHue =
            60 * (((myNormalizedRed - myNormalizedGreen) / myMaxMinDiff) + 4);
    }
    else
    {
        throw std::runtime_error(
            "rgbToHsv: myMax doesn't equal any of the normalized RGB values.");
    }

    if (myHue < 0)
    {
        myHue += 360;
    }

    double mySaturation{myMax == 0 ? 0 : myMaxMinDiff / myMax};
    double myValue{myMax};

    return {myHue, mySaturation, myValue};
}

struct RGB
{
    uint8_t theRed;
    uint8_t theGreen;
    uint8_t theBlue;

    RGB(uint8_t aRed, uint8_t aGreen, uint8_t aBlue)
        : theRed{aRed}, theGreen{aGreen}, theBlue{aBlue}
    {
    }

    RGB() : theRed{0}, theGreen{0}, theBlue{0} {}
};

// std::vector<RGB> createGradient(const RGB &aStartingColor,
//                                 const RGB &aEndingColor, size_t NUM_COLORS)
// {
//     auto myRedStep{std::abs(R1 - R2) /
//                    (NUM_COLORS - 1)};
//     auto myGreenStep{std::abs(G1 -
//     G2) /
//                      (NUM_COLORS - 1)};
//     auto myBlueStep{std::abs(B1 - B2) /
//                     (NUM_COLORS - 1)};
//     uint8_t myRed{R1};
//     uint8_t myGreen{G1};
//     uint8_t myBlue{B1};

//     std::vector<RGB> myGradientColors{aStartingColor};

//     for (size_t i{0}; i < NUM_COLORS - 2; ++i)
//     {
//         if (R1 < R2)
//         {
//             myRed += myRedStep;
//         }
//         else if (R1 > R2)
//         {
//             myRed -= myRedStep;
//         }

//         if (G1 < G2)
//         {
//             myGreen += myGreenStep;
//         }
//         else if (G1 > G2)
//         {
//             myGreen -= myGreenStep;
//         }

//         if (B1 < B2)
//         {
//             myBlue += myBlueStep;
//         }
//         else if (B1 > B2)
//         {
//             myBlue -= myBlueStep;
//         }
//         myGradientColors.push_back(RGB{myRed, myGreen, myBlue});
//     }
//     myGradientColors.push_back(aEndingColor);

//     return myGradientColors;
// }

template <uint8_t R1, uint8_t G1, uint8_t B1, uint8_t R2, uint8_t G2,
          uint8_t B2, size_t NUM_COLORS>
static constexpr std::array<RGB, NUM_COLORS> createGradient()
{
    auto myRedStep{std::abs(R1 - R2) / (NUM_COLORS - 1)};
    auto myGreenStep{std::abs(G1 - G2) / (NUM_COLORS - 1)};
    auto myBlueStep{std::abs(B1 - B2) / (NUM_COLORS - 1)};
    uint8_t myRed{R1};
    uint8_t myGreen{G1};
    uint8_t myBlue{B1};

    std::array<RGB, NUM_COLORS> myGradientColors{};
    myGradientColors[0] = RGB{R1, G1, B1};
    for (size_t i{1}; i < NUM_COLORS - 2; ++i)
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
        myGradientColors[i] = RGB{myRed, myGreen, myBlue};
    }
    myGradientColors[NUM_COLORS - 1] = RGB{R2, G2, B2};

    return myGradientColors;
}

int main()
{
    auto myGradientColors{createGradient<0, 0, 0, 0, 255, 0, 10>()};

    for (const auto &myGradientColor : myGradientColors)
    {
        std::cout << "(" << +myGradientColor.theRed << ", "
                  << +myGradientColor.theGreen << ", "
                  << +myGradientColor.theBlue << ")" << std::endl;
    }

    // auto myHsv{rgbToHsv(myRed, myGreen, myBlue)};

    // std::cout << "RGB: (" << myRed << ", " << myGreen << ", " << myBlue <<
    // ")"
    //           << std::endl;
    // std::cout << "HSV: (" << myHsv.theHue << ", " << myHsv.theSaturation <<
    // ", "
    //           << myHsv.theValue << ")" << std::endl;
}
