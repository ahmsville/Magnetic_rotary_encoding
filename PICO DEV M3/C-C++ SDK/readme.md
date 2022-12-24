# Pico Dev M3 C/C++ SDK

The board will come with a default firmware loaded, so you should be able to just connect it to a computer and see the output from a serial terminal like putty or arduino's serial monitor. The default firmware is in C/C++ and you can download the UF2 file [**here.**](https://github.com/ahmsville/Magnetic_rotary_encoding/tree/master/PICO%20DEV%20M3/C-C%2B%2B%20SDK/PICO%20DEV%20M3/build) 

To use the C/C++ code for the pico dev m3, you should alredy have your computer setup to program a Raspberry pi pico using the C/C++ SDK, if not, i recommend watching this [video from Learn Embedded Systems](https://www.youtube.com/watch?v=mUF9xjDtFfY) or you can follow the official instructions at [**Raspberry Pi Documentation.**](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html)

This page contains the project folder for the pico dev m3, you can download it and build it yourself, i personally use visual studio code to build my RP2040 projects, and its just a matter of adding the folder to the workspace and allowing the project to configure itself, you can then click on build.
![Captuzvdfdsfsdfre](https://user-images.githubusercontent.com/44074914/209451442-1f71561a-8746-4954-ac7a-1c13731285bf.PNG)


To load the compiled firmware to the pico dev m3, simply copy the generated UF2 file located in the build folder to the board by connecting to the board via the usb type c port while holding the BOOTSEL button at the back of the board, reference the pinout diagram.

## Changing the encoding method used on the board.

Please see the main [Magnetic rotary encoder](https://github.com/ahmsville/Magnetic_rotary_encoding) page for details on the differences between the modes listed below.

To switch modes, simply uncomment one of this lines in the example code

```C++
// #define ENCMODE 1 // BASE RESOLUTION ANALOG ONLY
#define ENCMODE 2 // BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
// #define ENCMODE 3 // EXTENDED RESOLUTION ANALOG ONLY
// #define ENCMODE 4 // EXTENDED RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
// #define ENCMODE 5 // BASE RESOLUTION ANALOG AND DIGITAL
// #define ENCMODE 6 // BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
// #define ENCMODE 7 // EXTENDED RESOLUTION ANALOG AND DIGITAL
// #define ENCMODE 8 // EXTENDED RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
// #define ENCMODE 9 // BASE RESOLUTION DIGITAL ONLY
// #define ENCMODE 10 // BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING
```
