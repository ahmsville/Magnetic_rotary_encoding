#ifndef MagRotaryEncoder_h
#define MagRotaryEncoder_h

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/

#include "Arduino.h"

#include "ADCRead.h"

class MagRotaryEncoder {
private:
	int state, startposition = 0, prevstartposition,stepres = 100, sensor1, sensor2, sensor1_pin, sensor2_pin, prevsensor1, prevsensor2, setresolution = 1;
	int Neutral[2], S1_Northpole[2], S1_Southpole[2], S1_North, S1_South, S2_Northpole[2], S2_Southpole[2], S2_North, S2_South, Mid;
	int count, tempcount, bound = 10, addedbound = 0, stablerange = 10, tempanavaldiff, multiplier = 10, timetomultiply = 10;
	int southRegion, northRegion;
	long rotationrate = 0, timetracker = 0;
	int haptics_pin = 0, haptics_duration = 0, haptics_strength = 0, haptics_ontime, haptics_offtime, haptics_state;
	bool use_extended_resolution = false;
	int countt = 0, distance = 0;
	int SensorINTpin[2];

	int tempstartposition = 0, tempsensor1, tempsensor2, tempprevsensor1, tempprevsensor2;
	int encoderResolution = 0, encoderPosition = 0;
	int storedstartposition;
	int storedsensor1state, storedsensor2state;
	bool inSync = false;

	bool useInterrupt = false;
	bool interruptFired = false;

	bool INT1fired = false;
	bool INT2fired = false;

	bool INTProcessed = true;

	//typedef void (MagRotaryEncoder::* ISRfunctions)(); // new data type 
	//ISRfunctions SensorISRfunctions[2];

	ADCRead adcVal;

public:

	MagRotaryEncoder(int s1, int s2);
	MagRotaryEncoder(int s1, int s2, int s1INTpin, int s2INTpin);
	void set_poleStateValues(int sensornum, int np, int nu, int sp);  //set ADC values for the poles (northpole, neutral, southpole)
	void initialize_encoder();  //initialize encoder
	void recaliberate_startPosition();  //sets the start position based on the ADC values
	int get_sensorValue(int sensornum);  //
	int get_currentSensorValue(int sensornum); 
	int detect_rotation();   // rotation encoding function 
	int detect_rotationHR();   // rotation encoding function 
	int detect_rotationWithRate();
	void rotation_action(int act);
	void set_bound(int b);
	void set_resolution(int percent);
	void set_haptics(int pin, int duration, int strength);   //use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
	void haptics(int state);

	void setsleep(bool slpact);
	bool readsleep();

	int sensor1_INT();
	int sensor2_INT();
	void set_encoderResolution(int res);
	int get_encResCount(int retstep);
	int setToStart();
	
	int get_sensorINTpin(int sensornum);  //
	


};



#endif // !MagRotaryEncoder_h
