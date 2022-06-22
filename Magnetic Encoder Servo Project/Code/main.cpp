#include <stdio.h>
#include "pico/stdlib.h"
#include "MagRotaryEncoding.h"
#include <string>
#include <string.h>
#include <iostream>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "ws2812.pio.h"
#include "PID.h"

using namespace std;

//#define ENCMODE 1 // BASE RESOLUTION ANALOG ONLY
//#define ENCMODE 2 // BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
//#define ENCMODE 3 // EXTENDED RESOLUTION ANALOG ONLY
//#define ENCMODE 4 // BASE RESOLUTION ANALOG AND DIGITAL
//#define ENCMODE 5 // BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
//#define ENCMODE 6 // EXTENDED RESOLUTION ANALOG AND DIGITAL
#define ENCMODE 7 // BASE RESOLUTION DIGITAL ONLY
//#define ENCMODE 8 // BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING

/*************************servo encoder******************************/
#define E1_INT1 16 // hall sensor 1 interrupt pin 1
#define E1_INT2 17 // hall sensor 1 interrupt pin 2
#define E2_INT1 18 // hall sensor 2 interrupt pin 1
#define E2_INT2 19 // hall sensor 2 interrupt pin 2

/************************controller encoder*************************/
#define CON_E1_INT1 4 // hall sensor 1 interrupt pin 1
#define CON_E1_INT2 5 // hall sensor 1 interrupt pin 2
#define CON_E2_INT1 6 // hall sensor 2 interrupt pin 1
#define CON_E2_INT2 7 // hall sensor 2 interrupt pin 2

#define E1_ADC_PIN 26 // ADC PIN for hall sensor 1
#define E2_ADC_PIN 27 // ADC PIN for hall sensor 2

#define E1_ADC_CH 0 // rp2040 ADC ch 0 for E1
#define E2_ADC_CH 1 // rp2040 ADC ch 1 for E2

#define ENC_BASERES 60 // ENCODER BASE RESOLUTION

#define HAPTICS_PIN 25       // ENCODER HAPTICS PIN
#define HAPTICS_STRENGTH 255 // ENCODER HAPTICS PWM DUTY CYCLE
#define HAPTICS_DURATION 50  // ENCODER HAPTICS DURATION

#define ADC_RES 12                    // ADC RESOLUTION 10 OR 12 BITS
#define BOUND 20                      // ADC NOISE ALLOWANCE
#define EXTENDEDRESOLUTION_PERCENT 95 // PERCENTAGE OF FULL ITERPOLATED ENCODER RESOLUTION

#define CT1 8 // PUSH BUTTON (CT1)

#define NUM_LEDS 15 // Num of leds on board
const int ALED_PIN = 15;

#define revMotor 13
#define forMotor 12

void setpwmpins()
{
  gpio_set_function(forMotor, GPIO_FUNC_PWM);
  gpio_set_function(revMotor, GPIO_FUNC_PWM);
  uint slice_num = pwm_gpio_to_slice_num(forMotor);
  // Set period of 4 cycles (0 to 3 inclusive)
  pwm_config config = pwm_get_default_config();
  pwm_config_set_clkdiv(&config, 4.f);
  pwm_init(slice_num, &config, true);
  pwm_set_wrap(slice_num, 255);
  // Set channel A output high for one cycle before dropping
  // pwm_set_chan_level(slice_num, pwm_gpio_to_channel(forMotor), 0);
  // Set the PWM running
  pwm_set_enabled(slice_num, true);

  slice_num = pwm_gpio_to_slice_num(revMotor);
  // // Set period of 4 cycles (0 to 3 inclusive)
  pwm_init(slice_num, &config, true);
  pwm_set_wrap(slice_num, 255);
   pwm_set_wrap(slice_num, 255);
  // // Set channel A output high for one cycle before dropping
  // pwm_set_chan_level(slice_num, pwm_gpio_to_channel(revMotor), 0);
  // // Set the PWM running
  pwm_set_enabled(slice_num, true);
}
void PWM_On(int strn, int pinnum)
{
  if (pinnum == forMotor)
  {
    pwm_set_gpio_level(revMotor, 0);
  }
  else if (pinnum == revMotor)
  {
    pwm_set_gpio_level(forMotor, 0);
  }

  pwm_set_gpio_level(pinnum, strn);
}

#define HIGHLEVEL 0x02
#define LOWLEVEL 0x01
#define RISING 0x08
#define FALLING 0x04

#define red 1
#define green 2
#define blue 3

int retstep1 = 0;
int retstep2 = 0;
bool debug = false;
int count = 0;
int prevcount = 0;
float G_setpoint = 0;

#if ENCMODE == 1
MagRotaryEncoder servoEncoder = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH);
#elif ENCMODE == 4 || ENCMODE == 5 || ENCMODE == 6
MagRotaryEncoder servoEncoder = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH, E1_INT1, E2_INT1);
#else
MagRotaryEncoder servoEncoder = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH);
MagRotaryEncoder myKnob = MagRotaryEncoder(E1_ADC_PIN, E1_ADC_CH, E2_ADC_PIN, E2_ADC_CH);
#endif

//;

char inbuff[100];
int buffcounter = 0;
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

#if ENCMODE == 4 || ENCMODE == 5 || ENCMODE == 6
void mainISR(uint gpiopin, uint32_t events) // ENCODER 1 INT 1 (SOUTH)
{
  servoEncoder.singleSensor_INT(gpiopin);
}
void setinterrupt()
{
  int pinNum = servoEncoder.get_sensorINTpin(1);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = servoEncoder.get_sensorINTpin(2);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
}

#elif ENCMODE == 7 || ENCMODE == 8
void mainISR(uint gpiopin, uint32_t events) // ENCODER 1 INT 1 (SOUTH)
{
/************************servo encoder*********************************/
  if (gpiopin == servoEncoder.get_sensorINTpin(3))
  {
    // 3
    activesensor1Interrupt = servoEncoder.processExINT(3);
    // printf("3");
  }
  else if (gpiopin == servoEncoder.get_sensorINTpin(1))
  {
    // 1
    activesensor1Interrupt = servoEncoder.processExINT(1);
    // printf("1");
  }

  if (gpiopin == servoEncoder.get_sensorINTpin(2))
  {
    // 2
    activesensor1Interrupt = servoEncoder.processExINT(2);
    // printf("2");
  }
  else if (gpiopin == servoEncoder.get_sensorINTpin(4))
  {
    // 4
    activesensor1Interrupt = servoEncoder.processExINT(4);
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
  servoEncoder.setExclINTmode(E1_INT1, E1_INT2, E2_INT1, E2_INT2, true);

  int pinNum = servoEncoder.get_sensorINTpin(1);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = servoEncoder.get_sensorINTpin(2);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = servoEncoder.get_sensorINTpin(3);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = servoEncoder.get_sensorINTpin(4);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);

  myKnob.setExclINTmode(CON_E1_INT1, CON_E1_INT2, CON_E2_INT1, CON_E2_INT2, true);

  pinNum = myKnob.get_sensorINTpin(1);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(2);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(3);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
  pinNum = myKnob.get_sensorINTpin(4);
  gpio_set_irq_enabled_with_callback(pinNum, RISING, true, mainISR);
}

#endif

void pidSetup();
int getEncoderStepsFromDegree(float deg);
void pidMove(float setpoint);

int activeledindex = 0;
void updateLed(uint8_t r, uint8_t g, uint8_t b, int ledindex)
{
  for (size_t i = 0; i < NUM_LEDS; i++)
  {
    if (i == ledindex)
    {
      put_pixel(urgb_u32(r, g, b));
    }
    else
    {
      put_pixel(urgb_u32(0, 0, 0)); // black
    }
  }
  sleep_us(110);
}

void caliberateSensors()
{
  int res1 = servoEncoder.CalibrateSensors(1);
  int res2 = servoEncoder.CalibrateSensors(2);

  servoEncoder.LoadCalibrationData(res1, res2);
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
    // cout << chr << buffcounter << std::endl;
    if (buffcounter < sizeof(inbuff))
    {
      inbuff[buffcounter] = chr;
      buffcounter += 1;
    }
    else
    {
      buffcounter = 0;
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
      servoEncoder.set_resolution(stoi(inString));
    }
    if (inString.find("b=") == 0) // set sensor value bound
    {
      inString.replace(inString.find("b="), 2, "");
      printf("setting BOUND to >> %s \n", inString.c_str());
      servoEncoder.set_bound(stoi(inString));
    }
    if (inString.find("b?") == 0) // get ideal bound (b?=1)
    {
      inString.replace(inString.find("b?"), 2, "");
      servoEncoder.get_bound(stoi(inString));
    }
    if (inString.find("st") == 0) // get ideal bound (b?=1)
    {
      printf("setting current position as encoder start position \n");
      printf("start position set to >> %d \n", servoEncoder.setToStart());
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
      printf("calibrating encoder.... \n");
      caliberateSensors();
    }
    if (inString.find("f=") == 0)
    {
      inString.replace(inString.find("f="), 2, "");
      printf("setting PWM to >> %s \n", inString.c_str());
      PWM_On(stoi(inString), forMotor);
    }
    if (inString.find("r=") == 0)
    {
      inString.replace(inString.find("r="), 2, "");
      printf("setting PWM to >> %s \n", inString.c_str());
      PWM_On(stoi(inString), revMotor);
    }
    if (inString.find("pid") == 0)
    {
      pidSetup();
    }
    if (inString.find("md=") == 0)
    {
      inString.replace(inString.find("md="), 3, "");
      printf("encoder steps equivalent >> %d \n", getEncoderStepsFromDegree(stof(inString)));

      G_setpoint = getEncoderStepsFromDegree(stof(inString));
    }

    buffcounter = 0;
    dataavailable = false;
    memset(inbuff, 0, sizeof(inbuff));
    inString = "";
  }
}

/**************************************PID VARIABLES**************************************/
/* Controller parameters */
#define PID_KP 2.5f
#define PID_KI 0.4f
#define PID_KD 0.5f

#define PID_TAU 0.01f

#define PWM_MAX 160.0f
#define PWM_MIN -160.0f

#define resolutionMultiplier 78.857f

#define PID_LIM_MIN PWM_MIN
#define PID_LIM_MAX PWM_MAX

#define PID_LIM_MIN_INT 0.2f
#define PID_LIM_MAX_INT 0.2f

#define SAMPLE_TIME_S 0.1f

PIDController pid;

/* Simulated dynamical system (first order) */
float TestSystem_Update(float inp);

int getEncoderStepsFromDegree(float deg)
{
  // calc encoder degree equivalent
  float eqE_deg = resolutionMultiplier * deg;
  // convert to encoder steps
  int toSteps = eqE_deg / (360 / ENC_BASERES);
  return toSteps;
}

void pidSetup()
{
  /* Initialise PID controller */
  pid = {PID_KP, PID_KI, PID_KD,
         PID_TAU,
         PID_LIM_MIN, PID_LIM_MAX,
         PID_LIM_MIN_INT, PID_LIM_MAX_INT,
         SAMPLE_TIME_S};

  PIDController_Init(&pid);
}

int getPWMin_Range(float pidpwm)
{
  float minpwm = 40;
  float pwmrange = PWM_MAX - minpwm;
  int res = minpwm + ((pwmrange / PWM_MAX) * pidpwm);
  return res;
}

void pidMove(float setpoint)
{
  float E_position = float(count);
  /* Compute new position */
  PIDController_Update(&pid, setpoint, E_position);
  // printf("pidout >> %f \n", pid.out);
  if (pid.out > 0)
  {
    PWM_On(getPWMin_Range(pid.out), forMotor);
  }
  else if (pid.out < 0)
  {
    PWM_On(getPWMin_Range((-1) * pid.out), revMotor);
  }
}
float TestSystem_Update(float inp)
{

  static float output = 0.0f;
  static const float alpha = 0.02f;

  output = (SAMPLE_TIME_S * inp + output) / (1.0f + alpha * SAMPLE_TIME_S);

  return output;
}
/*****************************************************************************/

long infotimer = (time_us_64() / 1000);
void updateCount()
{
#if ENCMODE == 2 || ENCMODE == 5 || ENCMODE == 8
  retstep1 = servoEncoder.detect_rotation(); // function returns a signed integer based on rotation step detected
  if (retstep1 != 0)
  {
    count = retstep1;
    printf("COUNT >> %d \n", count);
  }
#elif ENCMODE == 3 || ENCMODE == 6
  retstep1 = servoEncoder.detect_rotationHR(); // function returns a signed integer based on rotation step detected
  if (retstep1 != 0)
  {
    count += retstep1;
    printf("COUNT >> %d \n", count);
  }
#else
  retstep1 = servoEncoder.detect_rotation(); // function returns a signed integer based on rotation step detected
  if (retstep1 != 0)
  {
    count += retstep1; 
    //printf("COUNT >> %d \n", count); 
  }
  retstep2 = myKnob.detect_rotation(); // function returns a signed integer based on rotation step detected
  if (retstep2 != 0)
  {
    G_setpoint += float(retstep2);
     //printf("G_setpoint >> %f \n", G_setpoint);
  }

#endif
  if (prevcount != count)
  {

    if (abs(count) >= (ENC_BASERES / NUM_LEDS))
    {
      if (abs(count) % (ENC_BASERES / NUM_LEDS) == 0)
      {
        if (prevcount < count)
        {
          // led index to turn on
          if (activeledindex == (NUM_LEDS - 1))
          {
            activeledindex = 0;
          }
          else
          {
            activeledindex += 1;
          }
        }
        else if (prevcount > count)
        {
          // led index to turn on
          if (activeledindex == 0)
          {
            activeledindex = NUM_LEDS - 1;
          }
          else
          {
            activeledindex -= 1;
          }
        }

        updateLed(0, 200, 0, activeledindex);
         prevcount = count;
      }
    }
   
  }
  
}

int main()
{

  stdio_init_all();
  sleep_ms(500);
  pidSetup();

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  gpio_init(CT1);
  gpio_set_dir(CT1, GPIO_IN);
  gpio_pull_up(CT1);

  setpwmpins();

  PIO pio = pio0;
  int sm = 0;
  uint offset = pio_add_program(pio, &ws2812_program);
  // char str[12];

  ws2812_program_init(pio, sm, offset, ALED_PIN, 800000, false);

#if ENCMODE == 1
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_bound(BOUND);
  servoEncoder.initialize_encoder();
  servoEncoder.detect_rotationWithRate(true);
#elif ENCMODE == 2
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_bound(BOUND);
  servoEncoder.set_encoderResolution(ENC_BASERES);
  servoEncoder.initialize_encoder();
  // servoEncoder.invertCount(true);
  servoEncoder.setToStart();
#elif ENCMODE == 3
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  servoEncoder.set_bound(BOUND);
  servoEncoder.initialize_encoder();
#elif ENCMODE == 4
  setinterrupt();
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_bound(BOUND);
  servoEncoder.initialize_encoder();
#elif ENCMODE == 5
  setinterrupt();
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_bound(BOUND);
  servoEncoder.set_encoderResolution(ENC_BASERES);
  servoEncoder.initialize_encoder();
  // servoEncoder.invertCount(true);
  servoEncoder.setToStart();
#elif ENCMODE == 6
  setinterrupt();
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_adcresolution(ADC_RES);
  servoEncoder.set_resolution(EXTENDEDRESOLUTION_PERCENT);
  servoEncoder.set_bound(BOUND);
  servoEncoder.initialize_encoder();
#elif ENCMODE == 7
  setinterrupt();
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
   myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); 
  servoEncoder.initialize_encoder();
  myKnob.initialize_encoder();
#elif ENCMODE == 8
  setinterrupt();
  servoEncoder.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH); // set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
  servoEncoder.set_encoderResolution(ENC_BASERES);
  servoEncoder.initialize_encoder();
  // servoEncoder.invertCount(true);
  servoEncoder.setToStart();
#endif

  updateLed(255, 0, 0, activeledindex);

  while (true)
  {

    if (debug)
    {
      sleep_ms(100);
      servoEncoder.DebugEncoder();
    }

    if (gpio_get(CT1) == 0)
    {
      servoEncoder.setToStart();
    }

    readtobuffer();
    processinData();
    updateCount();
      if (((time_us_64() / 1000) - infotimer) > 200)
    {
      printf("ENCODER STEPS >> %d SETPOINT >> %f OUTPUT DEGREE >> %f \n", count,G_setpoint, (count*((360/ENC_BASERES)))/resolutionMultiplier);
    infotimer = (time_us_64() / 1000);
    }
    
    if (G_setpoint != count)
    {
       pidMove(G_setpoint);
    }
    else
    {
      // PWM_On(0, forMotor);
      // PWM_On(0, revMotor);
    }
  }
}
