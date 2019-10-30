#include "SpectrumRenderer.hpp"

namespace matrix
{
namespace rendering
{

SpectrumRenderer::FreqAndMag::FreqAndMag(double aFreq, double aMag)
    : theFreq{aFreq}, theMag{aMag}
{
}

void SpectrumRenderer::hannWindowSamples(BufferT &aBuffer)
{
    for (size_t i{0}; i < aBuffer.size(); ++i)
    {
        aBuffer[i] =
            aBuffer[i] * 0.5 * (1 - std::cos((2 * M_PI * i) / aBuffer.size()));
    }
}

SpectrumRenderer::FreqsAndMagsT
SpectrumRenderer::calculateFftMags(const BufferT &aBuffer)
{
    SamplesArrayT myFftSamples{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    for (size_t i{0}; i < aBuffer.size(); ++i)
    {
        myFftSamples[i] = aBuffer[i];
    }
    fftwpp::rcfft1d myFft(SAMPLES_PER_BUFFER, myFftSamples, myFftOutput);
    myFft.fft(myFftSamples, myFftOutput);

    SpectrumRenderer::FreqsAndMagsT myFreqsAndMags{};
    for (size_t i{0}; i < myFreqsAndMags.size(); ++i)
    {
        myFreqsAndMags[i] = {i * FFT_FUNDAMENTAL_FREQ,
                             std::abs(myFftOutput[i])};
    }

    return myFreqsAndMags;
}

void SpectrumRenderer::render()
{
    size_t myBuffersRead{0};
    BufferT myCurrentBuffer{};
    while (theQueue.read_available() > 0) // TODO: Is this ever > 1?
    {
        auto &myQueueBuffer{theQueue.front()};
        for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
        {
            myCurrentBuffer[i] = myQueueBuffer[i];
        }
        theQueue.pop();
    }

    hannWindowSamples(myCurrentBuffer);
    auto myFreqsAndMags{calculateFftMags(myCurrentBuffer)};
}
} // namespace rendering
} // namespace matrix