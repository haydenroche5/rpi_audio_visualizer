#include "led_matrix/led-matrix.h"

#include "rendering/FrameBufferManager.hpp"
#include "rendering/Renderer.hpp"

#include <assert.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace rgb_matrix;
using namespace matrix::rendering;

volatile bool InterruptReceived{false};
static void InterruptHandler(int signo) { InterruptReceived = true; }

int main(int argc, char *argv[])
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

    printf("Size: %dx%d. Hardware gpio mapping: %s\n", myMatrix->width(),
           myMatrix->height(), myMatrixOptions.hardware_mapping);

    Canvas *myCanvas{myMatrix};
    FrameBufferManager myFrameBufferManager{};
    std::unique_ptr<Renderer> myRenderer{
        new Renderer(myCanvas, myFrameBufferManager.getCurrentFrameBuffer())};

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    myRenderer->Start(); // Start doing things.

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    while (!InterruptReceived)
    {
        myFrameBufferManager.drawNextGradient();
        sleep(
            1); // Time doesn't really matter. The syscall will be interrupted.
        myFrameBufferManager.swapBuffers();
    }

    delete myCanvas;

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}