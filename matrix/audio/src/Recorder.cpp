#include "Recorder.hpp"
#include <cstring>
#include <stdexcept>

namespace matrix
{
namespace audio
{
Recorder::Recorder(AudioQueueT &aQueue, bool aIsStreamingMode,
                   size_t aSecondsToRecord, bool aVerbose)
    : theQueue{aQueue}, theIsStreamingMode{aIsStreamingMode},
      theSamplesToRecord{aSecondsToRecord * SAMPLE_RATE}, theVerbose{aVerbose}
{
    if (aSecondsToRecord != 0 && aIsStreamingMode)
    {
        throw std::invalid_argument{
            "matrix::audio::Recorder::Recorder: Can't enable streaming mode if "
            "seconds to record is non-zero."};
    }
}

bool Recorder::isDoneRecording() const { return theIsDoneRecording; }

int Recorder::paCallbackFun(const void *aInputBuffer, void *,
                            unsigned long aFrameCount,
                            const PaStreamCallbackTimeInfo *aTimeInfo,
                            PaStreamCallbackFlags aStatusFlags)
{
    if (theIsDoneRecording)
    {
        return paComplete;
    }

    std::memcpy(theCurrentBuffer.data(),
                static_cast<const float *>(aInputBuffer),
                SAMPLES_PER_BUFFER * sizeof(float));

    auto myPushSuccess{theQueue.push(theCurrentBuffer)};

    if (theVerbose)
    {
        std::cout << "Current time - ADC time: "
                  << (aTimeInfo->currentTime - aTimeInfo->inputBufferAdcTime)
                  << std::endl;
    }

    if (theVerbose && !myPushSuccess)
    {
        std::cout << "Failed to push audio. (" << aFrameCount << ")"
                  << std::endl;
    }

    if (!theIsStreamingMode)
    {
        theSamplesRecorded += SAMPLES_PER_BUFFER;

        if (theSamplesRecorded >= theSamplesToRecord)
        {
            theIsDoneRecording = true;
            return paComplete;
        }
    }

    return paContinue;
}
} // namespace audio
} // namespace matrix
