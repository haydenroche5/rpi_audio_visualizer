#include "audio/Defs.hpp"
#include "audio/RecorderStream.hpp"
#include "audio/SampleCollector.hpp"
#include "portaudiocpp/AutoSystem.hxx"
#include "portaudiocpp/Device.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/StreamParameters.hxx"
#include <boost/thread.hpp>
#include <functional>

using namespace matrix::audio;

void collectSamples(SampleCollector &aCollector)
{
    while (true)
    {
        if (aCollector.isStopped())
        {
            return;
        }

        aCollector.collect();
    }
}

BufferT generateSampledSineWave(uint32_t aSamples, double aFrequency)
{
    BufferT mySamples(aSamples, 0);
    for (size_t i{0}; i < aSamples; ++i)
    {
        mySamples[i] = std::sin(2 * M_PI * aFrequency * SAMPLE_PERIOD * i);
    }

    return mySamples;
}

BufferT hannWindow(const BufferT &aSamples)
{
    BufferT myWindowedSamples{};
    for (size_t i{0}; i < aSamples.size(); ++i)
    {
        auto myWindowedSample{aSamples[i] * 0.5 *
                              (1 - std::cos((2 * M_PI * i) / aSamples.size()))};
        myWindowedSamples.push_back(myWindowedSample);
    }

    return myWindowedSamples;
}

int main(void)
{
    portaudio::AutoSystem autoSys;
    portaudio::System &mySystem{portaudio::System::instance()};

    static constexpr size_t BUFFER_SIZE{1024};

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
        BUFFER_SIZE, STREAM_FLAGS};

    AudioQueueT myQueue{AUDIO_QUEUE_DEPTH};
    constexpr bool IS_STREAMING_MODE{false};
    constexpr size_t SECONDS_TO_RECORD{3};
    constexpr size_t SAMPLES_TO_RECORD{static_cast<size_t>(
        std::ceil((SECONDS_TO_RECORD * SAMPLE_RATE * CHANNELS) /
                  static_cast<float>(BUFFER_SIZE)) *
        BUFFER_SIZE)};
    Recorder myRecorder{myQueue, BUFFER_SIZE, IS_STREAMING_MODE,
                        SECONDS_TO_RECORD};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};
    SampleCollector myCollector{myQueue, SAMPLES_TO_RECORD};

    boost::thread mySampleCollectingThread(collectSamples,
                                           std::ref(myCollector));
    myRecorderStream.start();

    while (true)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if (myRecorder.isDoneRecording())
        {
            myRecorderStream.stop();
            myCollector.stop();
            break;
        }
    }

    mySampleCollectingThread.join();

    constexpr bool IS_WINDOW_MODE{false};
    const auto &myCollectedSamples{myCollector.getSamples()};
    constexpr size_t FFT_POINTS{SAMPLES_TO_RECORD};
    constexpr size_t FFT_POINTS_REAL{SAMPLES_TO_RECORD / 2 + 1};
    constexpr auto FUNDAMENTAL_FREQUENCY{1.0 * SAMPLE_RATE / FFT_POINTS};

    std::array<double, FFT_POINTS> myFftCenterFreqs{};
    for (size_t i{0}; i < FFT_POINTS_REAL; ++i)
    {
        myFftCenterFreqs[i] = FUNDAMENTAL_FREQUENCY * i;
    }

    BufferT myWindowedSamples{};
    if (IS_WINDOW_MODE)
    {
        myWindowedSamples = hannWindow(myCollectedSamples);
    }

    FftInputArrayT myFftInput{SAMPLES_TO_RECORD, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    for (size_t i{0}; i < SAMPLES_TO_RECORD; ++i)
    {
        if constexpr (IS_WINDOW_MODE)
        {
            myFftInput[i] = myWindowedSamples[i];
        }
        else
        {
            myFftInput[i] = myCollectedSamples[i];
        }
    }

    fftwpp::rcfft1d myFft(SAMPLES_TO_RECORD, myFftInput, myFftOutput);
    myFft.fft(myFftInput, myFftOutput);

    const char *mySamplesFileName{"samples.csv"};
    std::ofstream mySamplesFileStream;
    mySamplesFileStream.open(mySamplesFileName);
    mySamplesFileStream << "Time,Amplitude\n";
    for (size_t i{0}; i < SAMPLES_TO_RECORD; ++i)
    {
        mySamplesFileStream << i << "," << myFftInput[i] << "\n";
    }
    mySamplesFileStream.close();

    const char *theMagsFileName{"fft.csv"};
    std::ofstream theMagsFileStream;
    theMagsFileStream.open(theMagsFileName);
    theMagsFileStream << "Frequency,Magnitude\n";
    for (size_t i{0}; i < FFT_POINTS_REAL; ++i)
    {
        theMagsFileStream << myFftCenterFreqs[i] << ","
                          << std::abs(myFftOutput[i]) << "\n";
    }
    theMagsFileStream.close();
}
