#pragma once

#include "Array.h"
#include "fftw++.h"
#include "portaudiocpp/SampleDataFormat.hxx"
#include <boost/lockfree/spsc_queue.hpp>

namespace matrix
{
namespace audio
{
static constexpr size_t SAMPLE_RATE{16000};
static constexpr double SAMPLE_PERIOD{1.0 / SAMPLE_RATE};
static constexpr portaudio::SampleDataFormat SAMPLE_FORMAT{portaudio::FLOAT32};
static constexpr bool INTERLEAVED{true};
static constexpr size_t CHANNELS{1};
static constexpr PaStreamFlags STREAM_FLAGS{paClipOff};
static constexpr size_t AUDIO_QUEUE_DEPTH{1};
static constexpr auto FFT_ALIGNMENT{sizeof(Complex)};
static constexpr size_t BAR_QUEUE_DEPTH{1};

using SampleT = float;
using BufferT = std::vector<SampleT>;
using AudioQueueT = boost::lockfree::spsc_queue<BufferT>;
using FftInputArrayT = Array::array1<double>;
using FftOutputArrayT = Array::array1<Complex>;
} // namespace audio
} // namespace matrix
