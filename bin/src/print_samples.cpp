#include "AutoSystem.hxx"
#include "Defs.hpp"
#include "Device.hxx"
#include "DirectionSpecificStreamParameters.hxx"
#include "RecorderStream.hpp"
#include "SamplePrinter.hpp"
#include "StreamParameters.hxx"
#include <boost/thread.hpp>
#include <functional>

using namespace matrix::audio;

void printSamples(SamplePrinter &aPrinter)
{
    while (true)
    {
        if (aPrinter.isStopped())
        {
            return;
        }

        aPrinter.print();
    }
}

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

    AudioQueueT myQueue{AUDIO_QUEUE_DEPTH};
    bool myIsStreamingMode{false};
    size_t mySecondsToRecord{1};
    Recorder myRecorder{myQueue, myIsStreamingMode, mySecondsToRecord};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};
    SamplePrinter myPrinter{myQueue};

    boost::thread mySamplePrintingThread(printSamples, std::ref(myPrinter));
    myRecorderStream.start();

    while (true)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if (myRecorder.isDoneRecording())
        {
            myRecorderStream.stop();
            myPrinter.stop();
            break;
        }
    }

    mySamplePrintingThread.join();
}
