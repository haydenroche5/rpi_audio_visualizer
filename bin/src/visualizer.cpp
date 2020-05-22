#include "led_matrix/led-matrix.h"

#include "audio/Defs.hpp"
#include "audio/RecorderStream.hpp"
#include "audio/melfilters.hpp"
#include "portaudiocpp/AutoSystem.hxx"
#include "portaudiocpp/Device.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/StreamParameters.hxx"

#include "rendering/Defs.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Visualizer.hpp"

#include <boost/thread.hpp>
#include <chrono>
#include <getopt.h>
#include <iostream>
#include <signal.h>
#include <thread> // TODO: Needed?

using namespace rgb_matrix;
using namespace matrix::rendering;
using namespace matrix::audio;

volatile bool InterruptReceived{false};
static void InterruptHandler(int signo) { InterruptReceived = true; }

float MaxDbSeen{0};
float MinDbSeen{1000};

constexpr float MIN_DB{-128};
constexpr float MAX_DB{50};

static constexpr size_t NUM_BARS{16};
static constexpr size_t NUM_COLORS_PER_GRADIENT{8};
static constexpr size_t ANIMATION_SPEED{2};
static constexpr size_t MAX_ANIMATION_FRAMES{
    NUM_ROWS / ANIMATION_SPEED}; // TODO: duplicated in Visualizer
// static constexpr float PRE_EMPHASIS_FACTOR{0.9};

std::atomic<bool> Terminate{false};

template <size_t NUM_BARS>
void updatePositions(AudioQueueT &aAudioQueue,
                     VisualizerUpdateQueueT<NUM_BARS> &aBarUpdateQueue)
{
    FftInputArrayT myFftInput{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    std::array<float, FFT_POINTS_REAL> myPeriodogram{};

    while (!Terminate)
    {
        if (aAudioQueue.read_available() > 0)
        {
            const auto &myBuffer{aAudioQueue.front()};
            // BufferT myPreEmphasizedBuffer{};

            // myPreEmphasizedBuffer[0] = myBuffer[0];
            // for (size_t i{1}; i < SAMPLES_PER_BUFFER; ++i)
            // {
            //     myPreEmphasizedBuffer[i] =
            //         myBuffer[i] - PRE_EMPHASIS_FACTOR * myBuffer[i - 1];
            // }

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
                             FFT_POINTS};
                myPeriodogram[i] = myPower;
            }

            VisualizerBarPositionsT<NUM_BARS> myNewPositions{};
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                const auto &myFilter{MEL_FILTERS[i]};
                float mySum{0};

                for (size_t j{0}; j < FFT_POINTS_REAL; ++j)
                {
                    mySum += myFilter[j] * myPeriodogram[j];
                }

                auto myDb{20 * std::log10(mySum)};

                // if (myDb > MaxDbSeen)
                // {
                //     MaxDbSeen = myDb;
                //     std::cout << "MAX db seen: " << MaxDbSeen << std::endl;
                // }
                // if (myDb < MinDbSeen)
                // {
                //     MinDbSeen = myDb;
                //     std::cout << "MIN db seen: " << MinDbSeen << std::endl;
                // }

                auto myScaledDb{(myDb - MIN_DB) / (MAX_DB - MIN_DB)};
                auto myNewPosition{(NUM_ROWS - 1) -
                                   std::round((NUM_ROWS - 1) * myScaledDb)};

                myNewPositions[i] = myNewPosition;
            }

            aBarUpdateQueue.push(myNewPositions);
        }
    }
}

template <typename VisualizerT> void animate(VisualizerT &aVisualizer)
{
    while (!Terminate)
    {
        aVisualizer.animate();
    }
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

    VisualizerUpdateQueueT<NUM_BARS> myBarUpdateQueue{
        FREQ_BAR_UPDATE_QUEUE_DEPTH};
    using VisualizerT =
        Visualizer<NUM_BARS, NUM_COLORS_PER_GRADIENT, ANIMATION_SPEED>;
    VisualizerT myVisualizer{myMatrixOptions, myRuntimeOptions,
                             myBarUpdateQueue};

    boost::thread myVisualizerThread(animate<VisualizerT>,
                                     std::ref(myVisualizer));

    portaudio::AutoSystem myAutoSystem; // TODO: What does this do?
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

    boost::thread myPositionUpdateThread(updatePositions<NUM_BARS>,
                                         std::ref(myAudioQueue),
                                         std::ref(myBarUpdateQueue));

    myRecorderStream.start();

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    std::cout << "Press <CTRL-C> to exit and reset LEDs" << std::endl;

    while (!InterruptReceived)
    {
        boost::this_thread::sleep_for(boost::chrono::seconds{2});
    }

    std::cout << "Done." << std::endl;
    std::cout.flush();

    Terminate = true;

    myVisualizerThread.join();
    myPositionUpdateThread.join();

    return 0;
}
