#include "led_matrix/led-matrix.h"

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

volatile bool InterruptReceived{false};
static void InterruptHandler(int signo) { InterruptReceived = true; }

std::random_device myRandomDevice;
std::mt19937 myRNG(myRandomDevice());
std::uniform_int_distribution<std::mt19937::result_type> myDist63(0, 63);

template <size_t NUM_BARS>
void genRandomPositions(FreqBarsUpdateQueueT<NUM_BARS> &aQueue)
{
    while (true)
    {
        FreqBarPositionsT<NUM_BARS> myNewPositions{};
        for (size_t i{0}; i < NUM_BARS; ++i)
        {
            myNewPositions[i] = myDist63(myRNG);
        }
        aQueue.push(myNewPositions);
        boost::this_thread::sleep(boost::posix_time::milliseconds(200));
    }
}

template <typename FreqBarsT> void animate(FreqBarsT &aFreqBars)
{
    while (true)
    {
        aFreqBars.animate();
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

    static constexpr size_t NUM_BARS{16};
    static constexpr size_t NUM_COLORS_PER_GRADIENT{8};
    static constexpr size_t ANIMATION_SPEED{4};
    static constexpr size_t MAX_ANIMATION_FRAMES{
        NUM_ROWS / ANIMATION_SPEED}; // TODO: duplicated in FreqBarsRandom

    FreqBarsUpdateQueueT<NUM_BARS> myQueue{FREQ_BAR_UPDATE_QUEUE_DEPTH};
    using FreqBarsT =
        FreqBarsRandom<NUM_BARS, NUM_COLORS_PER_GRADIENT, ANIMATION_SPEED>;
    FreqBarsT myFreqBars{myMatrixOptions, myRuntimeOptions, myQueue};

    boost::thread myFreqBarsThread(animate<FreqBarsT>, std::ref(myFreqBars));
    boost::thread myRandomPosGenThread(genRandomPositions<NUM_BARS>,
                                       std::ref(myQueue));

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    while (!InterruptReceived)
    {
    }

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}
