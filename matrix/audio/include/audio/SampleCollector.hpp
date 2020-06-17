#pragma once

#include "Defs.hpp"
#include <vector>

namespace matrix
{
namespace audio
{
class SampleCollector
{
private:
    AudioQueueT &theQueue;
    bool theStop{false};
    BufferT theSamples{};

public:
    SampleCollector(AudioQueueT &aQueue, size_t aInitialSize);
    void collect();
    void stop();
    bool isStopped() const;
    const BufferT &getSamples() const;
};
} // namespace audio
} // namespace matrix
