from machine import Pin, ADC, PWM, Timer
import time


class MagRotaryEncoder:

    # self.sensor1_pin,self.sensor2_pin,s1dpin1,s1dpin2,s2dpin1,s2dpin2 = ADC(Pin(0)),ADC(Pin(0)),Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP)
    isr1, isr2, isr3, isr4 = 0, 0, 0, 0
    testcount = 0
    s1INTswitch, s2INTswitch = True, True
    opargs = [[0, 0], [0, 0]]
    startposition = 0
    sensor1_pin, sensor2_pin, useInterrupt, exclInterrupt = ADC(0), ADC(1), False, False
    SensorINTpin = [Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP),Pin(0, Pin.IN, Pin.PULL_UP)]
    SensorINTactualpin = [0,0,0,0]
    bound = 3000
    S1_Neutral, S2_Neutral, Neutral = [
        0, 0], [0, 0], [0, 0]
    S1_North, S1_South, S2_North, S2_South = 0, 0, 0, 0
    S1_absoluteneutral, S2_absoluteneutral, maxsway = 0,0,500
    northRegion, southRegion, stablerange, Mid = 0, 0, 100, 0
    absoluteNeutral, stepres, setHresolutionpercent = 32767, 100 , 100
    prevsensor1, prevsensor2, sensor1, sensor2 = 0, 0, 0, 0
    prevstartposition, count, tempcount, invertcount = 0, 0, 0, False
    use_extended_resolution, INT1fired, INT2fired = False, False, False
    encoderPosition, encoderResolution, inSync = 0, 0, False
    storedstartposition, storedsensor1state, storedsensor2state = 0, 0, 0
    use_extended_resolution, setresolution = False, 0
    haptics_duration, haptics_strength, haptics_ontime, haptics_state = 0, 0, 0, 0
    INTProcessed, INT1fired, INT2fired = True, False, False

    activesensorInterrupt = 1
    rotationrate, timetracker, timetomultiply, multiplier = 0, 0, 10, 10
    locksensorlevels = False
    alpha, prevsmoothsensor1, prevsmoothsensor2 = 0.01, 0, 0
    processS1_S_INT, processS1_N_INT, processS2_S_INT, processS2_N_INT =True, True, True, True 
    flagState = 0
    returnRateCount = False

    def __init__(self, *args):
        if len(args) == 2:
            self.sensor1_pin, self.sensor2_pin = ADC(Pin(args[0])), ADC(
                    Pin(args[1]))
            ...
        elif len(args) == 5:
            if args[4] == False:
                self.sensor1_pin, self.sensor2_pin = ADC(Pin(args[0])), ADC(
                    Pin(args[1]))
                self.SensorINTpin[0] = Pin(args[2], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[0].irq(trigger=Pin.IRQ_RISING, handler=self.singleSensor_INT)
                self.SensorINTpin[1] = Pin(self.args[3], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[1].irq(trigger=Pin.IRQ_RISING, handler=self.singleSensor_INT)
                self.useInterrupt = True
            elif args[4] == True:
                self.SensorINTpin[2] = Pin(args[0], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[2].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
                self.SensorINTpin[0] = Pin(args[1], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[0].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
                self.SensorINTpin[1] = Pin(args[2], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[1].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
                self.SensorINTpin[3] = Pin(self.args[3], Pin.IN, Pin.PULL_UP)
                self.SensorINTpin[3].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
                self.useInterrupt = True
                self.exclInterrupt = True
        else:
            raise Exception(
                "invalid arguments, expected two or five received  {}".format(len(args)))

    def mainISR(self,gpiopin): # 
        if gpiopin == self.get_sensorINTpin(3):
            self.processExINT(3)
            #print("3")
        elif gpiopin == self.get_sensorINTpin(1):
            self.processExINT(1)
            #print("1")
        elif gpiopin == self.get_sensorINTpin(2):
            self.processExINT(2)
            #print("2")
        elif gpiopin == self.get_sensorINTpin(4):
            self.processExINT(4)
            #print("4")
        ...
    def setExclINTmode(self, *args):
        if len(args) == 5:
            self.SensorINTpin[2] = Pin(args[0], Pin.IN, Pin.PULL_UP)
            self.SensorINTpin[2].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
            self.SensorINTpin[0] = Pin(args[1], Pin.IN, Pin.PULL_UP)
            self.SensorINTpin[0].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
            self.SensorINTpin[1] = Pin(args[2], Pin.IN, Pin.PULL_UP)
            self.SensorINTpin[1].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
            self.SensorINTpin[3] = Pin(args[3], Pin.IN, Pin.PULL_UP)
            self.SensorINTpin[3].irq(trigger=Pin.IRQ_RISING, handler=self.mainISR)
            self.useInterrupt = True
            self.exclInterrupt = True
        else:
            raise Exception(
                "invalid arguments, expected two or five received  {}".format(len(args)))


    def invertCount(self, inv):
        self.invertcount = inv



    def set_adcresolution(self,res):
        if res == 12:
        
            self.ADCMaxRes = 4095
            self.absoluteNeutral = 2048
            self.bound = 400
            self.maxsway = 252
        
        elif (res == 10):
            self.ADCMaxRes = 1023
            self.absoluteNeutral = 512
            self.bound = 10
            self.maxsway = 50
        elif (res == 16):
            self.ADCMaxRes = 65535
            self.absoluteNeutral = 32767
            self.bound = 6000
            self.maxsway = 1000
	

    def set_poleStateValues(self, sensornum, np, nu, sp):
        if sensornum == 1:
            self.S1_Neutral[0] = np - self.bound
            self.S1_Neutral[1] = np + self.bound
            self.S1_North = np
            self.S1_South = sp
            self.S1_absoluteneutral = nu
        elif sensornum == 2:
            self.S2_Neutral[0] = np - self.bound
            self.S2_Neutral[1] = np + self.bound
            self.S2_North = np
            self.S2_South = sp
            self.S2_absoluteneutral = nu
        self.southRegion = self.Neutral[1] + self.stablerange
        self.northRegion = self.Neutral[0] - self.stablerange
        self.Neutral[0] = nu - self.bound
        self.Neutral[1] = nu + self.bound
        self.Mid = nu

    def initialize_encoder(self):

        self.set_poleStateValues(1, self.absoluteNeutral - self.bound - self.stepres, self.absoluteNeutral, self.absoluteNeutral +
                                 self.bound + self.stepres)  # set the peak ADC values for the (sensor1, northpole, neutralstate, southpole)
        self.set_poleStateValues(2, self.absoluteNeutral - self.bound - self.stepres, self.absoluteNeutral, self.absoluteNeutral +
                                 self.bound + self.stepres)  # set the peak ADC values for the (sensor2, northpole, neutralstate, southpole)
        self.get_sensorValue(1)
        self.get_sensorValue(2)
        self.recaliberate_startPosition()
        self.prevsensor1 = self.sensor1
        self.prevsensor2 = self.sensor2

    def CalibrateSensors(self, snum):

        slevel = 0
        self.locksensorlevels = True
        if snum == 1:

            for i in range(10):
                slevel += self.sensor1_pin.read_u16()

            self.S1_absoluteneutral = slevel / 10
            self.S1_Neutral[0] = self.S1_absoluteneutral - self.bound
            self.S1_Neutral[1] = self.S1_absoluteneutral + self.bound
            return self.S1_absoluteneutral

        elif snum == 2:

            for i in range(10):
                slevel += self.sensor2_pin.read_u16()

            self.S2_absoluteneutral = slevel / 10
            self.S2_Neutral[0] = self.S2_absoluteneutral - self.bound
            self.S2_Neutral[1] = self.S2_absoluteneutral + self.bound
            return self.S2_absoluteneutral

        return 0

    def LoadCalibrationData(self, s1, s2):

        self.S1_absoluteneutral = self.S1
        self.S1_Neutral[0] = self.S1_absoluteneutral - self.bound
        self.S1_Neutral[1] = self.S1_absoluteneutral + self.bound

        self.S2_absoluteneutral = self.S2
        self.S2_Neutral[0] = self.S2_absoluteneutral - self.bound
        self.S2_Neutral[1] = self.S2_absoluteneutral + self.bound

        self.locksensorlevels = True
        return True

    def get_sensorValue(self, sennum):
        if sennum == 1:
            self.sensor1 = self.sensor1_pin.read_u16()
            if not self.locksensorlevels:

                if self.sensor1 > self.S1_South or self.sensor1 < self.S1_North:
                    self.sensor2 = self.sensor2_pin.read_u16()
                    if self.sensor2 < self.absoluteNeutral + self.maxsway and self.sensor2 > self.absoluteNeutral - self.maxsway:
                        if self.sensor1 > self.S1_South:
                            self.S1_South = self.sensor1
                            self.S2_absoluteneutral = self.sensor2
                            self.S2_Neutral[0] = self.S2_absoluteneutral - \
                                self.bound
                            self.S2_Neutral[1] = self.S2_absoluteneutral + \
                                self.bound
                            if self.use_extended_resolution:
                                self.calibrateStepRes_HR()
                        elif self.sensor1 < self.S1_North:
                            self.S1_North = self.sensor1
                            self.S2_absoluteneutral = self.sensor2
                            self.S2_Neutral[0] = self.S2_absoluteneutral - \
                                self.bound
                            self.S2_Neutral[1] = self.S2_absoluteneutral + \
                                self.bound
                            if self.use_extended_resolution:

                                self.calibrateStepRes_HR()

            if self.use_extended_resolution:
                self.sensor1 = (self.sensor1 * self.alpha) + \
                    (self.prevsmoothsensor1 * (1 - self.alpha))
                self.prevsmoothsensor1 = self.sensor1
            return self.sensor1
        elif sennum == 2:
            self.sensor2 = self.sensor2_pin.read_u16()
            if not self.locksensorlevels:

                if self.sensor2 > self.S2_South or self.sensor2 < self.S2_North:
                    self.sensor1 = self.sensor1_pin.read_u16()
                    if self.sensor1 < self.absoluteNeutral + self.maxsway and self.sensor1 > self.absoluteNeutral - self.maxsway:
                        if self.sensor2 > self.S2_South:
                            self.S2_South = self.sensor2
                            self.S1_absoluteneutral = self.sensor1
                            self.S1_Neutral[0] = self.S1_absoluteneutral - \
                                self.bound
                            self.S1_Neutral[1] = self.S1_absoluteneutral + \
                                self.bound
                            if self.use_extended_resolution:
                                self.calibrateStepRes_HR()
                        elif self.sensor2 < self.S2_North:
                            self.S2_North = self.sensor2
                            self.S1_absoluteneutral = self.sensor1
                            self.S1_Neutral[0] = self.S1_absoluteneutral - \
                                self.bound
                            self.S1_Neutral[1] = self.S1_absoluteneutral + \
                                self.bound
                            if self.use_extended_resolution:

                                self.calibrateStepRes_HR()

            if self.use_extended_resolution:
                self.sensor2 = (self.sensor2 * self.alpha) + \
                    (self.prevsmoothsensor2 * (1 - self.alpha))
                self.prevsmoothsensor2 = self.sensor2
            return self.sensor2

    def recaliberate_startPosition(self):
        if self.haptics_state == 1:
            self.haptics(0)
        if not self.useInterrupt or self.use_extended_resolution:  # interrupt detection is not used
            self.startposition = self.get_encodingState()
            if self.prevstartposition != self.startposition:  # if startposition didnt change
                if self.startposition == 1 or self.startposition == 3:
                    self.prevsensor2 = self.Mid
                elif self.startposition == 2 or self.startposition == 4:
                    self.prevsensor1 = self.Mid
                self.prevstartposition = self.startposition
            self.count = 0

        if self.startposition == 0:  # STARTING
            if not self.exclInterrupt:
                self.get_sensorValue(1)  # read sensor 1
                self.get_sensorValue(2)  # read sensor 2
                comp1 = self.sensor1 - self.absoluteNeutral
                comp2 = self.sensor2 - self.absoluteNeutral
                if comp1 < 0:

                    comp1 = comp1 * (-1)

                if comp2 < 0:

                    comp2 = comp2 * (-1)

                if comp1 < comp2:  # sensor1 is in neutral

                    # set new startposition
                    # sensor1 = neutral , sensor2 = south
                    if self.sensor2 > self.Neutral[1]:
                        self.startposition = 2
                        self.prevstartposition = 2
                        self.prevsensor1 = self.Mid

                    # sensor1 = neutral , sensor2 = north
                    elif self.sensor2 < self.Neutral[0]:
                        self.startposition = 4
                        self.prevstartposition = 4
                        self.prevsensor1 = self.Mid

                elif comp2 < comp1:  # sensor2 is in neutral

                    # set new startposition
                    # sensor2 = neutral , sensor1 = south
                    if self.sensor1 > self.Neutral[1]:
                        self.startposition = 3
                        self.prevstartposition = 3
                        self.prevsensor2 = self.Mid

                    # sensor2 = neutral , sensor1 = north
                    elif self.sensor1 < self.Neutral[0]:
                        self.startposition = 1
                        self.prevstartposition = 1
                        self.prevsensor2 = self.Mid
            else:
                self.startposition = self.get_encodingState()

    def get_encodingState(self, *args):
        if len(args) == 0:
            if not self.exclInterrupt:  # analog only detection
                # sensor1 = north , sensor2 = neutral
                if self.sensor1 < self.S1_Neutral[0] and self.sensor2 >= self.S2_Neutral[0] and self.sensor2 <= self.S2_Neutral[1]:
                    return 1

                # sensor1 = neutral , sensor2 = south
                elif self.sensor1 >= self.S1_Neutral[0] and self.sensor1 <= self.S1_Neutral[1] and self.sensor2 > self.S2_Neutral[1]:
                    return 2

                # sensor1 = south , sensor2 = neutral
                elif self.sensor1 > self.S1_Neutral[1] and self.sensor2 >= self.S2_Neutral[0] and self.sensor2 <= self.S2_Neutral[1]:
                    return 3

                # sensor1 = neutral , sensor2 = north
                elif self.sensor1 >= self.S1_Neutral[0] and self.sensor1 <= self.S1_Neutral[1] and self.sensor2 < self.S2_Neutral[0]:
                    return 4
                
                return self.startposition
            else:  # digital only detection
                ...
                return self.startposition

        elif len(args) == 1:
            if self.SensorINTpin[0].value() != self.SensorINTpin[1].value():
                if args[0] == 1:

                    # sensor1 = neutral , sensor2 = south
                    if self.sensor2 > self.Neutral[1]:
                        return 2

                    # sensor1 = neutral , sensor2 = north
                    elif self.sensor2 < self.Neutral[0]:
                        return 4

                elif args[0] == 2:
                    # sensor2 = neutral , sensor1 = south
                    if self.sensor1 > self.Neutral[1]:
                        return 3

                    # sensor2 = neutral , sensor1 = north
                    elif self.sensor1 < self.Neutral[0]:
                        return 1
            return self.startposition

    def get_encResCount(self, retstep):
        # keep counter value in the encoder's resolution range
        if retstep < 0:
            for i in range(-retstep):
                if (self.encoderPosition - 1) >= 1:
                    self.encoderPosition = self.encoderPosition - 1

                elif (self.encoderPosition - 1) < 1:
                    self.encoderPosition = self.encoderResolution

        elif retstep > 0:
            for i in range(retstep):
                if (self.encoderPosition + 1) <= self.encoderResolution:
                    self.encoderPosition = self.encoderPosition + 1

                elif (self.encoderPosition + 1) > self.encoderResolution:
                    self.encoderPosition = 1

        # validate stored start point data

        if self.encoderPosition == 1:
            passmark = 0
            self.inSync = False
            if self.storedstartposition == self.startposition:
                passmark += 1

            if passmark == 1:
                self.inSync = True

        if self.inSync:
            return self.encoderPosition

        else:
            self.encoderPosition = -1
            return self.encoderPosition

    def set_encoderResolution(self, res):
        self.encoderResolution = res
        self.encoderPosition = 0
        self.storedstartposition = 0
        self.storedsensor1state = 0
        self.storedsensor2state = 0
        self.inSync = False

    def get_sensorINTpin(self,sensornum):

        if sensornum == 1:

            return self.SensorINTpin[0]

        elif (sensornum == 2):

            return self.SensorINTpin[1]

        elif (sensornum == 3):

            return self.SensorINTpin[2]

        elif (sensornum == 4):

            return self.SensorINTpin[3]
 
        return 0

    def setToStart(self):
        self.encoderPosition = 1
        self.startposition = 0
        self.get_sensorValue(1)
        self.get_sensorValue(2)
        self.recaliberate_startPosition()
        self.count = 0
        if not self.exclInterrupt:
            if self.startposition != 0:
                self.storedstartposition = self.startposition
                self.storedsensor1state = self.sensor1
                self.storedsensor2state = self.sensor2
                self.inSync = True
                return self.startposition
            else:
                return -1
        else:
            print("waiting for interrupt......")
            while self.startposition == 0:
                self.recaliberate_startPosition()
            self.storedstartposition = self.startposition
            self.storedsensor1state = self.sensor1
            self.storedsensor2state = self.sensor2
            self.inSync = True
            return self.startposition
        


    def setrotationRate(self):

        self.rotationrate = time.ticks_ms() - self.timetracker
        self.timetracker = time.ticks_ms();


    def applyrateMultiplier(self):
        if self.rotationrate > 0:
            # avoid negative values
            if (self.rotationrate < self.timetomultiply):
                # not fast anough for multiplier
                calcmul = 1 - self.rotationrate / self.timetomultiply
                self.count = self.count * self.multiplier * calcmul
		
	


    def set_resolution(self, percent):
        if percent == 100:
            self.setresolution = 1
            self.use_extended_resolution = True

        elif percent == 0:
            self.use_extended_resolution = False

        else:
            self.setresolution = self.stepres - \
                ((percent * 0.01) * self.stepres)
            self.use_extended_resolution = True
        self.setHresolutionpercent = percent

    def calibrateStepRes_HR(self):
        self.stepres = self.S1_absoluteneutral - self.S1_South
        if (self.stepres < 0): # make sure its not negative
            self.stepres *= (-1)
        self.set_resolution(self.setHresolutionpercent)

    
    def rotation_action(self, act):  # sets action for clockwise and anticlockwise rotations

        if act == 1:
            self.count += 1

        elif act == 0:
            self.count -= 1

    def get_bound(self, sensor_num):  # calculate ideal bound value for hall sensor

        self.use_extended_resolution = False
        loops = 2000
        minval = 10000
        maxval = 0
        ret = 0
        print("reading ADC")

        if self.useInterrupt:

            print("\n")
            print("waiting for interrupt - rotate the knob...\n")
            # wait for interrupt
            while self.get_sensorINTpin(sensor_num).value() == 0:
                ...

            for i in range(10000):

                print("interrupt detected, reading sensor values\n")
                # get upper and lower limit of center region for analog and digital detection
                while self.get_sensorINTpin(sensor_num).value() == 1:
                    ret = self.get_sensorValue(sensor_num)
                    if ret < minval:
                            minval = ret
                    if ret > maxval:
                            maxval = ret
                    if i % (loops / 100) == 0:
                            print(".")
            use_extended_resolution = False

        else:
            for i in range(loops):
                # get upper and lower noise limits for abalog only detection
                ret = self.get_sensorValue(sensor_num)
                if ret < minval:
                    minval = ret
                if ret > maxval:
                    maxval = ret
                sleep_ms(10)
                if i % (loops / 10) == 0:
                    print(".")
            self.use_extended_resolution = True
        print("\n")
        print("maximum value >> {} minimum value >> {}  \n", maxval, minval)
        idealbound = maxval - minval
        print("ideal bound >> {} \n", idealbound)

        return idealbound

    def set_bound(self, b):  # this value determines the upper and lower limit of the ADC values
        self.bound = b
        self.S1_Neutral[0] = self.S1_absoluteneutral - self.bound;
        self.S1_Neutral[1] = self.S1_absoluteneutral + self.bound;
        self.S2_Neutral[0] = self.S2_absoluteneutral - self.bound;
        self.S2_Neutral[1] = self.S2_absoluteneutral + self.bound;
        self.Neutral[0] = self.absoluteNeutral - self.bound
        self.Neutral[1] = self.absoluteNeutral + self.bound
        

    def useinterruptdetection(self, act):
        self.useInterrupt = act

    # use to set haptics variables (pwm pin, duration of haptics(ms), strength from 0-255)
    def set_haptics(self, pin, duration, strength):
        global hapticscon
        hapticscon = PWM(Pin(pin))
        hapticscon.freq(1000)
        self.haptics_duration = duration
        self.haptics_strength = strength

    def haptics(self, state):  # viberation feedback function
        if state == 1:
            hapticscon.duty_u16(self.haptics_strength)
            self.haptics_ontime = time.ticks_ms()
            self.haptics_state = 1

        else:
            timediff = time.ticks_diff(time.ticks_ms(), self.haptics_ontime)
            if timediff >= self.haptics_duration and self.haptics_state == 1:
                hapticscon.duty_u16(0)
                self.haptics_state = 0

    def passSynctest(self, newspos):
        if self.encoderResolution > 0:
            if self.startposition == 1:
                if newspos == 4 or newspos == 2:
                    return True
            elif self.startposition == 2:
                if newspos == 1 or newspos == 3:
                    return True
            elif self.startposition == 3:
                if newspos == 2 or newspos == 4:
                    return True
            elif self.startposition == 4:
                if newspos == 3 or newspos == 1:
                    return True
            return False
        else:
            return True

    def processCOUNT(self, prevpos, newpos):
        if newpos == 1:
            if prevpos == 4:
                self.count += 1  # clockwise
                self.setrotationRate()
                self.haptics(1)
            elif prevpos == 2:
                self.count -= 1  # counterclockwise
                self.setrotationRate()
                self.haptics(1)

        elif newpos == 2:
            if prevpos == 1:
                self.count += 1  # clockwise
                self.setrotationRate()
                self.haptics(1)
            elif prevpos == 3:
                self.count -= 1  # counterclockwise
                self.setrotationRate()
                self.haptics(1)
            
        elif newpos == 3:
            if prevpos == 2:
                self.count += 1  # clockwise
                self.setrotationRate()
                self.haptics(1)
            elif prevpos == 4:
                self.count -= 1  # counterclockwise
                self.setrotationRate()
                self.haptics(1)
            
        elif newpos == 4:
            if prevpos == 3:
                self.count += 1  # clockwise
                self.setrotationRate()
                self.haptics(1)
            elif prevpos == 1:
                self.count -= 1  # counterclockwise
                self.setrotationRate()
                self.haptics(1)
        return self.count
                
    def processPULSE(self,currentpos):
	#************************************************************************#
        if currentpos == 4: #sensor 2 active check overlap on sensor one
            #CW
            if self.get_sensorINTpin(3).value() == 1: # state 3 - pin still high
                self.processS2_N_INT = False # disable yourown state
                self.processS1_S_INT = False
                self.processS2_S_INT = True
                self.processS1_N_INT = True
                #set indeterminate flag
                self.flagState = 3
            
            #CCW
            elif self.get_sensorINTpin(1).value() == 1: # state 1 - pin still high
                self.processS2_N_INT = False # disable yourown state
                self.processS1_N_INT = False
                self.processS2_S_INT = True
                self.processS1_S_INT = True
                #set indeterminate flag
                self.flagState = 1
            
            else: #determinate state
                self.processS2_N_INT = False # disable yourown sensor group
                self.processS1_S_INT = True
                self.processS2_S_INT = False
                self.processS1_N_INT = True
                #set determinate flag
                self.flagState = 0
            
        
        if currentpos == 2: #sensor 2 active check overlap on sensor one
            #CW
            if self.get_sensorINTpin(1).value() == 1: # state 1 - pin still high
                self.processS2_S_INT = False # disable yourown state
                self.processS1_N_INT = False
                self.processS2_N_INT = True
                self.processS1_S_INT = True
                #set indeterminate flag
                self.flagState = 1
            
            #CCW
            elif self.get_sensorINTpin(3).value() == 1: # state 3 - pin still high
                self.processS2_S_INT = False # disable yourown state
                self.processS1_S_INT = False
                self.processS2_N_INT = True
                self.processS1_N_INT = True
                #set indeterminate flag
                self.flagState = 3
            
            else: #determinate state
                self.processS2_N_INT = False # disable yourown sensor group
                self.processS1_S_INT = True
                self.processS2_S_INT = False
                self.processS1_N_INT = True
                #set determinate flag
                self.flagState = 0
            
        
        if currentpos == 3: #sensor 1 active check overlap on sensor two
        
            #CW
            if self.get_sensorINTpin(2).value() == 1: # state 2 - pin still high
            
                self.processS1_S_INT = False # disable yourown state
                self.processS2_S_INT = False
                self.processS2_N_INT = True
                self.processS1_N_INT = True
                #set indeterminate flag
                self.flagState = 2
            
            #CCW
            elif self.get_sensorINTpin(4).value() == 1: # state 4 - pin still high
            
                self.processS1_S_INT = False # disable yourown state
                self.processS2_N_INT = False
                self.processS2_S_INT = True
                self.processS1_N_INT = True
                #set indeterminate flag
                self.flagState = 4
            
            else:  #determinate state
                self.processS2_N_INT = True # disable yourown sensor group
                self.processS1_S_INT = False
                self.processS2_S_INT = True
                self.processS1_N_INT = False
                #set determinate flag
                self.flagState = 0
            
        
        if currentpos == 1: #sensor 1 active check overlap on sensor two
            #CW
            if self.get_sensorINTpin(4).value() == 1: # state 4 - pin still high
            
                self.processS1_N_INT = False # disable yourown state
                self.processS2_N_INT = False
                self.processS2_S_INT = True
                self.processS1_S_INT = True
                #set indeterminate flag
                self.flagState = 4
            #CCW
            elif self.get_sensorINTpin(2).value() == 1: # state 2 - pin still high
                self.processS1_N_INT = False # disable yourown state
                self.processS2_S_INT = False
                self.processS2_N_INT = True
                self.processS1_S_INT = True
                #set indeterminate flag
                self.flagState = 2
            else:  #determinate state
                self.processS2_N_INT = True # disable yourown sensor group
                self.processS1_S_INT = False
                self.processS2_S_INT = True
                self.processS1_N_INT = False
                #set determinate flag
                self.flagState = 0
            return self.flagState
 
    def singleSensor_INT(self,incenter):
        if not self.use_extended_resolution: # extended res is not used
            self.get_sensorValue(2) # read sensor 2
            self.get_sensorValue(1)
            # set new startposition
            if self.sensor2 != 0:
             # value read successfully, process here
                temppos = self.get_encodingState()
                if self.temppos == self.prevstartposition:
                
                    self.activesensorINT = 1
                    # INTProcessed = False;
                    return self.activesensorINT
                if self.passSynctest(temppos):
                    self.startposition = temppos
                    self.prevsensor1 = self.Mid
                    self.INTProcessed = True
                    # keep count
                    self.rotationrate = time.ticks_ms() - self.timetracker # rotation step rate
                    self.processCOUNT(self.prevstartposition,self.startposition)
                    self.prevstartposition = self.startposition
                    timetracker = time.ticks_ms()

                    self.activesensorINT = 2
                    return self.activesensorINT
                
                else:
                    self.count = 10000
                    self.inSync = False
                    return self.activesensorINT

            else:# process in loop
                self.INTProcessed = False
                self.INT1fired = True
                self.INT2fired = False
                self.rotationrate = time.ticks_ms() - self.timetracker # rotation step rate
                self.timetracker = time.ticks_ms()
                self.activesensorINT = 2
                return self.activesensorINT

        else:
            self.recaliberate_startPosition()
        self.activesensorINT = 1
        return self.activesensorINT

    def processExINT(self, encstate):
        if self.startposition == 0:
        
            self.startposition = encstate
            return 0
        
        if self.startposition != encstate:
            self.rotationrate = time.ticks_diff(time.ticks_ms(), self.timetracker) # rotation step rate

            if encstate == 1 and self.processS1_N_INT:
                if self.flagState != 0: #encoder reversed interpolate between states
                    if self.passSynctest(encstate):
                        self.processCOUNT(self.startposition, encstate)
                    
                    else:
                        self.processCOUNT(self.startposition, self.flagState)
                        self.processCOUNT(self.flagState, encstate)

                else:
                    self.processCOUNT(self.startposition, encstate)
                
                self.processPULSE(encstate)
                
                self.timetracker = time.ticks_ms()
                self.startposition = encstate
            elif encstate == 3 and self.processS1_S_INT:
                if self.flagState != 0: #encoder reversed interpolate between states
                    if self.passSynctest(encstate):
                        self.processCOUNT(self.startposition, encstate)
                    
                    else:
                        self.processCOUNT(self.startposition, self.flagState)
                        self.processCOUNT(self.flagState, encstate)

                else:
                    self.processCOUNT(self.startposition, encstate)
                
                self.processPULSE(encstate)
                
                self.timetracker = time.ticks_ms()
                self.startposition = encstate
            elif encstate == 2 and self.processS2_S_INT:
                if self.flagState != 0: #encoder reversed interpolate between states
                    if self.passSynctest(encstate):
                        self.processCOUNT(self.startposition, encstate)
                    
                    else:
                        self.processCOUNT(self.startposition, self.flagState)
                        self.processCOUNT(self.flagState, encstate)

                else:
                    self.processCOUNT(self.startposition, encstate)
                
                self.processPULSE(encstate)
                
                self.timetracker = time.ticks_ms()
                self.startposition = encstate
            elif encstate == 4 and self.processS2_N_INT:
                if self.flagState != 0: #encoder reversed interpolate between states
                    if self.passSynctest(encstate):
                        self.processCOUNT(self.startposition, encstate)
                    
                    else:
                        self.processCOUNT(self.startposition, self.flagState)
                        self.processCOUNT(self.flagState, encstate)

                else:
                    self.processCOUNT(self.startposition, encstate)
                
                self.processPULSE(encstate)
                
                self.timetracker = time.ticks_ms()
                self.startposition = encstate
            if self.startposition == encstate and self.count == 0:
                if self.encoderResolution > 0:
                    print("missed step\n")
                    #count = 10000
                    #inSync = false     
        return self.startposition


    def detect_rotation(self):  # openloop rotation encoding function

        if not self.useInterrupt:  # interrupt detection is not used

            self.get_sensorValue(1)
            self.get_sensorValue(2)
            newstate = self.get_encodingState()
            if newstate != self.startposition:
                if not self.passSynctest(newstate):
                
                    self.count = 10000
                    self.inSync = False
            self.processCOUNT(self.startposition,newstate)
		
       
            if self.count != 0:
                if self.invertcount:
                    self.count *= -1

                if self.encoderResolution > 0:

                    if self.inSync:

                        self.tempcount = self.count
                        self.recaliberate_startPosition()
                        self.tempcount = self.get_encResCount(self.tempcount)
                        # print("count {} encpos {} startpos {}".format(self.count,self.tempcount,self.startposition))

                    else:
                        self.recaliberate_startPosition()
                        self.tempcount = -1
                else:
                    if self.returnRateCount:
                        self.applyrateMultiplier()
                    self.tempcount = self.count
                    self.recaliberate_startPosition()
                
                
                self.count = 0
                return self.tempcount

            else:
                self.tempcount = self.count
                self.recaliberate_startPosition()
                return 0

        else:
            if not self.exclInterrupt:
                if self.INTProcessed:  # processed in interrupt
                    # print("south{}.north{}.count {}.startpos {}.".format(self.S1_South,self.S1_North,self.count,self.startposition))
                    if self.count != 0:
                        if self.invertcount:
                            self.count *= -1
                        if self.encoderResolution > 0:

                            if self.inSync:

                                self.tempcount = self.count
                                self.tempcount = self.get_encResCount(
                                    self.tempcount)
                                # print("count {} encpos {} startpos {}".format(self.count,self.tempcount,self.startposition))

                            else:
                                self.tempcount = -1

                        else:
                            if self.returnRateCount:
                                self.applyrateMultiplier()
                            self.tempcount = self.count

                        self.count = 0
                        return self.tempcount

                    else:
                        self.tempcount = self.count
                        self.recaliberate_startPosition()
                        return 0

                else:

                    if self.SensorINTpin[1].value() == 0:  # int1 active

                        self.get_sensorValue(2)
                        self.startposition = self.get_encodingState(1)
                        if self.startposition != self.prevstartposition:

                            self.processCOUNT(self.prevstartposition,self.startposition)
                            self.prevstartposition = self.startposition
                            self.INTProcessed = True

                    elif self.SensorINTpin[0].value() == 0:  # int2 active
                        self.get_sensorValue(1)
                        self.startposition = self.get_encodingState(2)
                        if self.startposition != self.prevstartposition:

                            self.processCOUNT(self.prevstartposition,self.startposition)
                            self.prevstartposition = self.startposition
                            self.INTProcessed = True

                    if self.count != 0:
                        if self.invertcount:

                            self.count *= -1

                        if self.encoderResolution > 0:

                            if self.inSync:

                                self.tempcount = self.count
                                self.tempcount = self.get_encResCount(
                                    self.tempcount)

                            else:
                                self.tempcount = -1

                        else:
                            if self.returnRateCount:
                                self.applyrateMultiplier()
                            self.tempcount = self.count
                            self.recaliberate_startPosition()

                        self.count = 0
                        return self.tempcount

                    else:
                        self.tempcount = self.count
                        self.recaliberate_startPosition()
                        return 0
            else:
                if self.count != 0:
                    if self.invertcount:
                        self.count *= -1

                    if self.encoderResolution > 0:

                        if self.inSync:

                            self.tempcount = self.count
                            self.tempcount = self.get_encResCount(
                                self.tempcount)
                            # print("count {} encpos {} startpos {}".format(self.count,self.tempcount,self.startposition))

                        else:
                            self.tempcount = -1
                    else:
                        if self.returnRateCount:
                            self.applyrateMultiplier()
                        self.tempcount = self.count

                    self.count = 0
                    return self.tempcount

                else:
                    self.tempcount = self.count
                    #self.recaliberate_startPosition()
                    return 0

    def detect_rotationWithRate(self,act):  # rate rotation encoding function
        self.returnRateCount = act
    def detect_rotationHR(self):  # openloop rotation encoding function
        # print(self.setresolution)
        if not self.useInterrupt or self.use_extended_resolution:  # nterrupt detection is not used
            if self.startposition == 1:  # sensor2 is in neutral
                self.get_sensorValue(1)
                self.get_sensorValue(2)
                # analog value is not in the self.Neutral range
                if not (self.sensor2 < self.S2_Neutral[1] and self.sensor2 > self.S2_Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor2 != self.Mid:
                        if self.sensor2 > (self.prevsensor2 + self.setresolution) or self.sensor2 < (self.prevsensor2 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor2 - self.sensor2
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor2 = self.sensor2 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor2 = self.sensor2 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                    else:
                        self.prevsensor2 = self.sensor2

            elif self.startposition == 2:  # sensor1 is in self.Neutral
                self.get_sensorValue(1)
                self.get_sensorValue(2)
                # analog value is not in the neutral range
                if not (self.sensor1 < self.S1_Neutral[1] and self.sensor1 > self.S1_Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor1 != self.Mid:
                        # check for actual rotation
                        if self.sensor1 > (self.prevsensor1 + self.setresolution) or self.sensor1 < (self.prevsensor1 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor1 - self.sensor1
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor1 = self.sensor1 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor1 = self.sensor1 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                    else:
                        self.prevsensor1 = self.sensor1

            elif self.startposition == 3:  # sensor2 is in neutral
                self.get_sensorValue(1)
                self.get_sensorValue(2)
                # analog value is not in the neutral range
                if not (self.sensor2 < self.S2_Neutral[1] and self.sensor2 > self.S2_Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor2 != self.Mid:
                        # check for actual rotation
                        if self.sensor2 > (self.prevsensor2 + self.setresolution) or self.sensor2 < (self.prevsensor2 - self.setresolution):
                            distance = self.prevsensor2 - self.sensor2
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor2 = self.sensor2 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor2 = self.sensor2 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                    else:
                        self.prevsensor2 = self.sensor2

            elif self.startposition == 4:  # sensor1 is in neutral
                self.get_sensorValue(1)
                self.get_sensorValue(2)
                # analog value is not in the neutral range
                if not (self.sensor1 < self.S1_Neutral[1] and self.sensor1 > self.S1_Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor1 != self.Mid:
                        # check for actual rotation
                        if self.sensor1 > (self.prevsensor1 + self.setresolution) or self.sensor1 < (self.prevsensor1 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor1 - self.sensor1
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor1 = self.sensor1 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor1 = self.sensor1 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                    else:
                        self.prevsensor1 = self.sensor1

            else:
                self.get_sensorValue(1)
                self.get_sensorValue(2)

            tempcount = self.count
            self.recaliberate_startPosition()
            return tempcount

        else:  # interrupt assisted
            if self.INT1fired:
                self.get_sensorValue(2)
                self.startposition = self.get_encodingState(1)

            elif self.INT2fired:
                self.get_sensorValue(1)
                self.startposition = self.get_encodingState(2)

            if self.startposition == 1:  # sensor2 is in neutral
                # self.get_sensorValue(1)
                self.get_sensorValue(2)

                # analog value is not in the neutral range
                if not (self.sensor2 < self.Neutral[1] and self.sensor2 > self.Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor2 != self.Mid:
                        if self.sensor2 > (self.prevsensor2 + self.setresolution) or self.sensor2 < (self.prevsensor2 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor2 - self.sensor2
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor2 = self.sensor2 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor2 = self.sensor2 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                    else:
                        self.prevsensor2 = self.sensor2

            elif self.startposition == 2:  # sensor1 is in neutral

                self.get_sensorValue(1)
                # self.get_sensorValue(2)
                # analog value is not in the neutral range
                if not (self.sensor1 < self.Neutral[1] and self.sensor1 > self.Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor1 != self.Mid:
                        # check for actual rotation
                        if self.sensor1 > (self.prevsensor1 + self.setresolution) or self.sensor1 < (self.prevsensor1 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor1 - self.sensor1
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor1 = self.sensor1 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor1 = self.sensor1 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                    else:
                        self.prevsensor1 = self.sensor1

            elif self.startposition == 3:  # sensor2 is in neutral

                # self.get_sensorValue(1)
                self.get_sensorValue(2)

                # analog value is not in the neutral range
                if not (self.sensor2 < self.Neutral[1] and self.sensor2 > self.Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor2 != self.Mid:
                        # check for actual rotation
                        if self.sensor2 > (self.prevsensor2 + self.setresolution) or self.sensor2 < (self.prevsensor2 - self.setresolution):
                            distance = self.prevsensor2 - self.sensor2
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor2 = self.sensor2 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor2 = self.sensor2 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                    else:
                        self.prevsensor2 = self.sensor2

            elif self.startposition == 4:  # sensor1 is in neutral

                self.get_sensorValue(1)
                # self.get_sensorValue(2)

                # analog value is not in the neutral range
                if not (self.sensor1 < self.Neutral[1] and self.sensor1 > self.Neutral[0]):
                    # check for actual rotation
                    if self.prevsensor1 != self.Mid:
                        # check for actual rotation
                        if self.sensor1 > (self.prevsensor1 + self.setresolution) or self.sensor1 < (self.prevsensor1 - self.setresolution):
                            # get distance from north and south pole magnets
                            distance = self.prevsensor1 - self.sensor1
                            # check rotation direction with distance values
                            if distance > 0:  # moved towards magnet northpole
                                self.prevsensor1 = self.sensor1 + \
                                    self.setresolution - \
                                    (self.setresolution - 1)
                                self.rotation_action(1)
                                self.haptics(1)

                            elif distance < 0:  # moved towards magnet southpole
                                self.prevsensor1 = self.sensor1 - \
                                    self.setresolution + \
                                    (self.setresolution - 1)
                                self.rotation_action(0)
                                self.haptics(1)

                    else:
                        self.prevsensor1 = self.sensor1

            if self.invertcount:

                self.count *= -1

            self.tempcount = self.count
            self.recaliberate_startPosition()
            return self.tempcount
    def DebugEncoder(self):
        print("prevstartposition >> {} startposition >> {} abs_s1 >> {} abs_s2 >> {}".format(self.prevstartposition, self.startposition, self.S1_absoluteneutral, self.S2_absoluteneutral))
        print("\n")
        print("stepres >> {} setresolution >> {} bound >> {}".format(self.stepres, self.setresolution, self.bound))
        print("\n")
        print("S1---- {} S1_S---- {} S1_N---- {} S2---- {} S2_S---- {} S2_N---- {}".format (self.get_sensorValue(1), self.get_sensorINTpin(3).value(), self.get_sensorINTpin(1).value(), self.get_sensorValue(2), self.get_sensorINTpin(2).value(), self.get_sensorINTpin(4).value()))
        print("\n")






       
      
