#ifndef MagRotaryEncoder_h
#define MagRotaryEncoder_h

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include <iostream>

class MagRotaryEncoder
{
private:
	int state, startposition = 0, prevstartposition, stepres = 100, setHresolutionpercent = 100, sensor1_pin, sensor2_pin, sensor1_adc_channel, sensor2_adc_channel, prevsensor1, prevsensor2, setresolution = 1;
	int sensor1, sensor2;
	int Neutral[2], S1_North, S1_South, S2_North, S2_South, Mid;
	int S1_Neutral[2], S2_Neutral[2], S1_absoluteneutral, S2_absoluteneutral, maxsway = 50;
	bool locksensorlevels = false;
	int count, tempcount, bound = 75, ADCMaxRes = 4096, addedbound = 0, stablerange = 10, tempanavaldiff, multiplier = 10, timetomultiply = 10;
	int southRegion, northRegion;
	long rotationrate = 0, timetracker = 0;
	int haptics_pin = 0, haptics_duration = 0, haptics_strength = 0, haptics_ontime, haptics_offtime, haptics_state;
	bool use_extended_resolution = false;
	int countt = 0, distance = 0;
	int SensorINTpin[4] = { 0, 0, 0, 0 };

	int tempstartposition = 0, tempsensor1, tempsensor2, tempprevsensor1, tempprevsensor2;
	int encoderResolution = 0, encoderPosition = 0;
	int storedstartposition;
	int storedsensor1state, storedsensor2state;
	bool inSync = false;
	bool ReturnSteps = true;

	float alpha = 0.01;
	int prevsmoothsensor1 = 0, prevsmoothsensor2 = 0;

	bool useInterrupt = false;
	bool interruptFired = false;
	bool exclInterrupt = false;

	bool partialexclInterrupt = true;
	bool INT1fired = false;
	bool INT2fired = false;

	bool INTProcessed = false;
	bool useautosetpoles = false;

	int activesensorINT = 1;
	int absoluteNeutral = 2048;

	bool invertcount = false;
	bool processS1_S_INT = true, processS1_N_INT = true, processS2_S_INT = true, processS2_N_INT = true;
	int flagState = 0;
	bool returnRateCount = false;
	

public:
	MagRotaryEncoder(int s1, int ch1, int s2, int ch2);
	MagRotaryEncoder(int s1, int ch1, int s2, int ch2, int s1INTpin, int s2INTpin);

	void setExclINTmode(int s1, int ch1, int s2, int ch2, bool exclint);
	void set_poleStateValues(int sensornum, int np, int nu, int sp); // set ADC values for the poles (northpole, neutral, southpole)
	void initialize_encoder();										 // initialize encoder
	void recaliberate_startPosition();								 // sets the start position based on the ADC values
	int get_sensorValue(int sensornum);								 //
	int get_sensorValue(int sensornum, bool ret);					 //
	int get_currentSensorValue(int sensornum);
	int detect_rotation();	 // rotation encoding function
	int detect_rotationHR(); // rotation encoding function
	int detect_rotationWithRate(bool act);
	void setrotationRate();
	void applyrateMultiplier();
	void rotation_action(int act);
	void set_bound(int b);
	void set_resolution(float percent);
	void set_haptics(int pin, int duration, int strength); // use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
	void haptics(int state);

	void setsleep(bool slpact);
	bool readsleep();
	bool passSynctest(int newspos);
	int singleSensor_INT(int incenter);
	int sensor1_INT();
	int sensor2_INT();
	int getActiveSensorInterrupt();
	void set_encoderResolution(int res);
	int get_encResCount(int retstep);
	int setToStart();
	int setToStart(int startfrom);
	void useinterruptdetection(bool act);

	void setResistorDivider(float R1, float R2, float vcc);

	int get_encodingState();
	int get_encodingState(int incenter);

	void invertCount(bool inv);

	int get_sensorINTpin(int sensornum); //
	void set_adcresolution(int res);
	int CalibrateSensors(int snum);
	bool LoadCalibrationData(int s1, int s2);
	void DebugEncoder();
	void calibrateStepRes_HR();
	int get_bound(int sensor_num);
	int processExINT(int encstate);
	int processCOUNT(int prevpos, int newpos);
	int processPULSE(int currentpos);
};

#endif // !MagRotaryEncoder_h