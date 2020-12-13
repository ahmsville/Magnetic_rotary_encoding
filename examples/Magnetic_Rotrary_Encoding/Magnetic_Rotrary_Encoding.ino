#include <MagRotaryEncoding.h>

/*  Example sketch for counting up or down with a magnetic rotary encoding setup
     the primary function detect_rotation() returns a signed integer based on the number of rotation steps detected

     connect signal pin from hall effect sensors to A0 & A1
     connect viberation motor/haptics switch circuit to pin 9 (optional)

	 ....By Ahmsville...
*/


MagRotaryEncoder knob = MagRotaryEncoder(A0, A1); // create new encoding instance and specify the arduino pins connected to the hall effect sensors


int countsteps = 0;

void setup() {
  knob.set_haptics(9, 50, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
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
