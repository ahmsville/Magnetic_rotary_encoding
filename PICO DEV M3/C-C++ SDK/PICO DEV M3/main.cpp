#include <stdio.h>
#include "pico/stdlib.h"
#include "MagRotaryEncoding.h"
#include <string>
#include <string.h>
#include <iostream>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "ws2812.pio.h"

using namespace std;

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

/*************************digital pins******************************/
#define E1_INT1 2 // hall sensor 1 interrupt pin 1
#define E1_INT2 3 // hall sensor 1 interrupt pin 2
#define E2_INT1 4 // hall sensor 2 interrupt pin 1
#define E2_INT2 5 // hall sensor 2 interrupt pin 2

#define E1_ADC_PIN 26 // ADC PIN for hall sensor 1
#define E2_ADC_PIN 27 // ADC PIN for hall sensor 2

#define E1_ADC_CH 0 // rp2040 ADC ch 0 for E1
#define E2_ADC_CH 1 // rp2040 ADC ch 1 for E2

#define ENC_BASERES 36 // ENCODER BASE RESOLUTION

#define HAPTICS_PIN 25       // ENCODER HAPTICS PIN
#define HAPTICS_STRENGTH 255 // ENCODER HAPTICS PWM DUTY CYCLE
#define HAPTICS_DURATION 50  // ENCODER HAPTICS DURATION

#define ADC_RES 12                   // ADC RESOLUTION 10 OR 12 BITS
#define BOUND 20                     // ADC NOISE ALLOWANCE
#define EXTENDEDRESOLUTION_PERCENT 1 // PERCENTAGE OF FULL ITERPOLATED ENCODER RESOLUTION

#define CT1 24 // PUSH BUTTON (CT1)

#define NUM_LEDS 18 // Num of leds on board
const int ALED_PIN = 8;

float ALEDBrightness = 0.2;
uint32_t ledMem[18];

#define HIGHLEVEL 0x02
#define LOWLEVEL 0x01
#define RISING 0x08
#define FALLING 0x04

#define red 1
#define green 2
#define blue 3

int retstep1 = 0;
int retstep2 = 0;
bool debug = true;
long debugtimetracker = 0;
int count = 0;
int prevcount = 0;
float G_setpoint = 0;

#if ENCMODE == 1
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH);
#elif ENCMODE == 5 || ENCMODE == 6 || ENCMODE == 7 || ENCMODE == 8
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH, E1_INT1, E2_INT1);
#else
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH);
#endif

//;

char inbuff[100];
int buffloatcounter = 0;
bool dataavailable = false;
string inString = "";
string outstr = "";

int activeledcolor[NUM_LEDS];

volatile int activesensor1Interrupt = 1; // used for switching active ISR
volatile int count_interrupt = 0;

static inline void put_pixel(uint32_t pixel_grb)
{
  pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
  return ((uint32_t)(r) << 8) |
         ((uint32_t)(g) << 16) |
         (uint32_t)(b);
}

#if ENCMODE == 5 || ENCMODE == 6 || ENCMODE == 7 || ENCMODE == 8
void mainISR(uint gpiopin, uint32_t events) // ENCODER 1 INT 1 (SOUTH)
{
  myKnob.singleSensor_INT(gpiopin);
}
void setinterrupt()
{
  int pinNum = myKnob.get_sensorINTpin(1);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(2);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
}

#elif ENCMODE == 9 || ENCMODE == 10
void mainISR(uint gpiopin, uint32_t events) // ENCODER 1 INT 1 (SOUTH)
{
  /************************servo encoder*********************************/
  if (gpiopin == myKnob.get_sensorINTpin(3))
  {
    // 3
    activesensor1Interrupt = myKnob.processExINT(3);
    // printf("3");
  }
  else if (gpiopin == myKnob.get_sensorINTpin(1))
  {
    // 1
    activesensor1Interrupt = myKnob.processExINT(1);
    // printf("1");
  }

  if (gpiopin == myKnob.get_sensorINTpin(2))
  {
    // 2
    activesensor1Interrupt = myKnob.processExINT(2);
    // printf("2");
  }
  else if (gpiopin == myKnob.get_sensorINTpin(4))
  {
    // 4
    activesensor1Interrupt = myKnob.processExINT(4);
    // printf("4");
  }
  /***********************************************************************/

  /***************************knob encoder********************************/
  if (gpiopin == myKnob.get_sensorINTpin(3))
  {
    // 3
    myKnob.processExINT(3);
    // printf("3");
  }
  else if (gpiopin == myKnob.get_sensorINTpin(1))
  {
    // 1
    myKnob.processExINT(1);
    // printf("1");
  }

  if (gpiopin == myKnob.get_sensorINTpin(2))
  {
    // 2
    myKnob.processExINT(2);
    // printf("2");
  }
  else if (gpiopin == myKnob.get_sensorINTpin(4))
  {
    // 4
    myKnob.processExINT(4);
    // printf("4");
  }
  /***************************************************************************/
}

void setinterrupt()
{
  myKnob.setExclINTmode(E1_INT1, E1_INT2, E2_INT1, E2_INT2, true);

  int pinNum = myKnob.get_sensorINTpin(1);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(2);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(3);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(4);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
}

#endif

int activeledindex = 0;
int setLedBrightness(int clevel)
{
  float tempb = float(clevel) * ALEDBrightness;
  return int(tempb);
}
void updateLed(uint8_t r, uint8_t g, uint8_t b, int ledindex)
{
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    if (i == ledindex)
    {
      ledMem[i] = urgb_u32(setLedBrightness(r), setLedBrightness(g), setLedBrightness(b));
      put_pixel(ledMem[i]);
    }
    else
    {
      put_pixel(ledMem[i]);
    }
  }
  sleep_us(110);
}

void caliberateSensors()
{
  int res1 = myKnob.CalibrateSensors(1);
  int res2 = myKnob.CalibrateSensors(2);

  myKnob.LoadCalibrationData(res1, res2);
  printf("sensor 1 center >> %d sensor 2 center >> %d \n", res1, res2);
}

void readtobuffer()
{
  char chr = getchar_timeout_us(0);
  if (chr != 255)
  {
    if (chr == '*')
    {
      dataavailable = true;
    }
    // cout << chr << buffloatcounter << std::endl;
    if (buffloatcounter < sizeof(inbuff))
    {
      inbuff[buffloatcounter] = chr;
      buffloatcounter += 1;
    }
    else
    {
      buffloatcounter = 0;
    }
  }
  else
  {
    dataavailable = false;
  }
}

void processinData()
{
  if (dataavailable)
  {
    // cout << inbuff << std::endl;
    int tempcnt = 0;
    while (inbuff[tempcnt] != '*')
    {
      inString += inbuff[tempcnt];
      tempcnt += 1;
    }
    printf(inString.c_str());
    printf("\n");
    // quick serial commands
    if (inString.find("hres=") == 0) // set high res encoding resolution
    {
      inString.replace(inString.find("hres="), 5, "");
      printf("setting high res encoding resolution to >> %s \n", inString.c_str());
      myKnob.set_resolution(stoi(inString));
    }
    if (inString.find("b=") == 0) // set sensor value bound
    {
      inString.replace(inString.find("b="), 2, "");
      printf("setting BOUND to >> %s \n", inString.c_str());
      myKnob.set_bound(stoi(inString));
    }
    if (inString.find("b?") == 0) // get ideal bound (b?=1)
    {
      inString.replace(inString.find("b?"), 2, "");
      myKnob.get_bound(stoi(inString));
    }
    if (inString.find("st") == 0) // get ideal bound (b?=1)
    {
      printf("setting current position as encoder start position \n");
      printf("start position set to >> %d \n", myKnob.setToStart());
    }
    if (inString.find("debug") == 0) // get ideal bound (b?=1)
    {
      if (debug)
      {
        debug = false;
        printf("debug info turned off\n");
      }
      else
      {
        debug = true;
        printf("debug info turned on\n");
      }
    }
    if (inString.find("cali") == 0) // get ideal bound (b?=1)
    {
      printf("Make sure the knob is not attached for this process.... \n");
      printf("calibrating encoder.... \n");
      caliberateSensors();
    }

    buffloatcounter = 0;
    dataavailable = false;
    memset(inbuff, 0, sizeof(inbuff));
    inString = "";
  }
}

/*****************************************************************************/

long infotimer = (time_us_64() / 1000);
float floatcount = 0;
void updateCount()
{
#if ENCMODE == 2 || ENCMODE == 6 || ENCMODE == 10
  retstep1 = myKnob.detect_rotation(); // function returns a signed integer based on rotation step detected
  if (retstep1 != 0)
  {
    count = retstep1;
    printf("COUNT >> %d \n", count);
  }
#elif ENCMODE == 3 || ENCMODE == 7
  floatcount += myKnob.detect_rotationHR(); // function returns a signed integer based on rotation step detected
  if (int(floatcount) != count)
  {
    if (((time_us_64() / 1000) - infotimer) > 5)
    {
      printf("FLOATCOUNT >> %f \n", floatcount);
      infotimer = (time_us_64() / 1000);
    }
    count = int(floatcount);
  }

#elif ENCMODE == 4 || ENCMODE == 8
  floatcount = myKnob.detect_rotationHR(); // function returns a signed integer based on rotation step detected
  if (((time_us_64() / 1000) - infotimer) > 50)
  {
    printf("FLOATCOUNT >> %f \n", floatcount);
    infotimer = (time_us_64() / 1000);
  }
  count = int(floatcount);
#else
  retstep1 = myKnob.detect_rotation(); // function returns a signed integer based on rotation step detected
  if (retstep1 != 0)
  {
    count += retstep1;
    printf("COUNT >> %d \n", count);
  }

#endif
  
}

int main()
{

  stdio_init_all();
  sleep_ms(500);

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  gpio_init(CT1);
  gpio_set_dir(CT1, GPIO_IN);
  gpio_pull_up(CT1);

  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
  // char str[12];

  ws2812_program_init(pio, sm, offset, ALED_PIN, 800000, false);

  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    updateLed(255, 0, 0, i);
  }
  sleep_ms(1000);
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    updateLed(0, 255, 0, i);
  }
  sleep_ms(1000);
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    updateLed(0, 0, 255, i);
  }
  sleep_ms(1000);

#if ENCMODE == 1
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
  myKnob.detect_rotationWithRate(true);
#elif ENCMODE == 2
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  myKnob.invertCount(true);
  myKnob.setToStart();
#elif ENCMODE == 3
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.setExtendedAbsolutePositioning(false);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
  // myKnob.setToStart();
#elif ENCMODE == 4
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.setExtendedAbsolutePositioning(true);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
  myKnob.setToStart();
#elif ENCMODE == 5
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
#elif ENCMODE == 6
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.set_bound(BOUND);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  // myKnob.invertCount(true);
  myKnob.setToStart();
#elif ENCMODE == 7
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.setExtendedAbsolutePositioning(false);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
#elif ENCMODE == 8
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_adcresolution(ADC_RES);
  myKnob.setExtendedAbsolutePositioning(true);
  myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.set_bound(BOUND);
  myKnob.initialize_encoder();
  myKnob.setToStart();
#elif ENCMODE == 9
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH);
  myKnob.initialize_encoder();
  myKnob.initialize_encoder();
#elif ENCMODE == 10
  setinterrupt();
  myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  myKnob.set_encoderResolution(ENC_BASERES);
  myKnob.initialize_encoder();
  // myKnob.invertCount(true);
  myKnob.setToStart();
#endif

  while (true)
  {

    if (debug)
    {
      if (((time_us_64() / 1000) - debugtimetracker) > 1000)
      {
        myKnob.DebugEncoder();
        debugtimetracker = (time_us_64() / 1000);
      }
    }

    if (gpio_get(CT1) == 0)
    {
      printf("button pressed \n");
      myKnob.setToStart();
    }

    readtobuffer();
    processinData();
    updateCount();

    if (count > prevcount) //++
    {
      if (activeledindex == (NUM_LEDS - 1))
      {
        activeledindex = 0;
      }
      else
      {
        activeledindex += 1;
      }
      for (size_t i = 0; i < NUM_LEDS; i++)
      {
        ledMem[i] = 0;
      }
      
      updateLed(255, 0, 0, activeledindex);
      prevcount = count;
    }
    else if (count < prevcount) //--
    {
      if (activeledindex == (0))
      {
        activeledindex = NUM_LEDS - 1;
      }
      else
      {
        activeledindex -= 1;
      }
      for (size_t i = 0; i < NUM_LEDS; i++)
      {
        ledMem[i] = 0;
      }
      updateLed(255, 0, 0, activeledindex);
      prevcount = count;
    }
  }
}
