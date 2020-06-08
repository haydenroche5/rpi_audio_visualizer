#pragma once

#include "Defs.hpp"
#include "portaudiocpp/CallbackInterface.hxx"

namespace matrix
{
namespace audio
{
class Recorder : public portaudio::CallbackInterface
{
public:
    Recorder(AudioQueueT &aQueue, bool aIsStreamingMode = true,
             size_t aSecondsToRecord = 0, bool aVerbose = false);
    bool isDoneRecording() const;

private:
    AudioQueueT &theQueue;
    BufferT theCurrentBuffer{};
    bool theIsStreamingMode;
    size_t theSamplesToRecord;
    size_t theSamplesRecorded{0};
    bool theIsDoneRecording{false};
    bool theVerbose;

    int paCallbackFun(const void *aInputBuffer, void *, unsigned long,
                      const PaStreamCallbackTimeInfo *,
                      PaStreamCallbackFlags) override;
};
} // namespace audio
} // namespace matrix