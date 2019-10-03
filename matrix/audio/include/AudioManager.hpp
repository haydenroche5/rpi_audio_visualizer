#pragma once

#include "dependencies/portaudio/include/portaudio.h"
#include <memory>

namespace matrix
{
namespace audio
{
class AudioManager
{
public:
    constexpr size_t NUM_CHANNELS{1};
    constexpr double SAMPLE_RATE{44100};
    constexpr unsigned long FRAMES_PER_BUFFER{512};

    using SampleT = paFloat32;
    using StreamT = PaStream;

private:
    std::unique_ptr<StreamT> theStream{nullptr};
    PaStreamParameters theStreamInputParams{};

public:
    AudioManager();
    void openStream();
};
} // namespace audio
} // namespace matrix