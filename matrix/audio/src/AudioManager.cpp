#include <stdexcept>

#include "matrix/include/audio/AudioManager.hpp"

namespace matrix
{
namespace audio
{
AudioManager::AudioManager()
{
    auto myPortAudioError{Pa_Initialize()};
    if (myPortAudioError != paNoError)
    {
        throw std::runtime_error(
            "AudioManager::AudioManager: Failed to Pa_Initialize.");
    }

    theStreamInputParams.device = Pa_GetDefaultInputDevice();
    if (theStreamInputParams.device == paNoDevice)
    {
        throw std::runtime_error(
            "AudioManager::AudioManager: Failed to Pa_GetDefaultInputDevice.");
    }

    theStreamInputParams.channelCount = NUM_CHANNELS;
    theStreamInputParams.sampleFormat = SampleT;
    theStreamInputParams.suggestedLatency =
        Pa_GetDeviceInfo(myinputParameters.device)
            ->defaultLowInputLatency; // TODO: Read more about this.
    theStreamInputParams.hostApiSpecificStreamInfo =
        nullptr; // TODO: Needed? Or will it be defaulted to 0?
}

void AudioManager::openStream()
{
    auto myPortAudioError{Pa_OpenStream(
        &(theStream.get()), &theStreamInputParams,
        nullptr, /* &outputParameters, */
        SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, recordCallback, &data)};
}
} // namespace audio
} // namespace matrix