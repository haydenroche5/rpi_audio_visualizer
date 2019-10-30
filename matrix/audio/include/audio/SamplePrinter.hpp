#pragma once

#include "Defs.hpp"
#include <fstream>

namespace matrix
{
namespace audio
{
class SamplePrinter
{
private:
    AudioQueueT &theQueue;
    bool theStop{false};
    const char *theSamplesFileName{"samples.txt"};
    std::ofstream theSamplesFileStream;

public:
    SamplePrinter(AudioQueueT &aQueue);
    ~SamplePrinter();
    void print();
    void stop();
    bool isStopped() const;
};
} // namespace audio
} // namespace matrix