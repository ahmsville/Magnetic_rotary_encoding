#ifndef MagRotaryEncoder_h
#define MagRotaryEncoder_h

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/

#include "Arduino.h"



class MagRotaryEncoder {
private:
	int state, startposition, sensor1, sensor2, sensor1_pin, sensor2_pin;
	int Neutral[2], Northpole[2], Southpole[2];
	int count, bound = 200;
	int haptics_pin = 0, haptics_duration = 0, haptics_strength = 0, haptics_ontime, haptics_offtime, haptics_state;

public:
	MagRotaryEncoder(int s1, int s2);
	void set_poleStateValues(int np, int nu, int sp);  //set ADC values for the poles (northpole, neutral, southpole)
	void initialize_encoder();  //initialize encoder
	void recaliberate_startPosition();  //sets the start position based on the ADC values
	int get_sensorValue(int sensornum);  //
	int get_currentSensorValue(int sensornum); 
	int detect_rotation();   // openloop rotation encoding function 
	int detect_rotationContinuously();  // // closedlood rotation encoding function
	void rotation_action(int act);
	void set_bound(int b);
	void set_haptics(int pin, int duration, int strength);   //use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
	void haptics(int state);
};


#endif // !MagRotaryEncoder_h