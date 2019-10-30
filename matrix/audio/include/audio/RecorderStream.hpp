#pragma once

#include "Recorder.hpp"
#include "portaudiocpp/InterfaceCallbackStream.hxx"

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