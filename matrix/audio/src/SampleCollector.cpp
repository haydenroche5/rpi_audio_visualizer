#include "SampleCollector.hpp"

namespace matrix
{
namespace audio
{
SampleCollector::SampleCollector(AudioQueueT &aQueue, size_t aInitialSize)
    : theQueue{aQueue}
{
    theSamples.reserve(aInitialSize);
}

void SampleCollector::collect()
{
    auto myBuffersToRead{theQueue.read_available()};
    size_t myBuffersRead{0};
    while (myBuffersRead != myBuffersToRead)
    {
        const auto &myBuffer{theQueue.front()};
        for (const auto mySample : myBuffer)
        {
            theSamples.push_back(mySample);
        }
        theQueue.pop();
        ++myBuffersRead;
    }
}

void SampleCollector::stop() { theStop = true; }

bool SampleCollector::isStopped() const { return theStop; }

const SampleCollector::SamplesT &SampleCollector::getSamples() const
{
    return theSamples;
}
} // namespace audio
} // namespace matrix