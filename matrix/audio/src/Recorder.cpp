#include "Recorder.hpp"
#include <cstring>

namespace matrix
{
namespace audio
{
Recorder::Recorder(AudioQueueT &aQueue, bool aIsStreamingMode,
                   size_t aSecondsToRecord)
    : theQueue{aQueue}, theIsStreamingMode{aIsStreamingMode},
      theSamplesToRecord{aSecondsToRecord * SAMPLE_RATE}
{
    // TODO: shouldn't allow aSecondsToRecord to be non-zero if aIsStreamingMode
    // is true
}

bool Recorder::isDoneRecording() const { return theIsDoneRecording; }

int Recorder::paCallbackFun(const void *aInputBuffer, void *, unsigned long,
                            const PaStreamCallbackTimeInfo *,
                            PaStreamCallbackFlags)
{
    if (theIsDoneRecording)
    {
        return paComplete;
    }

    std::memcpy(theCurrentBuffer.data(),
                static_cast<const float *>(aInputBuffer), SAMPLES_PER_BUFFER);
    theQueue.push(theCurrentBuffer);

    // TODO: Stream vs. non-stream should be a compile time thing, i.e. set with
    // a template parameter, and then this code can be constexpr-if'ed
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
