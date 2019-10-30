#include "Array.h"
#include "AutoSystem.hxx"
#include "Defs.hpp"
#include "Device.hxx"
#include "DirectionSpecificStreamParameters.hxx"
#include "FftMagnitudePrinter.hpp"
#include "RecorderStream.hpp"
#include "StreamParameters.hxx"
#include "fftw++.h"
#include <boost/thread.hpp>

using namespace matrix::audio;

void printSamples(FftMagnitudePrinter &aPrinter)
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

    QueueT myQueue{AUDIO_QUEUE_DEPTH};
    bool myIsStreamingMode{false};
    size_t mySecondsToRecord{1};
    Recorder myRecorder{myQueue, myIsStreamingMode, mySecondsToRecord};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};
    FftMagnitudePrinter myPrinter{myQueue};

    boost::thread myFftMagPrintingThread(printSamples, std::ref(myPrinter));
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

    myFftMagPrintingThread.join();
}
