#include "led_matrix/led-matrix.h"

#include "rendering/GradientCycler.hpp"
#include "rendering/Renderer.hpp"

#include <chrono>
#include <getopt.h>
#include <signal.h>
#include <thread>

using namespace rgb_matrix;
using namespace matrix::rendering;

volatile bool InterruptReceived{false};
static void InterruptHandler(int signo) { InterruptReceived = true; }

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

    Renderer<GradientCycler> myRenderer{myMatrixOptions, myRuntimeOptions};

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    while (!InterruptReceived)
    {
        myRenderer.renderNextFrame();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}