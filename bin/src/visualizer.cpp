#include "led_matrix/led-matrix.h"

#include "portaudiocpp/AutoSystem.hxx"
#include "portaudiocpp/Device.hxx"
#include "portaudiocpp/DirectionSpecificStreamParameters.hxx"
#include "portaudiocpp/StreamParameters.hxx"

#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <getopt.h>
#include <iostream>
#include <stdexcept>

#include "audio/Defs.hpp"
#include "audio/DspPipeline.hpp"
#include "audio/RecorderStream.hpp"

#include "rendering/Defs.hpp"
#include "rendering/Visualizer.hpp"

using namespace rgb_matrix;
using namespace matrix::rendering;
using namespace matrix::audio;
namespace po = boost::program_options;

std::atomic<bool> Terminate{false};

float calibrate(AudioQueueT &aAudioQueue, size_t aBufferSize,
                float aPreEmphasisFactor, size_t aCalibrationSeconds)
{
    auto mySamplesToRecord{
        static_cast<size_t>(std::ceil((aCalibrationSeconds * SAMPLE_RATE) /
                                      static_cast<float>(aBufferSize)) *
                            aBufferSize)};
    size_t mySamplesRecorded{0};
    DspPipeline<NUM_BARS> myDspPipeline{aBufferSize, aPreEmphasisFactor};
    float myMaxMagnitude{0};

    while (mySamplesRecorded < mySamplesToRecord)
    {
        if (aAudioQueue.read_available() > 0)
        {
            mySamplesRecorded += aBufferSize;
            auto myMaxMagnitudePerOctave{myDspPipeline(aAudioQueue.front())};
            aAudioQueue.pop();

            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                if (myMaxMagnitudePerOctave[i] > myMaxMagnitude)
                {
                    myMaxMagnitude = myMaxMagnitudePerOctave[i];
                }
            }
        }
    }

    return myMaxMagnitude;
}

void updatePositions(AudioQueueT &aAudioQueue, size_t aBufferSize,
                     float aPreEmphasisFactor, float aMaxMagnitude,
                     VisualizerUpdateQueueT &aBarUpdateQueue)
{
    VisualizerBarPositionsT myNewPositions{};

    DspPipeline<NUM_BARS> myDspPipeline{aBufferSize, aPreEmphasisFactor};

    while (!Terminate)
    {
        if (aAudioQueue.read_available() > 0)
        {
            auto myMaxMagnitudePerOctave{myDspPipeline(aAudioQueue.front())};
            aAudioQueue.pop();

            // Compute the position of each bar. Each bar corresponds to one
            // octave. The height of the bar is directly proportional to the
            // maximum FFT magnitude in the octave.
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                auto myScaledMagnitude{myMaxMagnitudePerOctave[i] /
                                       aMaxMagnitude};
                auto myNewPosition{
                    (NUM_ROWS - 1) -
                    std::round((NUM_ROWS - 1) * myScaledMagnitude)};

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

void validateCommandLineArgs(const po::variables_map &aVariablesMap)
{
    auto mySpeed{aVariablesMap["speed"].as<int>()};

    if (mySpeed == 0)
    {
        throw std::invalid_argument{"Speed must be > 0."};
    }

    auto mySpeedIsPowerOfTwo{(mySpeed & (mySpeed - 1)) == 0};

    if (!mySpeedIsPowerOfTwo)
    {
        throw std::invalid_argument{"Speed must be a power of 2."};
    }

    static constexpr std::array<size_t, 4> ALLOWED_BUFFER_SIZES{256, 512, 1024,
                                                                2048};
    auto myBufferSize{aVariablesMap["buffer-size"].as<size_t>()};

    if (std::find(ALLOWED_BUFFER_SIZES.begin(), ALLOWED_BUFFER_SIZES.end(),
                  myBufferSize) == ALLOWED_BUFFER_SIZES.end())
    {
        throw std::invalid_argument{
            "Buffer size must be one of 256, 512, 1024, or 2048."};
    }

    auto myPreEmphasisFactor{aVariablesMap["pre-emphasis"].as<float>()};

    if (myPreEmphasisFactor < 0.0 || myPreEmphasisFactor > 1.0)
    {
        throw std::invalid_argument{
            "Pre-emphasis factor must be in the range [0, 1]."};
    }
}

int main(int argc, char *argv[])
{
    po::options_description myOptionsDesc("Allowed options");
    myOptionsDesc.add_options()("help", "produce help message")(
        "speed", po::value<int>()->default_value(8),
        "set the visualizer's animation speed "
        "(must be a power of 2 in range [2, 64])")(
        "buffer-size", po::value<size_t>()->default_value(512),
        "set the size of the audio buffer, which also determines the FFT "
        "length (allowed values: 256, 512, 1024, 2048)")(
        "pre-emphasis", po::value<float>()->default_value(0.9),
        "set the pre-emphasis factor");

    po::variables_map myVariablesMap;
    po::store(po::parse_command_line(argc, argv, myOptionsDesc),
              myVariablesMap);
    po::notify(myVariablesMap);

    if (myVariablesMap.count("help"))
    {
        std::cout << myOptionsDesc << std::endl;
        return 1;
    }

    validateCommandLineArgs(myVariablesMap);

    auto myBufferSize{myVariablesMap["buffer-size"].as<size_t>()};
    auto myAnimationSpeed{myVariablesMap["speed"].as<int>()};
    auto myPreEmphasisFactor{myVariablesMap["pre-emphasis"].as<float>()};

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
        myBufferSize, STREAM_FLAGS};

    AudioQueueT myAudioQueue{AUDIO_QUEUE_DEPTH};
    constexpr bool IS_STREAMING_MODE{true};
    constexpr size_t CALIBRATION_SECONDS{5};
    Recorder myRecorder{myAudioQueue, myBufferSize, IS_STREAMING_MODE};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};

    myRecorderStream.start();

    std::cout << "Calibrating..." << std::endl;
    auto myMaxMagnitude{calibrate(myAudioQueue, myBufferSize,
                                  myPreEmphasisFactor, CALIBRATION_SECONDS)};
    std::cout << "Calibration done. Starting visualizer." << std::endl;

    VisualizerUpdateQueueT myBarUpdateQueue{FREQ_BAR_UPDATE_QUEUE_DEPTH};
    using VisualizerT = Visualizer<NUM_BARS>;
    constexpr bool ENABLE_PROFILING{false};

    rgb_matrix::RGBMatrix::Options myMatrixOptions{};
    myMatrixOptions.hardware_mapping = "adafruit-hat-pwm";
    myMatrixOptions.rows = NUM_ROWS;
    myMatrixOptions.cols = NUM_COLS;
    rgb_matrix::RuntimeOptions myRuntimeOptions{};

    VisualizerT myVisualizer{myMatrixOptions, myRuntimeOptions,
                             myBarUpdateQueue, myAnimationSpeed,
                             ENABLE_PROFILING};

    boost::thread myVisualizerThread(animate<VisualizerT>,
                                     std::ref(myVisualizer));
    boost::thread myPositionUpdateThread(
        updatePositions, std::ref(myAudioQueue), myBufferSize,
        myPreEmphasisFactor, myMaxMagnitude, std::ref(myBarUpdateQueue));

    std::cout << "Press 'q' to exit." << std::endl;

    while (!Terminate)
    {
        auto myInputChar{std::cin.get()};

        if (myInputChar == 'q')
        {
            Terminate = true;
        }
    }

    myVisualizerThread.join();
    myPositionUpdateThread.join();

    return 0;
}
