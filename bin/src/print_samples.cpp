#include "AutoSystem.hxx"
#include "Defs.hpp"
#include "Device.hxx"
#include "DirectionSpecificStreamParameters.hxx"
#include "RecorderStream.hpp"
#include "SamplePrinter.hpp"
#include "StreamParameters.hxx"
#include <boost/thread.hpp>

using namespace matrix::audio;

int main(void)
{
    portaudio::AutoSystem autoSys;
    portaudio::System &mySystem{portaudio::System::instance()};

    portaudio::DirectionSpecificStreamParameters myInputStreamParams{
        mySystem.defaultInputDevice(),
        CHANNELS,
        SAMPLE_FORMAT,
        INTERLEAVED,
        mySystem.defaultInputDevice().defaultLowInputLatency(),
        nullptr};
    portaudio::StreamParameters myStreamParams{
        myInputStreamParams,
        portaudio::DirectionSpecificStreamParameters::null(), SAMPLE_RATE,
        FRAMES_PER_BUFFER, STREAM_FLAGS};

    QueueT myQueue{QUEUE_DEPTH};
    Recorder myRecorder{myQueue};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};
    SamplePrinter myPrinter{myQueue};

    boost::thread mySamplePrinterThread(&SamplePrinter::execute, myPrinter);
    myRecorderStream.start();

    while (true)
    {
    }
}
