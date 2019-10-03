#include "SamplePrinter.hpp"

#include <iostream>

namespace matrix
{
namespace audio
{
SamplePrinter::SamplePrinter(QueueT &aQueue) : theQueue{aQueue} {}

void SamplePrinter::execute()
{
    while (true)
    {
        while (theQueue.read_available() > 0)
        {
            const auto &myBuffer{theQueue.front()};
            for (const auto mySample : myBuffer)
            {
                std::cout << mySample << "\n";
            }
            theQueue.pop();
        }
    }
}
} // namespace audio
} // namespace matrix
