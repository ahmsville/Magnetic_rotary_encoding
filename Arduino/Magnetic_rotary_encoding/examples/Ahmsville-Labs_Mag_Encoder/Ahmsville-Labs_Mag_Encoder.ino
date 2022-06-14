#include <MagRotaryEncoding.h>
#include <FastLED.h>
/*  Example sketch for counting up or down with a magnetic rotary encoding setup
     the primary function detect_rotation() returns a signed integer based on the number of rotation steps detected

     connect signal pin from hall effect sensors to A0 & A1 (ENC-1, ENC-2)
     connect Interrupt signal (E1_INT1 ,EI_NT2,E2_INT1 ,E2_NT2) to interrupt enabled pins
     connect CT1 to pin 5 and CT2 to pin 6 for capacitive touch
     connect LED to pin 4
     connect viberation motor/haptics switch circuit or led to pin 9 (optional)

   ....By Ahmsville Labs...
*/
#define USECAPTOUCH 0


int touchtype;
int colorcounter = 260;
/********************************************************/
#define USECAPTOUCH 0 //set to zero to disable captouch
/********************************************************/

//#define ENCMODE 1 // BASE RESOLUTION ANALOG ONLY
#define ENCMODE 2 // BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
//#define ENCMODE 3 // EXTENDED RESOLUTION ANALOG ONLY
//#define ENCMODE 4 // BASE RESOLUTION ANALOG AND DIGITAL
//#define ENCMODE 5 // BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
//#define ENCMODE 6 // EXTENDED RESOLUTION ANALOG AND DIGITAL
//#define ENCMODE 7 // BASE RESOLUTION DIGITAL ONLY
//#define ENCMODE 8 // BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING

#define E1_INT1 2 // hall sensor 1 interrupt pin 1
#define E1_INT2 8 // hall sensor 1 interrupt pin 2
#define E2_INT1 3 // hall sensor 2 interrupt pin 1
#define E2_INT2 5 // hall sensor 2 interrupt pin 2

#define E1_ADC_PIN A0 // ADC PIN for hall sensor 1
#define E2_ADC_PIN A1 // ADC PIN for hall sensor 2

#define ENC_BASERES 60 // ENCODER BASE RESOLUTION

#define HAPTICS_PIN LED_BUILTIN       // ENCODER HAPTICS PIN
#define HAPTICS_STRENGTH 255 // ENCODER HAPTICS PWM DUTY CYCLE
#define HAPTICS_DURATION 50  // ENCODER HAPTICS DURATION

#define ADC_RES 10                    // ADC RESOLUTION 10 OR 12 BITS
#define BOUND 50                      // ADC NOISE ALLOWANCE
#define EXTENDEDRESOLUTION_PERCENT 95 // PERCENTAGE OF FULL ITERPOLATED ENCODER RESOLUTION

#define NUM_LEDS 15 // Num of leds on board

#if ENCMODE == 1
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E2_ADC_PIN);
#elif ENCMODE == 4 || ENCMODE == 5 || ENCMODE == 6
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E2_ADC_PIN, E1_INT1, E2_INT1);
#else
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E2_ADC_PIN);
#endif


volatile int activesensor1Interrupt = 1; //used fo switching active ISR

#if ENCMODE == 4 || ENCMODE == 5 || ENCMODE == 6
void mainISR() // ENCODER 1 INT 1 (SOUTH)
{
    myKnob.singleSensor_INT(1);
}
void setinterrupt()
{
    int pinNum = myKnob.get_sensorINTpin(1);
    pinMode(myKnob.get_sensorINTpin(1), INPUT);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(1)), mainISR, RISING);
    pinMode(myKnob.get_sensorINTpin(2), INPUT);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(2)), mainISR, RISING);

}

#elif ENCMODE == 7 || ENCMODE == 8

void isr1() {
    // 1
    activesensor1Interrupt = myKnob.processExINT(1);
    // printf("1");
}
void isr2() {
    // 2
    activesensor1Interrupt = myKnob.processExINT(2);
    // printf("2");
}
void isr3() {
    // 3
    activesensor1Interrupt = myKnob.processExINT(3);
    // printf("3");
}
void isr4() {
    // 4
    activesensor1Interrupt = myKnob.processExINT(4);
    // printf("4");
}




void setinterrupt()
{
    myKnob.setExclINTmode(E1_INT1, E1_INT2, E2_INT1, E2_INT2, true);
    pinMode(myKnob.get_sensorINTpin(1), INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(1)), isr1, RISING);
    pinMode(myKnob.get_sensorINTpin(2), INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(2)), isr2, RISING);
    pinMode(myKnob.get_sensorINTpin(3), INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(3)), isr3, RISING);
    pinMode(myKnob.get_sensorINTpin(4), INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(myKnob.get_sensorINTpin(4)), isr4, RISING);
}

#endif



int retstep1 = 0, count = 0;
bool debug = false;
String inString = "";

#if USECAPTOUCH == 1
#include <AdvCapTouch.h>
 AdvCapTouch touchpad  = AdvCapTouch();  //create a new captouch object
#endif

#define DATA_PIN    5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

int BRIGHTNESS = 125;
CRGBPalette16 currentPalette;
TBlendType    currentBlending;

void setup() {
#if ENCMODE == 1
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();

#elif ENCMODE == 2
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  // myKnob.set_bound(BOUND);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  // myKnob.invertCount(true);
  myKnob.setToStart();
#elif ENCMODE == 3
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
#elif ENCMODE == 4
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
#elif ENCMODE == 5
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  // myKnob.invertCount(true);
  myKnob.setToStart();
  #elif ENCMODE == 6
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
#elif ENCMODE == 7
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.initialize_encoder();
#elif ENCMODE == 8
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  // myKnob.invertCount(true);
  myKnob.setToStart();
#endif
#if USECAPTOUCH == 1
 touchpad.set_capTouchPins(6, 5, 0, 0, 0); //set arduino pins associated with the pads (sendpin, receivepin1, receivepin2, receivepin3, receivepin4) this example uses just one pad.
  touchpad.set_haptics(HAPTICS_PIN, HAPTICS_STRENGTH, HAPTICS_DURATION); //set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)------(optional)
  touchpad.initialize_capTouch(1);
#endif
 

  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  currentPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
}

void loop() {
  /**********************************************Detection Routines*******************************************/

#if ENCMODE == 2 || ENCMODE == 5 || ENCMODE == 8
    retstep1 = myKnob.detect_rotation(); // function returns a signed integer based on rotation step detected
    if (retstep1 != 0)
    {
        count = retstep1;
        Serial.println(count);
        colorcounter += 255/ENC_BASERES; 
          ledring_Animation2();
    }
#elif ENCMODE == 3 || ENCMODE == 6
    retstep1 = myKnob.detect_rotationHR(); // function returns a signed integer based on rotation step detected
    if (retstep1 != 0)
    {
        count += retstep1;
        Serial.println(count);
         ledring_Animation1();
    }
#else
    retstep1 = myKnob.detect_rotation(); // function returns a signed integer based on rotation step detected
    if (retstep1 != 0)
    {
        count += retstep1;
        Serial.println(count);
        ledring_Animation1();
    }
#endif

    if (debug)
    {
        delay(500);
        myKnob.DebugEncoder();
    }
    getcommand();

  /*********************************************************************************************************************/
  #if USECAPTOUCH == 1
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
#endif
  
}

void ledring_Animation1() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( currentPalette, count, BRIGHTNESS, currentBlending);
  }
  FastLED.show();
}
void ledring_Animation2() {
  int ledtoturnON = ((count + 1) - (ENC_BASERES / NUM_LEDS)) / (ENC_BASERES / NUM_LEDS); //calc the index of the led to turn on based on the resolution and count
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

void caliberateSensors()
{
    int res1 = myKnob.CalibrateSensors(1);
    int res2 = myKnob.CalibrateSensors(2);

    myKnob.LoadCalibrationData(res1, res2);
    Serial.print("sensor 1 center >> ");
    Serial.println(res1);
    Serial.print("sensor 2 center >> ");
    Serial.println(res2);
}

void getcommand() {
    char inByte = '0';
    while (Serial.available() > 0) {  //pc replied
      /******************************************/
        inByte = Serial.read();
        inString += (char)inByte;
    }
    // quick serial commands
    if (inString.startsWith("hres=")) // set high res encoding resolution
    {
        inString.replace("hres=", "");
        Serial.print("setting high res encoding resolution to >>");
        Serial.println(inString);
        myKnob.set_resolution(inString.toInt());
    }
    if (inString.startsWith("b=")) // set sensor value bound
    {
        inString.replace("b=", "");
        Serial.print("setting BOUND to >>");
        Serial.println(inString);
        myKnob.set_bound(inString.toInt());
    }
    if (inString.startsWith("b?")) // get ideal bound (b?=1)
    {
        inString.replace("b?", "");
        myKnob.get_bound(inString.toInt());
    }
    if (inString.startsWith("st")) // get ideal bound (b?=1)
    {
        Serial.println("setting current position as encoder start position");
        Serial.print("start position set to >> ");
        Serial.println(myKnob.setToStart());
    }
    if (inString.startsWith("debug")) // get ideal bound (b?=1)
    {
        if (debug)
        {
            debug = false;
            Serial.print("debug info turned off\n");
        }
        else
        {
            debug = true;
            Serial.print("debug info turned on\n");
        }
    }
    if (inString.startsWith("cali")) // get ideal bound (b?=1)
    {
        Serial.print("calibrating encoder.... \n");
        caliberateSensors();
    }
    inString = "";
}