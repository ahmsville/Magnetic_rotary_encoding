# Magnetic_rotary_encoding

This library was written based on the rotation detection method described @ https://youtu.be/atb6d4b-VYk and also in the pdf included with the library, so if you intend to use said method for detecting rotations in your projects, this library will come in handy. The library also features a haptics controller which allows you to connect and control a vibration motor as a step count haptic feedback.



# How to connect and use the Ahmsville labs magnetic rotary encoder.

There are three ways to connect the magnetic rotary encoder, all with their benefits and draw backs.
## 1 Analog only Detection: 

in this mode the library uses just the signals from the hall effect sensors to make sense of the rotation of the encoder. the benefit of this mode is that you only need two analog pins to do all the rotation encoding, also been analog means you can use the libraries built in interpolation mode for an extremely high and granular resolution. The drawback is, this mode requires constant polling of the sensors which is not ideal for a complex system, also if the board does not have analog pins or if your setup cannot spare any of them, then you cannot use this mode.

## Connection

To use this mode, simply connect ENC-1 and ENC-2 to two analog input pins, you may also connect the LED pin to any available pin on the board. VCC  should be connected to the same voltage level used by the microcontroller on the boards, so power the encoder with 5v if the board uses 5v, and power it with 3.3v it the board uses 3.3v, you should offcus also connect the GND pin.

## Code

To test the example code, all you have to do is activate the analog only mode and change the pin assignment according to your connections. For the mode, you have the option to choose continuous rotation detection which returns a continuous count in either direction, or absolute positioning, which returns a value within the encoder’s base resolution, you can also choose the extended interpolated resolution mode for a super-duper high resolution.
