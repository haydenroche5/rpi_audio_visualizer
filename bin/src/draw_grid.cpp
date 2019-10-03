#include "dependencies/led_matrix/include/led-matrix.h"

#include "matrix/include/rendering/Scene.hpp"

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
using namespace matrix::shapes;

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
    std::unique_ptr<Scene> myScene{new Scene(myCanvas)};

    signal(SIGTERM, InterruptHandler);
    signal(SIGINT, InterruptHandler);

    Shape::PixelsT myGridPixels{};
    for (int myX{0}; myX < myCanvas->height(); ++myX)
    {
        for (int myY{0}; myY < myCanvas->width(); ++myY)
        {
            if (myX % 2 && myY % 2)
            {
                myGridPixels.emplace_back(myX, myY, Color{255, 0, 0});
            }
        }
    }
    myScene->addShape(myGridPixels);

    myScene->Start(); // Start doing things.

    printf("Press <CTRL-C> to exit and reset LEDs\n");
    while (!InterruptReceived)
    {
        sleep(
            1); // Time doesn't really matter. The syscall will be interrupted.
    }

    delete myCanvas;

    printf("\%s. Exiting.\n",
           InterruptReceived ? "Received CTRL-C" : "Timeout reached");
    return 0;
}