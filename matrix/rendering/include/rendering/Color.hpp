#pragma once

#include <array>
#include <ostream>

namespace matrix
{
namespace rendering
{

static constexpr size_t NUM_COLOR_CHANNELS{3};

class Color
{
public:
    using ChannelT = uint8_t;
    using ChannelsT = std::array<ChannelT, NUM_COLOR_CHANNELS>;

private:
    ChannelsT theChannels;

public:
    constexpr Color() : theChannels{0, 0, 0} {}
    constexpr Color(ChannelT aRed, ChannelT aGreen, ChannelT aBlue)
        : theChannels{aRed, aGreen, aBlue}
    {
    }

    ChannelsT getChannels() const { return theChannels; }

    ChannelT getRed() const { return theChannels[0]; }

    ChannelT getGreen() const { return theChannels[1]; }

    ChannelT getBlue() const { return theChannels[2]; }

    void setChannels(ChannelsT aChannels) { theChannels = aChannels; }

    void setRed(ChannelT aValue) { theChannels[0] = aValue; }

    void setGreen(ChannelT aValue) { theChannels[1] = aValue; }

    void setBlue(ChannelT aValue) { theChannels[2] = aValue; }
};

std::ostream &operator<<(std::ostream &aStream, const Color &aColor)
{
    aStream << "R: " << +aColor.getRed() << ", G: " << +aColor.getGreen()
            << ", B: " << +aColor.getBlue();
    return aStream;
}
} // namespace rendering
} // namespace matrix