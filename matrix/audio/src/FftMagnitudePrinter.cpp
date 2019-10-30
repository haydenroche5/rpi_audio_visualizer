#include "FftMagnitudePrinter.hpp"
#include "fftw++.h"
#include <iostream>

namespace matrix
{
namespace audio
{
FftMagnitudePrinter::FftMagnitudePrinter(AudioQueueT &aQueue) : theQueue{aQueue}
{
    fftwpp::fftw::maxthreads = get_max_threads();
    theMagsFileStream.open(theMagsFileName);
}

FftMagnitudePrinter::~FftMagnitudePrinter()
{
    theMagsFileStream.flush();
    if (theMagsFileStream.is_open())
    {
        theMagsFileStream.close();
    }
}

void FftMagnitudePrinter::print()
{
    while (theQueue.read_available() > 0)
    {
        const auto &myBuffer{theQueue.front()};
        for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
        {
            theFftInput[i] = myBuffer[i];
        }

        fftwpp::rcfft1d myFft(SAMPLES_PER_BUFFER, theFftInput, theFftOutput);
        myFft.fft(theFftInput, theFftOutput);

        for (size_t i{0}; i < FFT_OUTPUTS; ++i)
        {
            theMagsFileStream << std::abs(theFftOutput[i]) << "\n";
        }

        theQueue.pop();
    }
}

void FftMagnitudePrinter::stop()
{
    theStop = true;
    theMagsFileStream.flush();
    theMagsFileStream.close();
}

bool FftMagnitudePrinter::isStopped() const { return theStop; }
} // namespace audio
} // namespace matrix
