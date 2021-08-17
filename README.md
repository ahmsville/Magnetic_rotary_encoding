# Magnetic_rotary_encoding

This library was written based on the rotation detection method described @ https://youtu.be/atb6d4b-VYk and also in the pdf included with the library, so if you intend to use said method for detecting rotations in your projects, this library will come in handy. The library also features a haptics controller which allows you to connect and control a vibration motor as a step count haptic feedback.

# Using the library

```C
#include <MagRotaryEncoding.h>

/*  Example sketch for counting up or down with a magnetic rotary encoding setup
     the primary function detect_rotation() returns a signed integer based on the number of rotation steps detected
     connect analog signal pin from hall effect sensors to A5 & A6
     connect Interrupt signal pin from encoder to pin 2 & 3
     connect viberation motor/haptics switch circuit to pin 9 (optional)
	 ....By Ahmsville...
*/


//Interrupt based detection
MagRotaryEncoder knob = MagRotaryEncoder(A5, A6, 2, 3); // create new encoding instance and specify the arduino pins connected to the hall effect sensors, and the pins use for interrupts


int countsteps = 0;
volatile bool activesensorInterrupt = 0; //used fo switching active ISR

void ISR1 () {
  if (activesensorInterrupt == 0) { //sensor 1 interrupt is active.
    knob.sensor1_INT();
    activesensorInterrupt = !activesensorInterrupt;
  }
}
void ISR2 () {
  if (activesensorInterrupt == 1) { //sensor 1 interrupt is active.
    knob.sensor2_INT();
    activesensorInterrupt = !activesensorInterrupt;
  }
}


void setinterrupt() {
  pinMode(knob.get_sensorINTpin(1), INPUT);
  attachInterrupt(digitalPinToInterrupt(knob.get_sensorINTpin(1)), ISR1, RISING);

  pinMode(knob.get_sensorINTpin(2), INPUT);
  attachInterrupt(digitalPinToInterrupt(knob.get_sensorINTpin(2)), ISR2, RISING);
}


void setup() {
  setinterrupt();
  knob.set_haptics(9, 50, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  //knob.set_resolution(90); //uncomment if your using the detect_rotationHR() function
  knob.initialize_encoder();
  Serial.begin(115200);
}

void loop() {
  /**********************************************Detection Routines*******************************************/
  //oncomment one of the rotation detection functions
  countsteps += knob.detect_rotation();  // function returns a signed integer based on rotation step detected
  //countsteps += knob.detect_rotationWithRate();  // function returns a rate multiplied signed integer based on rotation step detected
  //countsteps += knob.detect_rotationHR();  // function returns granular rotation steps

  Serial.println(countsteps);
}
```

The code snippet above shows the basic use of the library.

In the main loop, you have three detection function you can call.

- detect_rotation() returns a signed integer “1” if a clockwise rotation is detected, “-1” if a counter clockwise rotation is detected and a “0” if no rotation is detected.
- detect_rotationWithRate() returns a rate multiplied signed integer “1” if a clockwise rotation is detected, “-1” if a counter clockwise rotation is detected and a “0” if no rotation is detected.
- detect_rotationHR() returns granular signed integers “1” ,"-1" or "0".


