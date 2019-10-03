#include "RecorderStream.hpp"

namespace matrix
{
namespace audio
{
RecorderStream::RecorderStream(Recorder &aRecorder,
                               const portaudio::StreamParameters &aStreamParams)
    : portaudio::InterfaceCallbackStream(aStreamParams, aRecorder)
{
}
} // namespace audio
} // namespace matrix
