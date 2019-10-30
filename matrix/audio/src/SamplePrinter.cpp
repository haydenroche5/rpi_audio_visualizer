#include "SamplePrinter.hpp"

#include <iostream>

namespace matrix
{
namespace audio
{
SamplePrinter::SamplePrinter(AudioQueueT &aQueue) : theQueue{aQueue}
{
    theSamplesFileStream.open(theSamplesFileName);
}

SamplePrinter::~SamplePrinter()
{
    theSamplesFileStream.flush();
    if (theSamplesFileStream.is_open())
    {
        theSamplesFileStream.close();
    }
}

void SamplePrinter::print()
{
    auto myBuffersToRead{theQueue.read_available()};
    size_t myBuffersRead{0};
    while (myBuffersRead != myBuffersToRead)
    {
        // Note: So far, I only ever see this print 1.
        // std::cout << "Reading " << myBuffersToRead
        //           << " buffers from the queue.\n";
        const auto &myBuffer{theQueue.front()};
        for (const auto mySample : myBuffer)
        {
            theSamplesFileStream << mySample << "\n";
        }
        theQueue.pop();
        ++myBuffersRead;
    }
}

void SamplePrinter::stop()
{
    theStop = true;
    theSamplesFileStream.flush();
    theSamplesFileStream.close();
}

bool SamplePrinter::isStopped() const { return theStop; }
} // namespace audio
} // namespace matrix
