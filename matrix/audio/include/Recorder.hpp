#pragma once

#include "CallbackInterface.hxx"
#include "Defs.hpp"

namespace matrix
{
namespace audio
{
class Recorder : public portaudio::CallbackInterface
{
public:
    Recorder(QueueT &aQueue);

private:
    QueueT &theQueue;
    BufferT theCurrentBuffer{};

    int paCallbackFun(const void *aInputBuffer, void *, unsigned long,
                      const PaStreamCallbackTimeInfo *,
                      PaStreamCallbackFlags) override;
};
} // namespace audio
} // namespace matrix