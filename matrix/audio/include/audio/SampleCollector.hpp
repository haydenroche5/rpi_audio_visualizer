#pragma once

#include "Defs.hpp"
#include <vector>

namespace matrix
{
namespace audio
{
class SampleCollector
{
public:
    using SamplesT = std::vector<SampleT>;

private:
    AudioQueueT &theQueue;
    bool theStop{false};
    SamplesT theSamples{};

public:
    SampleCollector(AudioQueueT &aQueue, size_t aInitialSize);
    void collect();
    void stop();
    bool isStopped() const;
    const SamplesT &getSamples() const;
};
} // namespace audio
} // namespace matrix
