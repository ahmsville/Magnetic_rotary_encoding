#include "MagRotaryEncoding.h"

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/


MagRotaryEncoder::MagRotaryEncoder(int s1, int s2) {  //set sensor pins
	sensor1_pin = s1;
	sensor2_pin = s2;
}

void MagRotaryEncoder::set_poleStateValues(int np, int nu, int sp) {  //set ADC values for the poles (northpole, neutral, southpole)
	Northpole[0] = np - bound;
	Northpole[1] = np + bound;
	Neutral[0] = nu - 100;
	Neutral[1] = nu + 100;
	Southpole[0] = sp - bound;
	Southpole[1] = sp + bound;
}

void MagRotaryEncoder::initialize_encoder() {   //initialize encoder
	recaliberate_startPosition();
}

void MagRotaryEncoder::recaliberate_startPosition() {  //sets the start position based on the ADC values
	get_sensorValue(1);
	get_sensorValue(2);
	if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Southpole[0] && sensor2 <= Southpole[1]) {
		startposition = 1;
	}
	else if (sensor1 >= Northpole[0] && sensor1 <= Northpole[1] && sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {
		startposition = 2;
	}
	else if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Northpole[0] && sensor2 <= Northpole[1]) {
		startposition = 3;
	}
	else if (sensor1 >= Southpole[0] && sensor1 <= Southpole[1] && sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {
		startposition = 4;
	}
	count = 0;
}

int MagRotaryEncoder::get_sensorValue(int sensornum) {
	if (sensornum == 1) {
		sensor1 = analogRead(sensor1_pin);
		return sensor1;
	}
	else if (sensornum == 2) {
		sensor2 = analogRead(sensor2_pin);
		return sensor2;
	}
}

int MagRotaryEncoder::get_currentSensorValue(int sensornum) {
	if (sensornum == 1) {
		return sensor1;
	}
	else if (sensornum == 2) {
		return sensor2;
	}
}

int MagRotaryEncoder::detect_rotation() {  // openloop rotation encoding function 
	//recaliberate_startPosition();
	if (startposition == 1) {
		get_sensorValue(1);
		if (sensor1 >= Northpole[0] && sensor1 <= Northpole[1]) {
			rotation_action(1);
			haptics();
			startposition = 2;
		}
		else if (sensor1 >= Southpole[0] && sensor1 <= Southpole[1]) {
			rotation_action(0);
			haptics();
			startposition = 4;
		}
	}

	else if (startposition == 2) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Northpole[0] && sensor2 <= Northpole[1]) {
			rotation_action(1);
			haptics();
			startposition = 3;
		}
		else if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Southpole[0] && sensor2 <= Southpole[1]) {
			rotation_action(0);
			haptics();
			startposition = 1;
		}
	}

	else if (startposition == 3) {
		get_sensorValue(1);
		if (sensor1 >= Southpole[0] && sensor1 <= Southpole[1]) {
			rotation_action(1);
			haptics();
			startposition = 4;
		}
		else if (sensor1 >= Northpole[0] && sensor1 <= Northpole[1]) {
			rotation_action(0);
			haptics();
			startposition = 2;
		}
	}

	else if (startposition == 4) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Southpole[0] && sensor2 <= Southpole[1]) {
			rotation_action(1);
			haptics();
			startposition = 1;
		}
		else if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 >= Northpole[0] && sensor2 <= Northpole[1]) {
			rotation_action(0);
			haptics();
			startposition = 3;
		}
	}
	else {
	
	}
	return count;
}

int MagRotaryEncoder::detect_rotationContinuously() {   // closedlood rotation encoding function 
	
}

void MagRotaryEncoder::rotation_action(int act) {  //sets action for clockwise and anticlockwise rotations
	if (act == 1) {
		count++;
	}
	else if (act == 0) {
		count--;
	}
}

void MagRotaryEncoder::set_bound(int b) {   //this value determines the upper and lower limit of the ADC values
	bound = b;
}

void MagRotaryEncoder::set_haptics(int pin, int duration, int strength) {  //use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
	haptics_pin = pin;
	haptics_duration = duration;
	haptics_strength = strength;
}

void MagRotaryEncoder::haptics() {   //viberation feedback function
	analogWrite(haptics_pin, haptics_strength);
	delay(haptics_duration);
	analogWrite(haptics_pin, 0);
}