#include "matrix/include/rendering/Color.hpp"

namespace matrix
{
namespace rendering
{
Color::Color(ChannelT aRed, ChannelT aGreen, ChannelT aBlue)
    : theChannels{aRed, aGreen, aBlue}
{
}

Color::ChannelsT Color::getChannels() const { return theChannels; }

Color::ChannelT Color::getRed() const { return theChannels[0]; }

Color::ChannelT Color::getGreen() const { return theChannels[1]; }

Color::ChannelT Color::getBlue() const { return theChannels[2]; }

void Color::setChannels(Color::ChannelsT aChannels) { theChannels = aChannels; }

void Color::setRed(Color::ChannelT aValue) { theChannels[0] = aValue; }

void Color::setGreen(Color::ChannelT aValue) { theChannels[1] = aValue; }

void Color::setBlue(Color::ChannelT aValue) { theChannels[2] = aValue; }
} // namespace rendering
} // namespace matrix
