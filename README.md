# Magnetic_rotary_encoding

This library was written based on the rotation detection method described @ https://youtu.be/atb6d4b-VYk and also in the included pdf, so if you intend to use said method for detecting rotations in your projects, this library will come in handy. The library also features a haptics controller which allows you to connect and control a vibration motor as a step count haptic feedback.

# Using the library

```C
#include <MagRotaryEncoding.h>

MagRotaryEncoder knob = MagRotaryEncoder(A0,A1); // create new encoding object and specify the Arduino analog pins connected to the hall effect sensors
int countsteps = 0;

void setup() {
  knob.set_poleStateValues(250,512,750); // set the peak ADC values for the (northpole, neutralstate, southpole)
  knob.set_haptics(6,50,255);  //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  knob.initialize_encoder();
  Serial.begin(9600);
}
void loop() {
countsteps += knob.detect_rotation();  // function returns a signed integer based on the number of rotation steps detected
  Serial.println(countsteps);
  knob.recaliberate_startPosition();
}
```

The code snippet above shows the basic use of the library.
Line 2 shows how to create a magnetic rotary encoding object, and also how you specify the analog pins on the Arduino that will be used to read the ADC values.
The first line in the setup section is used to set the ADC values that represents the magnetic states discussed above. 
The second line in the setup section is optional, only use this function when you have a haptics circuit setup.
In the main loop, the function detect_rotation() returns a signed integer “1” if a clockwise rotation is detected, “-1” if a counter clockwise rotation is detected and a “0” if no rotation is detected.
The function recaliberate_startPosition() does exactly what its name implies, this function should be called before or after the detect_rotation() function.
