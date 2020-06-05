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
static constexpr size_t FRAMES_PER_BUFFER{1024};
static constexpr size_t SAMPLES_PER_BUFFER{CHANNELS * FRAMES_PER_BUFFER};
static constexpr size_t FFT_POINTS{SAMPLES_PER_BUFFER};
static constexpr size_t FFT_POINTS_REAL{SAMPLES_PER_BUFFER / 2 + 1};
static constexpr float FFT_FUNDAMENTAL_FREQ{static_cast<float>(SAMPLE_RATE) /
                                            FFT_POINTS};
static constexpr PaStreamFlags STREAM_FLAGS{paClipOff};
static constexpr size_t AUDIO_QUEUE_DEPTH{1};
static constexpr auto FFT_ALIGNMENT{sizeof(Complex)};
static constexpr size_t BAR_QUEUE_DEPTH{1};

using SampleT = float;
using BufferT = std::array<SampleT, SAMPLES_PER_BUFFER>;
using AudioQueueT = boost::lockfree::spsc_queue<BufferT>;
using BarQueueT = boost::lockfree::spsc_queue<std::pair<size_t, double>>;
using SamplesArrayT = Array::array1<double>;
using FftInputArrayT = SamplesArrayT;
using FftOutputArrayT = Array::array1<Complex>;
} // namespace audio
} // namespace matrix
