#pragma once

#include <array>

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
    Color(ChannelT aRed, ChannelT aGreen, ChannelT aBlue);

    ChannelsT getChannels() const;
    ChannelT getRed() const;
    ChannelT getGreen() const;
    ChannelT getBlue() const;

    void setChannels(ChannelsT aChannels);
    void setRed(ChannelT aValue);
    void setGreen(ChannelT aValue);
    void setBlue(ChannelT aValue);
};
} // namespace rendering
} // namespace matrix