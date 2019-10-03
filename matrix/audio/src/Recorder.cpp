#include "Recorder.hpp"

namespace matrix
{
namespace audio
{
Recorder::Recorder(QueueT &aQueue) : theQueue{aQueue} {}

int Recorder::paCallbackFun(const void *aInputBuffer, void *, unsigned long,
                            const PaStreamCallbackTimeInfo *,
                            PaStreamCallbackFlags)
{
    const float *myCurrentSamplePtr{static_cast<const float *>(aInputBuffer)};

    if (aInputBuffer == nullptr)
    {
        for (size_t i{0}; i < FRAMES_PER_BUFFER; ++i)
        {
            for (size_t j{0}; j < CHANNELS; ++j)
            {
                theCurrentBuffer[i + j] = 0;
            }
        }
    }
    else
    {
        for (size_t i{0}; i < FRAMES_PER_BUFFER; ++i)
        {
            for (size_t j{0}; j < CHANNELS; ++j)
            {
                theCurrentBuffer[i + j] = *myCurrentSamplePtr++;
            }
        }
    }

    theQueue.push(theCurrentBuffer);

    return paContinue;
}
} // namespace audio
} // namespace matrix
