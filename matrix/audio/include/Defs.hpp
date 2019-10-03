#pragma once

#include "SampleDataFormat.hxx"
#include <boost/lockfree/spsc_queue.hpp>

namespace matrix
{
namespace audio
{
static constexpr double SAMPLE_RATE{41000};
static constexpr portaudio::SampleDataFormat SAMPLE_FORMAT{portaudio::FLOAT32};
static constexpr bool INTERLEAVED{true};
static constexpr size_t CHANNELS{2};
static constexpr size_t FRAMES_PER_BUFFER{128};
static constexpr size_t SAMPLES_PER_BUFFER{CHANNELS * FRAMES_PER_BUFFER};
static constexpr PaStreamFlags STREAM_FLAGS{paClipOff};
static constexpr size_t QUEUE_DEPTH{64};

using SampleT = float;
using BufferT = std::array<SampleT, SAMPLES_PER_BUFFER>;
using QueueT = boost::lockfree::spsc_queue<BufferT>;
} // namespace audio
} // namespace matrix
