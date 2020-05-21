#include "AutoSystem.hxx"
#include "Defs.hpp"
#include "Device.hxx"
#include "DirectionSpecificStreamParameters.hxx"
#include "RecorderStream.hpp"
#include "SampleCollector.hpp"
#include "StreamParameters.hxx"
#include <boost/thread.hpp>
#include <functional>

using namespace matrix::audio;
using namespace matrix::rendering;

using FreqsAndMagsT = std::array<FreqAndMag, FFT_POINTS_REAL>;

void hannWindow(BufferT &aBuffer)
{
    for (size_t i{0}; i < aBuffer.size(); ++i)
    {
        aBuffer[i] =
            aBuffer[i] * 0.5 * (1 - std::cos((2 * M_PI * i) / aBuffer.size()));
    }
}

FreqsAndMagsT calculateFftMags(const BufferT &aBuffer)
{
    SamplesArrayT myFftSamples{SAMPLES_PER_BUFFER, FFT_ALIGNMENT};
    FftOutputArrayT myFftOutput{FFT_POINTS_REAL, FFT_ALIGNMENT};
    for (size_t i{0}; i < aBuffer.size(); ++i)
    {
        myFftSamples[i] = aBuffer[i];
    }
    fftwpp::rcfft1d myFft(SAMPLES_PER_BUFFER, myFftSamples, myFftOutput);
    myFft.fft(myFftSamples, myFftOutput);

    FreqsAndMagsT myFreqsAndMags{};
    for (size_t i{0}; i < myFreqsAndMags.size(); ++i)
    {
        myFreqsAndMags[i] = {i * FFT_FUNDAMENTAL_FREQ,
                             std::abs(myFftOutput[i])};
    }

    return myFreqsAndMags;
}

template <size_t NUM_BARS>
class Renderer : public rgb_matrix::ThreadedCanvasManipulator
{
private:
    constexpr size_t NUM_COLS{64};
    constexpr size_t NUM_ROWS{64};
    constexpr size_t BAR_WIDTH{NUM_COLS / NUM_BARS};

    using RowT = std::array<Color, NUM_COLS>;
    using GridT = std::array<RowT, NUM_ROWS>;

    const std::array<Color, NUM_BARS> theBarColors;
    BarQueueT &theQueue;
    GridT theGrid{};

public:
    Renderer(rgb_matrix::Canvas *aCanvas,
             const std::array<Color, NUM_BARS> &aBarColors)
        : rgb_matrix::ThreadedCanvasManipulator(aCanvas), theBarColors{
                                                              aBarColors}
    {
    }

    void Run()
    {
        while (running())
        {
            if (theQueue.read_available() > 0)
            {
                const auto myMags{theQueue.front()};
                for (size_t i{0}; i < NUM_BARS; ++i)
                {
                    size_t myStartingX{myMags[i].first * BAR_WIDTH};
                    for (size_t j{0}; j < NUM_ROWS; ++j)
                    {
                        for (size_t k{0}; k < BAR_WIDTH; ++k)
                        {
                            theGrid[j][myStartingX + k] = theBarColors[i];
                        }
                    }
                }
            }

            for (size_t i{0}; i < NUM_ROWS; ++i)
            {
                for (size_t j{0}; j < NUM_COLS; ++j)
                {
                    canvas()->SetPixel(j, i, theGrid[i][j].getRed(),
                                       theGrid[i][j].getGreen(),
                                       theGrid[i][j].getBlue());
                }
            }
        }
    }
};

int main(void)
{
    RGBMatrix::Options myMatrixOptions;
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

    RGBMatrix *myMatrix{
        CreateMatrixFromOptions(myMatrixOptions, myRuntimeOptions)};

    if (myMatrix == NULL)
        return 1;

    constexpr std::array<Color, NUM_BARS> BAR_COLORS{
        {240, 68, 135}, {225, 84, 124}, {210, 99, 113}, {195, 115, 101},
        {179, 130, 90}, {164, 146, 79}, {149, 161, 68}, {134, 177, 56},
        {119, 193, 45}, {104, 208, 34}, {88, 224, 23},  {73, 239, 11},
        {58, 255, 0},   {58, 255, 0},   {58, 255, 0},   {58, 255, 0}};
    constexpr double NUM_BARS{16};
    Renderer<NUM_BARS> myRenderer{&myMatrix, BAR_COLORS};

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

    AudioQueueT myAudioQueue{AUDIO_QUEUE_DEPTH};
    constexpr bool IS_STREAMING_MODE{true};
    Recorder myRecorder{myAudioQueue, IS_STREAMING_MODE};
    RecorderStream myRecorderStream{myRecorder, myStreamParams};

    BarQueueT myBarQueue{BAR_QUEUE_DEPTH};

    myRecorderStream.start();
    myRenderer.Start();

    constexpr double FREQS_PER_BAR{std::ceil(HUMAN_HEARING_LIMIT / NUM_BARS)};

    while (true)
    {
        if (myAudioQueue.read_available() < 1) // TODO: Is this ever > 1?
        {
            continue; // TODO: sleep?
        }

        BufferT myCurrentBuffer{};
        auto &myQueueBuffer{myAudioQueue.front()};
        for (size_t i{0}; i < SAMPLES_PER_BUFFER; ++i)
        {
            myCurrentBuffer[i] = myQueueBuffer[i];
        }
        myAudioQueue.pop();

        hannWindow(myCurrentBuffer);
        auto myFreqsAndMags{calculateFftMags(myCurrentBuffer)};

        std::array<std::pair<size_t, double>, NUM_BARS> myAverageMagnitudes{};
        const auto myCurrentFreqAndMagIt{myFreqsAndMags.cbegin()};
        for (size_t i{0}; i < NUM_BARS; ++i)
        {
            myAverageMagnitudes[i] = std::make_pair(
                i, std::accumulate(myCurrentFreqAndMagIt,
                                   myCurrentFreqAndMagIt + FREQS_PER_BAR) /
                       NUM_BARS);
            myCurrentFreqAndMagIt += NUM_BARS;
        }

        std::sort(myAverageMagnitudes.begin(), myAverageMagnitudes.end(),
                  [](std::pair<size_t, double> a, std::pair<size_t, double> b) {
                      return a.second > b.second;
                  });

        myBarQueue.push(myAverageMagnitudes);
    }
}
