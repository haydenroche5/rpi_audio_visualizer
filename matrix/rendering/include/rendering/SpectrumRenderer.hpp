#pragma once

#include "Defs.hpp"
#include "FftCalculator.hpp"

namespace matrix
{
namespace rendering
{
class SpectrumRenderer
{
private:
    struct FreqAndMag
    {
        double theFreq{0};
        double theMag{0};

        FreqAndMag(double aFreq, double aMag);
    };

    QueueT &theSampleQueue;

public:
    using FreqsAndMagsT = std::array<FreqAndMag, FFT_POINTS_REAL>;

    SpectrumRenderer(QueueT &aQueue);
    void hannWindowSamples(BufferT &aBuffer);
    FreqsAndMagsT calculateFftMags(const BufferT &aBuffer);
    void render();
};
} // namespace rendering
} // namespace matrix
