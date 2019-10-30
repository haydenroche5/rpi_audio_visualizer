#pragma once

#include "Defs.hpp"
#include <fstream>

namespace matrix
{
namespace audio
{
class FftMagnitudePrinter
{
private:
    static constexpr size_t FFT_OUTPUTS{512};

    AudioQueueT &theQueue;
    bool theStop{false};
    const char *theMagsFileName{"fft_mags.txt"};
    std::ofstream theMagsFileStream;
    FftInputArrayT theFftInput{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT theFftOutput{FFT_OUTPUTS, FFT_ALIGNMENT};

public:
    FftMagnitudePrinter(AudioQueueT &aQueue);
    ~FftMagnitudePrinter();
    void print();
    void stop();
    bool isStopped() const;
};
} // namespace audio
} // namespace matrix