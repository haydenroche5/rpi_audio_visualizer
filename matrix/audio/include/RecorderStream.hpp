#pragma once

#include "InterfaceCallbackStream.hxx"
#include "Recorder.hpp"

namespace matrix
{
namespace audio
{
class RecorderStream : public portaudio::InterfaceCallbackStream
{
public:
    RecorderStream(Recorder &aRecorder,
                   const portaudio::StreamParameters &aStreamParams);
};
} // namespace audio
} // namespace matrix