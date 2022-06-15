# Magnetic_rotary_encoding

This library was written based on the rotation detection method described @ https://youtu.be/atb6d4b-VYk and also in the pdf included with the library, so if you intend to use said method for detecting rotations in your projects, this library will come in handy. The library also features a haptics controller which allows you to connect and control a vibration motor as a step count haptic feedback.



# How to connect and use the Ahmsville labs magnetic rotary encoder.

There are three ways to connect the magnetic rotary encoder, all with their benefits and draw backs.

## 1. Analog only Detection: 

in this mode the library uses just the signals from the hall effect sensors to make sense of the rotation of the encoder. the benefit of this mode is that you only need two analog pins to do all the rotation encoding, also been analog means you can use the libraries built in interpolation mode for an extremely high and granular resolution. The drawback is, this mode requires constant polling of the sensors which is not ideal for a complex system, also if the board does not have analog pins or if your setup cannot spare any of them, then you cannot use this mode.

## Connection

To use this mode, simply connect ENC-1 and ENC-2 to two analog input pins, you may also connect the LED pin to any available pin on the board. VCC  should be connected to the same voltage level used by the microcontroller on the boards, so power the encoder with 5v if the board uses 5v, and power it with 3.3v it the board uses 3.3v, you should offcus also connect the GND pin.

## Code

To test the example code, all you have to do is activate the analog only mode and change the pin assignment according to your connections. For the mode, you have the option to choose continuous rotation detection which returns a continuous count in either direction, or absolute positioning, which returns a value within the encoder’s base resolution, you can also choose the extended interpolated resolution mode for a super-duper high resolution.

```c++
#define ENCMODE 1 // BASE RESOLUTION ANALOG ONLY
//#define ENCMODE 2 // BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
//#define ENCMODE 3 // EXTENDED RESOLUTION ANALOG ONLY
//#define ENCMODE 4 // BASE RESOLUTION ANALOG AND DIGITAL
//#define ENCMODE 5 // BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
//#define ENCMODE 6 // EXTENDED RESOLUTION ANALOG AND DIGITAL
//#define ENCMODE 7 // BASE RESOLUTION DIGITAL ONLY
//#define ENCMODE 8 // BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING
```

## 2. Analog and Digital Detection: 

in this mode the library uses the signals from the hall effect sensors together with two additional digital interrupt enabled input pins. This mode has all the same features and benefits as the analog only detection mode without the need for constant polling of the hall effect sensors, this is extremely usefull for complex systems, as the interrupt pins notify the code of when a rotation occurs, so constant polling is not needed, this mode is in fact the mode used on the [Ahmsville Dial V2](https://youtu.be/yGbshYLfDnc).
Please note that the code will return to constant polling if you select the Extended Resolution mode, since this process requires interpolation between the encoder’s steps.

## Connection

To use this mode, simply connect ENC-1 and ENC-2 to two analog input pins, and E1-INT1 and E2-INT1 to two interrupt enabled pins on the board, you may also connect the LED pin to any available pins on the board. VCC  should be connected to the same voltage level used by the microcontroller on the boards, so power the encoder with 5v if the board uses 5v, and power it with 3.3v it the board uses 3.3v, you should offcus also connect the GND pin.

## Code

To test the code, all you have to do is activate the analog and digital mode and also change the pin assignment according to your connections. For this mode, you also have the option to choose continuous rotation detection which returns a continuous count in either direction, or absolute positioning, which returns a value within the encoder’s base resolution, you can also choose the extended interpolated resolution mode for a super-duper high resolution.

## 2. Digital only Detection: 

in this mode the library uses only digital interrupt enabled input pins; four of them. This mode is similar in operation to traditional rotary switches, the analog signals from the hall effect sensors have been converted to digital signals using comparators, so no analog input pins are required. This mode was developed for systems that don’t have or cannot spare analog input pins, its also the fastest mode and requires zero polling, which is why I used it to encode the [Pick N Place Wheel](https://youtu.be/MtNO8-5vCqg). 
Because this mode uses digital signals alone, you cannot use the interpolated extended resolution mode that is available in the previous modes.


## Connection

To use this mode, simply connect E1-INT1, E1-INT2, E2-INT1 and E2-INT2 to four interrupt enabled pins on the board, you may also connect the LED pin to any available pins on the board. VCC  should be connected to the same voltage level used by the microcontroller on the boards, so power the encoder with 5v if the board uses 5v, and power it with 3.3v it the board uses 3.3v,  also connect the GND pin.

## Code

To test the code, all you have to do is activate the digital only mode and also change the pin assignments according to your connections. For this mode, you have the option to choose continuous rotation detection which returns a continuous count in either direction, or absolute positioning, which returns a value within the encoder’s base resolution.

# Complimentary Input on the Encoder

You have two options when it comes to complimentary input on the encoder, you can choose to use capacitive touch or a normal push button, both the capacitive touch and the push button will be included with the kit if you buy it from my store.
To use capacitive touch, you will need an Arduino board, if you use other development boards like the ESP32 or Raspberry PI Pico, you will have to use the push button.

## Capacitive touch
To use the capacitive touch on an Arduino, you must have assembled the encoder according to the instructions, so that you can simply connect CT1 and CT2 to two available pins and then update the pin assignment in the example code accordingly.

## Push button
To use the push button, you should solder the included button to the encoder board and then connect only CT1 to an available pin on the development board.


