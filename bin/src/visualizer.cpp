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

constexpr float MIN_MAGNITUDE{0};
constexpr float MAX_MAGNITUDE{20};

static constexpr size_t NUM_BARS{16};
static constexpr size_t NUM_COLORS_PER_GRADIENT{8};
static constexpr size_t ANIMATION_SPEED{4};
static constexpr size_t MAX_ANIMATION_FRAMES{
    NUM_ROWS / ANIMATION_SPEED}; // TODO: duplicated in Visualizer
// static constexpr float PRE_EMPHASIS_FACTOR{0.9};

std::atomic<bool> Terminate{false};

std::array<float, NUM_BARS> OCTAVE_BOUNDARIES{
    18.581,  26.278,  37.163,  52.556,  74.325,   105.112,  148.651,  210.224,
    297.302, 420.448, 594.604, 840.896, 1189.207, 1681.793, 2378.414, 3363.586};

template <size_t NUM_BARS>
void updatePositions(AudioQueueT &aAudioQueue,
                     VisualizerUpdateQueueT<NUM_BARS> &aBarUpdateQueue)
{
    FftInputArrayT myFftInput{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    std::array<float, FFT_POINTS_REAL> myPeriodogram{};

    VisualizerBarPositionsT<NUM_BARS> myOldPositions{};

    while (!Terminate)
    {
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

            size_t myOctaveBoundaryIdx{0};
            std::array<float, NUM_BARS> myMaxMagnitudePerOctave{};
            float myMaxMagnitude{0};

            for (size_t i{0}; i < FFT_POINTS_REAL; ++i)
            {
                auto myFrequency{i * FFT_FUNDAMENTAL_FREQ};

                if (myFrequency >= OCTAVE_BOUNDARIES[myOctaveBoundaryIdx])
                {
                    if (myOctaveBoundaryIdx == NUM_BARS - 1)
                    {
                        break;
                    }

                    myMaxMagnitudePerOctave[myOctaveBoundaryIdx] =
                        myMaxMagnitude;
                    myMaxMagnitude = 0;
                    ++myOctaveBoundaryIdx;
                }

                auto myMagnitude{std::abs(myFftOutput[i])};

                if (myMagnitude > myMaxMagnitude)
                {
                    myMaxMagnitude = std::abs(myFftOutput[i]);
                }
            }

            VisualizerBarPositionsT<NUM_BARS> myNewPositions{};
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                auto myScaledMagnitude{
                    (myMaxMagnitudePerOctave[i] - MIN_MAGNITUDE) /
                    (MAX_MAGNITUDE - MIN_MAGNITUDE)};
                auto myNewPosition{
                    (NUM_ROWS - 1) -
                    std::round((NUM_ROWS - 1) * myScaledMagnitude)};

                myNewPositions[i] = myNewPosition;
            }

            myOldPositions = myNewPositions;

            auto myUpdateSuccess{aBarUpdateQueue.push(myNewPositions)};

            if (!myUpdateSuccess)
            {
                std::cout << "Failed to update bar positions." << std::endl;
            }
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
                             myBarUpdateQueue, true};

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
