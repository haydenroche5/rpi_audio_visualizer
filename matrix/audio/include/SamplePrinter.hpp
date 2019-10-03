#pragma once

#include "Defs.hpp"

namespace matrix
{
namespace audio
{
class SamplePrinter
{
private:
    QueueT &theQueue;

public:
    SamplePrinter(QueueT &aQueue);
    void execute();
};
} // namespace audio
} // namespace matrix