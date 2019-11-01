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

static bool STARTED{false};

template <size_t NUM_BARS>
void genRandomPositions(FreqBarsUpdateQueueT<NUM_BARS> &aQueue)
{
    while (true)
    {
        if (STARTED)
        {
            // auto t1 = std::chrono::high_resolution_clock::now();

            FreqBarPositionsT<NUM_BARS> myNewPositions{};
            for (size_t i{0}; i < NUM_BARS; ++i)
            {
                myNewPositions[i] = myDist63(myRNG);
            }
            aQueue.push(myNewPositions);
            boost::this_thread::sleep(boost::posix_time::milliseconds(8000));
            // auto t2 = std::chrono::high_resolution_clock::now();
            // auto duration =
            //     std::chrono::duration_cast<std::chrono::microseconds>(t2 -
            //     t1)
            //         .count();
            // std::cout << "Producer waited " << duration
            //           << " microseconds before pushing the next update"
            //           << std::endl;
        }
    }
}

template <typename DrawerT> void animate(DrawerT &aDrawer)
{
    while (true)
    {
        aDrawer.animate();
        boost::this_thread::sleep(boost::posix_time::milliseconds(100));
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
    static constexpr size_t ANIMATION_SPEED{1};
    static constexpr size_t MAX_ANIMATION_FRAMES{
        NUM_ROWS / ANIMATION_SPEED}; // TODO: duplicated in FreqBarsRandom

    FreqBarsUpdateQueueT<NUM_BARS> myQueue{FREQ_BAR_UPDATE_QUEUE_DEPTH};
    using DrawerT =
        FreqBarsRandom<NUM_BARS, NUM_COLORS_PER_GRADIENT, ANIMATION_SPEED>;
    DrawerT myDrawer{myQueue};
    Renderer<MAX_ANIMATION_FRAMES> myRenderer{myMatrixOptions,
                                              myRuntimeOptions};
    myDrawer.setNextFrameBuffers(myRenderer.getNextFrameBuffers());

    boost::thread myFreqBarsThread(animate<DrawerT>, std::ref(myDrawer));
    boost::thread myRandomPosGenThread(genRandomPositions<NUM_BARS>,
                                       std::ref(myQueue));

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    STARTED = true;
    while (!InterruptReceived)
    {
        myRenderer.renderNextFrame();
    }

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}
