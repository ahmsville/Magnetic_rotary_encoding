from machine import Pin, ADC, PWM, Timer
from MagneticRotaryEncoder import MagRotaryEncoder
import array,time
import rp2

#ENCMODE = 1 # BASE RESOLUTION ANALOG ONLY
#ENCMODE  = 2 # BASE RESOLUTION ANALOG ONLY ABSOLUTE POSITIONING
#ENCMODE  = 3 # EXTENDED RESOLUTION ANALOG ONLY
#ENCMODE  = 4 # BASE RESOLUTION ANALOG AND DIGITAL
#ENCMODE  = 5 # BASE RESOLUTION ANALOG AND DIGITAL ABSOLUTE POSITIONING
#ENCMODE  = 6 # EXTENDED RESOLUTION ANALOG AND DIGITAL
ENCMODE  = 7 # BASE RESOLUTION DIGITAL ONLY
#ENCMODE  = 8 # BASE RESOLUTION DIGITAL ONLY ABSOLUTE POSITIONING

E1_INT1 = 2  # hall sensor 1 interrupt pin 1
E1_INT2 = 3  # hall sensor 1 interrupt pin 2
E2_INT1 = 4  # hall sensor 2 interrupt pin 1
E2_INT2 = 5  # hall sensor 2 interrupt pin 2

E1_ADC_PIN = 26  # ADC PIN for hall sensor 1
E2_ADC_PIN = 27  # ADC PIN for hall sensor 2

E1_ADC_CH = 0  # rp2040 ADC ch 0 for E1
E2_ADC_CH = 1  # rp2040 ADC ch 1 for E2

ENC_BASERES = 36  # ENCODER BASE RESOLUTION

HAPTICS_PIN = 25       # ENCODER HAPTICS PIN
HAPTICS_STRENGTH = 65535  # ENCODER HAPTICS PWM DUTY CYCLE
HAPTICS_DURATION = 50  # ENCODER HAPTICS DURATION

ADC_RES = 16                    # ADC RESOLUTION 10 OR 12 BITS
BOUND = 4000                      # ADC NOISE ALLOWANCE
EXTENDEDRESOLUTION_PERCENT = 90  # PERCENTAGE OF FULL ITERPOLATED ENCODER RESOLUTION

CT1 = 24 # PUSH BUTTON (CT1)

# NUM_LEDS 15 # Num of leds on board
ALED_PIN = 8
NUM_LEDS = 18


if ENCMODE == 1:
    myKnob = MagRotaryEncoder(E1_ADC_PIN,E2_ADC_PIN)
elif ENCMODE == 4 or ENCMODE == 5 or ENCMODE == 6:
    myKnob = MagRotaryEncoder(E1_ADC_PIN,E2_ADC_PIN,E1_INT1, E2_INT1, False)
elif ENCMODE == 7 or ENCMODE == 8:
    myKnob = MagRotaryEncoder(E1_ADC_PIN,E2_ADC_PIN)
    myKnob.setExclINTmode(E1_INT1, E1_INT2, E2_INT1, E2_INT2, True)
else:
    myKnob = MagRotaryEncoder(E1_ADC_PIN,E2_ADC_PIN)


if ENCMODE == 1:
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_bound(BOUND)
    myKnob.initialize_encoder()
    myKnob.detect_rotationWithRate(True)
elif ENCMODE == 2:
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_bound(BOUND)
    myKnob.set_encoderResolution(ENC_BASERES)
    myKnob.initialize_encoder()
    myKnob.invertCount(True)
    myKnob.setToStart()
elif ENCMODE == 3:
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT)
    myKnob.set_bound(BOUND)
    myKnob.initialize_encoder()
elif ENCMODE == 4: 
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_bound(BOUND)
    myKnob.initialize_encoder()
elif ENCMODE == 5:
    
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_bound(BOUND)
    myKnob.set_encoderResolution(ENC_BASERES)
    myKnob.initialize_encoder()
    myKnob.invertCount(True)
    myKnob.setToStart()
elif ENCMODE == 6: 
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_adcresolution(ADC_RES)
    myKnob.set_resolution(EXTENDEDRESOLUTION_PERCENT)
    myKnob.set_bound(BOUND)
    myKnob.initialize_encoder()
elif ENCMODE == 7:
    
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.initialize_encoder()
    myKnob.invertCount(True)
elif ENCMODE == 8:
    
    myKnob.set_haptics(HAPTICS_PIN, HAPTICS_DURATION, HAPTICS_STRENGTH) # set haptic feedback variables (arduino pwm pin, duration of haptics(ms), pwn strength from 0-255)
    myKnob.set_encoderResolution(ENC_BASERES)
    myKnob.initialize_encoder()
    myKnob.invertCount(True)
    myKnob.setToStart()

brightness = 0.2

@rp2.asm_pio(sideset_init=rp2.PIO.OUT_LOW, out_shiftdir=rp2.PIO.SHIFT_LEFT, autopull=True, pull_thresh=24)
def ws2812():
    T1 = 2
    T2 = 5
    T3 = 3
    wrap_target()
    label("bitloop")
    out(x, 1)               .side(0)    [T3 - 1]
    jmp(not_x, "do_zero")   .side(1)    [T1 - 1]
    jmp("bitloop")          .side(1)    [T2 - 1]
    label("do_zero")
    nop()                   .side(0)    [T2 - 1]
    wrap()


# Create the StateMachine with the ws2812 program, outputting on pin
sm = rp2.StateMachine(0, ws2812, freq=8_000_000, sideset_base=Pin(ALED_PIN))

# Start the StateMachine, it will wait for data on its FIFO.
sm.active(1)

# Display a pattern on the LEDs via an array of LED RGB values.
ar = array.array("I", [0 for _ in range(NUM_LEDS)])

##########################################################################
BLACK = (0, 0, 0)
PURPLE = (180, 0, 255)
activeledindex = 0

def pixels_show():
    for i,c in enumerate(ar):
        r = int(((c >> 8) & 0xFF) * brightness)
        g = int(((c >> 16) & 0xFF) * brightness)
        b = int((c & 0xFF) * brightness) 
    sm.put(ar, 8)
    #time.sleep_us(110)

def pixels_set(i, color):
    ar[i] = (color[1]<<16) + (color[0]<<8) + color[2]

def updateLed(color, ledindex):
    for i in range(NUM_LEDS):
        if i == ledindex:
          pixels_set(i,PURPLE)
        else:
          pixels_set(i,BLACK)
        time.sleep_us(110)
    pixels_show()

count = 0
prevcount = 0
retstep1 = 0

buttonin = Pin(CT1, Pin.IN, Pin.PULL_UP)

debug = False
def printdata(timer):
    if debug:
        myKnob.DebugEncoder()
        ...

timer = Timer()
timer.init(freq=1, mode=Timer.PERIODIC, callback=printdata)
while True:
    if ENCMODE == 2 or ENCMODE == 5 or ENCMODE == 8:
        retstep1 = myKnob.detect_rotation(); # function returns a signed integer based on rotation step detected
        if retstep1 != 0:
            count = retstep1
            print("COUNT >> {}".format(count))
        
    elif ENCMODE == 3 or ENCMODE == 6:
        retstep1 = myKnob.detect_rotationHR(); # function returns a signed integer based on rotation step detected
        if retstep1 != 0:
            count += retstep1
            print("COUNT >> {}".format(count))
    else:
        retstep1 = myKnob.detect_rotation(); # function returns a signed integer based on rotation step detected
        if retstep1 != 0:
            count += retstep1
            print("COUNT >> {}".format(count))
    if buttonin.value() == 0:
        print("Button Pressed \n")
        print("setting start pos to {}".format(myKnob.setToStart()))
    
    if count > prevcount: #++    
      if activeledindex == (NUM_LEDS - 1):
        activeledindex = 0
      else:
        activeledindex += 1  
      for i in range(NUM_LEDS):
        ...
      updateLed(PURPLE, activeledindex)
      prevcount = count
    elif count < prevcount: #--    
      if activeledindex == 0:
        activeledindex = (NUM_LEDS - 1)
      else:
        activeledindex -= 1  
      for i in range(NUM_LEDS):
        ...
      updateLed(PURPLE, activeledindex)
      prevcount = count
    
    
 