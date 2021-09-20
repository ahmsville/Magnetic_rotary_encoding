#include <MagRotaryEncoding.h>
#include <AdvCapTouch.h>
#include <FastLED.h>
/*  Example sketch for counting up or down with a magnetic rotary encoding setup
     the primary function detect_rotation() returns a signed integer based on the number of rotation steps detected

     connect signal pin from hall effect sensors to A5 & A6 (ENC-1, ENC-2)
     connect Interrupt signal (EINT1 ,EINT2) pin from encoder to pin 2 & 3 or any other interrupt enabled pins
     connect CT1 to pin 5 and CT2 to pin 6 for capacitive touch
     connect LED to pin 4
     connect viberation motor/haptics switch circuit to pin 9 (optional)

   ....By Ahmsville...
*/

//Interrupt based detection
MagRotaryEncoder knob = MagRotaryEncoder(A6, A7, 2, 3); // create new encoding instance and specify the arduino pins connected to the hall effect sensors, and the pins use for interrupts
AdvCapTouch touchpad  = AdvCapTouch();  //create a new captouch object

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


int touchtype;

int countsteps = 0, retstep, COUNT = 0;
int colorcounter = 260;
int encoderRes = 60;

int detectionMode = 1;

#define DATA_PIN    7
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    15
CRGB leds[NUM_LEDS];

int BRIGHTNESS = 95;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {
  setinterrupt();
  knob.set_haptics(9, 50, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  knob.initialize_encoder();
  //knob.set_resolution(90); //uncomment if your using the detect_rotationHR() function

  touchpad.set_capTouchPins(6, 5, 0, 0, 0); //set arduino pins associated with the pads (sendpin, receivepin1, receivepin2, receivepin3, receivepin4) this example uses just one pad.
  touchpad.set_haptics(9, 40, 255); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)------(optional)
  touchpad.initialize_capTouch(1);

  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {
  /**********************************************Detection Routines*******************************************/
  //oncomment one of the rotation detection functions
 retstep = knob.detect_rotation();  // function returns a signed integer based on rotation step detected
  //retstep = knob.detect_rotationWithRate();  // function returns a rate multiplied signed integer based on rotation step detected
  //retstep = knob.detect_rotationHR();  // function returns granular rotation steps
  /***********************************************************************************************************/

  COUNT += retstep; //continuous step counter
  colorcounter += retstep;

  if (retstep != 0) {
   Serial.print("Returned =");
   Serial.print("\t");
    Serial.print(retstep);
   Serial.print("\t");
    Serial.println(COUNT);
  }

  //keep counter value in the encoder's resolution range
  if (retstep < 0) { //counterclockwise rotation
    if (countsteps == 0) {
      countsteps = encoderRes;
      countsteps += retstep;
    } else {
      countsteps += retstep;
    }
  } else {
    countsteps += retstep;
  }
  if (countsteps >= encoderRes) { //reset counter after every full revolution
    countsteps = 0;
  } else if (countsteps < 0) {
    countsteps = encoderRes - 1;
  }

  /*********************************************************************************************************************/
  //TOUCH DETECTION
  touchtype = touchpad.detect_touchFromNoise(0);  //function return 1-4 based on the input detected, 1 = singletap, 2 = doubletap, 3 = shortpress, 4 = longpress
  if (touchtype == 1) {
    Serial.println("Singletap");
  }
  else if (touchtype == 2) {
    Serial.println("Doubletap");
  }
  else if (touchtype == 3) {
    Serial.println("Shortpress");
  }
  else if (touchtype == 4) {
    Serial.println("longpress");
  }

  /*****************************************************LedRing Animations***************************************************************/

  ledring_Animation1();
  //ledring_Animation2();

}

void ledring_Animation1() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, colorcounter, BRIGHTNESS, currentBlending);
  }
  FastLED.show();
}
void ledring_Animation2() {
  int ledtoturnON = ((countsteps + 1) - (encoderRes / NUM_LEDS)) / (encoderRes / NUM_LEDS); //calc the index of the led to turn on based on the resolution and count
  //turn on only the calc led index
  int blendrate = 10;
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == (ledtoturnON - 1)) {
      nblend(leds[i], CRGB::Black, blendrate);
    }
    else if (i == ledtoturnON) {
      nblend(leds[i], ColorFromPalette( currentPalette, colorcounter, BRIGHTNESS, currentBlending), blendrate);
      // leds[i] = ColorFromPalette( currentPalette, colorcounter, BRIGHTNESS, currentBlending);
    }
    else if (i == (ledtoturnON + 1)) {
      nblend(leds[i], CRGB::Black, blendrate);
    }
    else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();

}