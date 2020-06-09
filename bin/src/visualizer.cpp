#include "led_matrix/led-matrix.h"

#include "audio/Defs.hpp"
#include "audio/RecorderStream.hpp"
#include "portaudiocpp/AutoSystem.hxx"
#include "portaudiocpp/Device.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/StreamParameters.hxx"

#include "rendering/Defs.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/Visualizer.hpp"

#include <boost/thread.hpp>
#include <getopt.h>
#include <iostream>

using namespace rgb_matrix;
using namespace matrix::rendering;
using namespace matrix::audio;

float MaxDbSeen{0};
float MinDbSeen{1000};

constexpr float MIN_MAGNITUDE{0};
constexpr float MAX_MAGNITUDE{5};
static constexpr size_t NUM_BARS{32};
static constexpr size_t NUM_COLORS_PER_GRADIENT{16};
static constexpr size_t ANIMATION_SPEED{8};
static constexpr float PRE_EMPHASIS_FACTOR{0.97};

static constexpr std::array<float, NUM_BARS> OCTAVE_BOUNDARIES{
    14.1, 17.8, 22.4, 28.2, 35.5, 44.7, 56.2, 70.8, 89.1, 112,  141,
    178,  224,  282,  355,  447,  562,  708,  891,  1122, 1413, 1778,
    2239, 2818, 3548, 4467, 5623, 7079, 9999, 9999, 9999, 9999};

std::atomic<bool> Terminate{false};

template <size_t NUM_BARS>
void updatePositions(AudioQueueT &aAudioQueue,
                     VisualizerUpdateQueueT<NUM_BARS> &aBarUpdateQueue)
{
    BufferT myPreEmphasizedBuffer{};
    FftInputArrayT myFftInput{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    std::array<float, NUM_BARS> myMaxMagnitudePerOctave{};
    VisualizerBarPositionsT<NUM_BARS> myNewPositions{};
    // std::array<SampleT, SAMPLES_PER_BUFFER / 2> myPreviousBuffer{};
    // bool myPreviousBufferExists{false};

    while (!Terminate)
    {
        if (aAudioQueue.read_available() > 0)
        {
            const auto &myCurrentBuffer{aAudioQueue.front()};

            myPreEmphasizedBuffer[0] = myCurrentBuffer[0];

            for (size_t i{1}; i < SAMPLES_PER_BUFFER; ++i)
            {
                myPreEmphasizedBuffer[i] =
                    myCurrentBuffer[i] -
                    PRE_EMPHASIS_FACTOR * myCurrentBuffer[i - 1];
            }

            // Done with the original buffer, can pop.
            aAudioQueue.pop();

            // Window samples with a Hann window.
            for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
            {
                myFftInput[i] =
                    myPreEmphasizedBuffer[i] * 0.5 *
                    (1 - std::cos((2 * M_PI * i) / SAMPLES_PER_BUFFER));
            }

            // // Overlap with the latter half of the previous buffer
            // if (myPreviousBufferExists)
            // {
            //     std::copy(myPreviousBuffer.begin(), myPreviousBuffer.end(),
            //               myOverlappedBuffer.begin());
            //     std::copy(myCurrentBuffer.begin(),
            //               myCurrentBuffer.begin() + SAMPLES_PER_BUFFER / 2,
            //               myOverlappedBuffer.begin() + SAMPLES_PER_BUFFER /
            //               2);
            // }
            // else
            // {
            //     myOverlappedBuffer = myCurrentBuffer;
            //     myPreviousBufferExists = true;
            // }
            // std::copy(myCurrentBuffer.begin() + SAMPLES_PER_BUFFER / 2,
            //           myCurrentBuffer.end(), myPreviousBuffer.begin());

            // Take the Fourier transform of the samples.
            fftwpp::rcfft1d myFft(SAMPLES_PER_BUFFER, myFftInput, myFftOutput);
            myFft.fft(myFftInput, myFftOutput);

            // Compute the max FFT magnitude in each octave band.
            size_t myOctaveBoundaryIdx{0};
            float myMaxMagnitude{0};

            for (size_t i{0}; i < FFT_POINTS_REAL; ++i)
            {
                auto myFrequency{i * FFT_FUNDAMENTAL_FREQ};

                // TODO: Try some measure of the power in each octave instead of
                // max magnitude
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

            // Compute the position of each bar. Each bar corresponds to one
            // octave. The height of the bar is directly proportional to the
            // maximum FFT magnitude in the octave.
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
    portaudio::AutoSystem autoSys;
    portaudio::System &mySystem{portaudio::System::instance()};

    int myDeviceIdx{-1};
    for (int i{0}; i < mySystem.deviceCount(); ++i)
    {
        std::string myDeviceName{mySystem.deviceByIndex(i).name()};
        if (myDeviceName.find("USB Camera") != std::string::npos)
        {
            myDeviceIdx = i;
            break;
        }
    }

    if (myDeviceIdx == -1)
    {
        std::cout << "Device not found. Exiting." << std::endl;
        return 1;
    }

    const auto &myDevice{mySystem.deviceByIndex(myDeviceIdx)};

    portaudio::DirectionSpecificStreamParameters myInputStreamParams{
        myDevice,
        CHANNELS,
        SAMPLE_FORMAT,
        INTERLEAVED,
        myDevice.defaultLowInputLatency(),
        nullptr};
    portaudio::StreamParameters myStreamParams{
        myInputStreamParams,
        portaudio::DirectionSpecificStreamParameters::null(), SAMPLE_RATE,
        FRAMES_PER_BUFFER, STREAM_FLAGS};

    AudioQueueT myAudioQueue{AUDIO_QUEUE_DEPTH};
    constexpr bool IS_STREAMING_MODE{true};
    Recorder myRecorder{myAudioQueue, IS_STREAMING_MODE};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};

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
    constexpr bool ENABLE_PROFILING{false};
    VisualizerT myVisualizer{myMatrixOptions, myRuntimeOptions,
                             myBarUpdateQueue, ENABLE_PROFILING};

    boost::thread myVisualizerThread(animate<VisualizerT>,
                                     std::ref(myVisualizer));
    boost::thread myPositionUpdateThread(updatePositions<NUM_BARS>,
                                         std::ref(myAudioQueue),
                                         std::ref(myBarUpdateQueue));

    myRecorderStream.start();

    std::cout << "Press 'q' to exit and reset LEDs" << std::endl;

    while (!Terminate)
    {
        auto myInputChar{std::cin.get()};

        if (myInputChar == 'q')
        {
            Terminate = true;
        }
    }

    std::cout << "Done." << std::endl;

    myVisualizerThread.join();
    myPositionUpdateThread.join();

    return 0;
}
