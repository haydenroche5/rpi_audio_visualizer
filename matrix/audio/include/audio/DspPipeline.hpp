#pragma once

#include "Defs.hpp"

namespace matrix
{
namespace audio
{
template <size_t NUM_BARS> class DspPipeline
{
private:
    static constexpr std::array<float, NUM_BARS> OCTAVE_BOUNDARIES{
        14.1, 17.8, 22.4, 28.2, 35.5, 44.7, 56.2, 70.8, 89.1, 112,
        141,  178,  224,  282,  355,  447,  562,  708,  891,  1122,
        1413, 1778, 2239, 2818, 3548, 4467, 5623, 7079};

    size_t theBufferSize;
    float thePreEmphasisFactor;
    size_t theFftPointsReal;
    float theFftFundamentalFreq;
    bool thePreviousBufferExists{false};
    BufferT thePreviousBuffer;
    BufferT theOverlappedBuffer;
    BufferT thePreEmphasizedBuffer;
    FftInputArrayT theFftInput;
    FftOutputArrayT theFftOutput;
    std::array<float, NUM_BARS> theMaxMagnitudePerOctave{};

public:
    DspPipeline(size_t aBufferSize, float aPreEmphasisFactor)
        : theBufferSize{aBufferSize}, thePreEmphasisFactor{aPreEmphasisFactor},
          theFftPointsReal{aBufferSize / 2 + 1},
          theFftFundamentalFreq{static_cast<float>(SAMPLE_RATE) / aBufferSize},
          thePreviousBuffer(aBufferSize / 2, 0),
          theOverlappedBuffer(aBufferSize, 0),
          thePreEmphasizedBuffer(aBufferSize, 0), theFftInput{aBufferSize,
                                                              FFT_ALIGNMENT},
          theFftOutput{theFftPointsReal, FFT_ALIGNMENT}
    {
    }

    std::array<float, NUM_BARS> operator()(const BufferT &aAudioBuffer)
    {
        // Overlap consecutive buffers by 50%
        if (thePreviousBufferExists)
        {
            std::copy(thePreviousBuffer.begin(), thePreviousBuffer.end(),
                      theOverlappedBuffer.begin());
            std::copy(aAudioBuffer.begin(),
                      aAudioBuffer.begin() + theBufferSize / 2,
                      theOverlappedBuffer.begin() + theBufferSize / 2);
        }
        else
        {
            theOverlappedBuffer = aAudioBuffer;
            thePreviousBufferExists = true;
        }
        std::copy(aAudioBuffer.begin() + theBufferSize / 2, aAudioBuffer.end(),
                  thePreviousBuffer.begin());

        // Pre-emphasis to make the higher frequencies stand out more
        thePreEmphasizedBuffer[0] = theOverlappedBuffer[0];
        for (size_t i{1}; i < theBufferSize; ++i)
        {
            thePreEmphasizedBuffer[i] =
                theOverlappedBuffer[i] -
                thePreEmphasisFactor * theOverlappedBuffer[i - 1];
        }

        // Window samples with a Hann window.
        for (size_t i{0}; i < theBufferSize; ++i)
        {
            theFftInput[i] = thePreEmphasizedBuffer[i] * 0.5 *
                             (1 - std::cos((2 * M_PI * i) / theBufferSize));
        }

        // Take the (fast) Fourier transform of the samples.
        fftwpp::rcfft1d myFft(theBufferSize, theFftInput, theFftOutput);
        myFft.fft(theFftInput, theFftOutput);

        // Compute the max FFT magnitude in each octave band.
        size_t myOctaveBoundaryIdx{0};
        float myMaxMagnitude{0};

        for (size_t i{0}; i < theFftPointsReal; ++i)
        {
            auto myFrequency{i * theFftFundamentalFreq};

            if (myFrequency >= OCTAVE_BOUNDARIES[myOctaveBoundaryIdx])
            {
                theMaxMagnitudePerOctave[myOctaveBoundaryIdx] = myMaxMagnitude;
                myMaxMagnitude = 0;
                ++myOctaveBoundaryIdx;

                if (myOctaveBoundaryIdx == NUM_BARS)
                {
                    break;
                }
            }

            auto myMagnitude{std::abs(theFftOutput[i])};

            if (myMagnitude > myMaxMagnitude)
            {
                myMaxMagnitude = std::abs(theFftOutput[i]);
            }
        }

        return theMaxMagnitudePerOctave;
    }
};
} // namespace audio
} // namespace matrix
