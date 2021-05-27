#include <MagRotaryEncoding.h>
#include <AdvCapTouch.h>
#include <FastLED.h>
/*

   ....By Ahmsville...
*/

//Interrupt based detection
MagRotaryEncoder knob = MagRotaryEncoder(A6, A7, 2, 3); // create new encoding instance and specify the arduino pins connected to the hall effect sensors, and the pins use for interrupts
AdvCapTouch touchpad  = AdvCapTouch();  //create a new captouch object

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


int touchtype;

int Wheel1_Slot = 1;
int Wheel1_SlotCount = 60;
int retstep1;
bool Wheel1reset = false;
String wheel1outstring = "w1x";

int COUNT = 0;


#include <FastLED.h>
#define NUM_LEDS 15

CRGBArray<NUM_LEDS> leds;

void setup() {
  setinterrupt();
  knob.set_haptics(9, 50, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  knob.initialize_encoder();


  touchpad.set_capTouchPins(6, 5, 0, 0, 0); //set arduino pins associated with the pads (sendpin, receivepin1, receivepin2, receivepin3, receivepin4) this example uses just one pad.
  touchpad.set_haptics(9, 40, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)------(optional)
  touchpad.initialize_capTouch(1);

  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, 7>(leds, NUM_LEDS);
}

void loop() {
  retstep1 = knob.detect_rotationWithRate();  // function returns a signed integer based on rotation step detected


  if (retstep1 != 0) {
    Serial.print(retstep1);
    Serial.print("\t");
    getActiveSlots();
    Serial.println(wheel1outstring + Wheel1_Slot);
  }



  setLedState();


  /*********************************************************************************************************************/
  //TOUCH DETECTION
  touchtype = touchpad.detect_touchFromNoise(0);  //function return 1-4 based on the input detected, 1 = singletap, 2 = doubletap, 3 = shortpress, 4 = longpress
  if (touchtype == 1) {
    leds[0] = CRGB::Red;
    FastLED.show();
    Wheel1_Slot = 1;
    wheel1outstring = "w1_";
    Wheel1reset = true;

    delay(1000);
    leds[0] = CRGB::Blue;
    FastLED.show();
  }
  else if (touchtype == 2) {
    Serial.println("Doubletap");
  }


  /*****************************************************LedRing Animations***************************************************************/

}

void getActiveSlots() {


  //keep counter value in the encoder's resolution range
  if (retstep1 < 0) {
    for (int i = 0; i > retstep1; i--) {
      if ((Wheel1_Slot - 1) >= 1) {
        Wheel1_Slot = Wheel1_Slot - 1;
      } else if ((Wheel1_Slot - 1) < 1) {
        Wheel1_Slot = Wheel1_SlotCount;
      }
    }
  }
  else if (retstep1 > 0) {
    for (int i = 0; i < retstep1; i++) {
      if ((Wheel1_Slot + 1) <= Wheel1_SlotCount) {
        Wheel1_Slot = Wheel1_Slot + 1;
      } else if ((Wheel1_Slot + 1) > Wheel1_SlotCount) {
        Wheel1_Slot = 1;
      }
    }
  }



}

void setLedState() {
  if (Wheel1reset) {
    if (Wheel1_Slot == 1) {
      leds[0] = CRGB::Green;
    } else {
      leds[0] = CRGB::Blue;
    }
  } else {
    leds[0] = CRGB::Red;
  }

  FastLED.show();
}
