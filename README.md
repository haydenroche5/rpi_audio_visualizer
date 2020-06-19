# RPi Audio Visualizer
Visualize the spectrum of an audio stream in real time.

## Hardware
- [Raspberry Pi 4](https://www.raspberrypi.org/products/raspberry-pi-4-model-b/)
- [Playstation Eye](https://www.amazon.com/PlayStation-Eye-3/dp/B000VTQ3LU) (only using the mic)
- [64x64 RGB LED Matrix](https://www.adafruit.com/product/3649)
- [Adafruit RGB Matrix Bonnet for Raspberry Pi](https://www.adafruit.com/product/3211)

## Software
- [Boost](https://www.boost.org/)
  - headers
  - [thread](https://www.boost.org/doc/libs/1_73_0/doc/html/thread.html)
  - [chrono](https://www.boost.org/doc/libs/1_73_0/doc/html/chrono.html)
  - [program_options](https://www.boost.org/doc/libs/1_73_0/doc/html/program_options.html)
- [PortAudio](http://portaudio.com/)
- [FFTW](http://www.fftw.org/) and [FFTW++](http://fftwpp.sourceforge.net/)
- [rpi-rgb-led-matrix](https://github.com/hzeller/rpi-rgb-led-matrix)

## Building and running
- Run `cmake .` from the root of the project.
- Change directories to `bin/` and run `make visualizer`.
- Run `build/visualizer` with sudo. Command line options:
```
Allowed options:
  --help                            produce help message
  --speed arg (=8)                  set the visualizer's animation speed (must 
                                    be a power of 2 in range [2, 64])
  --buffer-size arg (=512)          set the size of the audio buffer, which 
                                    also determines the FFT length (allowed 
                                    values: 256, 512, 1024, 2048)
  --pre-emphasis arg (=0.899999976) set the pre-emphasis factor
```
## Examples
Check out `video_examples/` to see the visualizer in action.
