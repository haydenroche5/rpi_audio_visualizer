#include "led_matrix/led-matrix.h"

#include "audio/Defs.hpp"
#include "audio/RecorderStream.hpp"
#include "audio/melfilters.hpp"
#include "portaudiocpp/AutoSystem.hxx"
#include "portaudiocpp/Device.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/StreamParameters.hxx"

#include "rendering/Defs.hpp"
#include "rendering/FreqBarsRandom.hpp"
#include "rendering/Renderer.hpp"

#include <boost/thread.hpp>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <random>
#include <signal.h>
#include <thread>

using namespace rgb_matrix;
using namespace matrix::rendering;
using namespace matrix::audio;

volatile bool InterruptReceived{false};
static void InterruptHandler(int signo) { InterruptReceived = true; }

std::random_device myRandomDevice;
std::mt19937 myRNG(myRandomDevice());
std::uniform_int_distribution<std::mt19937::result_type> myDist63(0, 63);

template <size_t NUM_BARS, size_t FFT_POINTS_REAL>
std::array<float, NUM_BARS>
dotWithMelFilters(const std::array<float, FFT_POINTS_REAL> &aPeriodogram)
{
    std::array<float, NUM_BARS> mySums{};

    for (size_t i{0}; i < NUM_BARS; ++i)
    {
        const auto &myFilter{MEL_FILTERS[i]};
        float mySum{0};

        for (size_t j{0}; j < FFT_POINTS_REAL; ++j)
        {
            mySum += myFilter[j] * aPeriodogram[j];
        }

        mySums[i] = mySum;
    }

    return mySums;
}

template <size_t NUM_BARS>
void genRandomPositions(AudioQueueT &aAudioQueue,
                        FreqBarsUpdateQueueT<NUM_BARS> &aBarUpdateQueue)
{
    FftInputArrayT myFftInput{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    std::array<float, FFT_POINTS_REAL> myPeriodogram{};

    while (true)
    {
        /************/
        if (aAudioQueue.read_available() > 0)
        {
            const auto &myBuffer{aAudioQueue.front()};
            for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
            {
                myFftInput[i] =
                    myBuffer[i] * 0.5 *
                    (1 - std::cos((2 * M_PI * i) /
                                  SAMPLES_PER_BUFFER)); // Hann window
            }

            aAudioQueue.pop();

            fftwpp::rcfft1d myFft(SAMPLES_PER_BUFFER, myFftInput, myFftOutput);
            myFft.fft(myFftInput, myFftOutput);

            for (size_t i{0}; i < FFT_POINTS_REAL; ++i)
            {
                auto myPower{std::pow(std::abs(myFftOutput[i]), 2) /
                             FFT_POINTS_REAL};
                myPeriodogram[i] = myPower;
            }

            auto myMelFilteredValues{
                dotWithMelFilters<NUM_BARS, FFT_POINTS_REAL>(myPeriodogram)};

            auto myMinMel{*std::min_element(myMelFilteredValues.begin(),
                                            myMelFilteredValues.end())};
            auto myMaxMel{*std::max_element(myMelFilteredValues.begin(),
                                            myMelFilteredValues.end())};

            FreqBarPositionsT<NUM_BARS> myNewPositions{};
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                auto myScaledMel{(myMelFilteredValues[i] - myMinMel) /
                                 (myMaxMel - myMinMel)};
                auto myNewPosition{std::round((NUM_ROWS - 1) * myScaledMel)};
                myNewPositions[i] = myNewPosition;
            }

            if (aBarUpdateQueue.empty())
            {
                aBarUpdateQueue.push(myNewPositions);
            }
        }
    }
}

template <typename FreqBarsT> void animate(FreqBarsT &aFreqBars)
{
    while (true)
    {
        aFreqBars.animate();
    }
}

BufferT generateSampledSineWave(double aFrequency)
{
    BufferT mySamples{};
    for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
    {
        mySamples[i] = std::sin(2 * M_PI * aFrequency * SAMPLE_PERIOD * i);
    }

    return mySamples;
}

int main(int argc, char *argv[])
{
    rgb_matrix::RGBMatrix::Options myMatrixOptions;
    rgb_matrix::RuntimeOptions myRuntimeOptions;

    // These are the defaults when no command-line flags are given.
    myMatrixOptions.rows = 32;
    myMatrixOptions.chain_length = 1;
    myMatrixOptions.parallel = 1;

    // First things first: extract the command line flags that contain
    // relevant matrix options.
    if (!ParseOptionsFromFlags(&argc, &argv, &myMatrixOptions,
                               &myRuntimeOptions))
    {
        return 1;
    }

    static constexpr size_t NUM_BARS{16};
    static constexpr size_t NUM_COLORS_PER_GRADIENT{8};
    static constexpr size_t ANIMATION_SPEED{4};
    static constexpr size_t MAX_ANIMATION_FRAMES{
        NUM_ROWS / ANIMATION_SPEED}; // TODO: duplicated in FreqBarsRandom

    FreqBarsUpdateQueueT<NUM_BARS> myBarUpdateQueue{
        FREQ_BAR_UPDATE_QUEUE_DEPTH};
    using FreqBarsT =
        FreqBarsRandom<NUM_BARS, NUM_COLORS_PER_GRADIENT, ANIMATION_SPEED>;
    FreqBarsT myFreqBars{myMatrixOptions, myRuntimeOptions, myBarUpdateQueue};

    boost::thread myFreqBarsThread(animate<FreqBarsT>, std::ref(myFreqBars));

    /****************************/
    portaudio::AutoSystem autoSys; // TODO: Needed?
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

    AudioQueueT myAudioQueue{AUDIO_QUEUE_DEPTH};
    constexpr bool IS_STREAMING_MODE{true};
    Recorder myRecorder{myAudioQueue, IS_STREAMING_MODE};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};

    /****************************/

    boost::thread myRandomPosGenThread(genRandomPositions<NUM_BARS>,
                                       std::ref(myAudioQueue),
                                       std::ref(myBarUpdateQueue));

    myRecorderStream.start();

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    while (!InterruptReceived)
    {
        // myAudioQueue.push(generateSampledSineWave(1000));
        // boost::this_thread::sleep_for(boost::chrono::seconds{5});
    }

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}
