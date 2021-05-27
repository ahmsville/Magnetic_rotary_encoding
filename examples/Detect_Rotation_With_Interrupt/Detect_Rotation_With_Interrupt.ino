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
volatile int activesensorInterrupt = 1; //used fo switching active ISR

void ISR1 () {
  if (activesensorInterrupt == 1) { //sensor 1 interrupt is active.
    
    activesensorInterrupt = knob.sensor1_INT();
  }
}
void ISR2 () {
  if (activesensorInterrupt == 2) { //sensor 1 interrupt is active.
    
    activesensorInterrupt = knob.sensor2_INT();
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