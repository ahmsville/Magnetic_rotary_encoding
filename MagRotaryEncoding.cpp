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

void MagRotaryEncoder::set_poleStateValues(int sensornum, int np, int nu, int sp) {  //set ADC values for the poles (northpole, neutral, southpole)
	if (sensornum == 1) {
		S1_Northpole[0] = np - bound;
		S1_Northpole[1] = np + bound;

		S1_Southpole[0] = sp - bound;
		S1_Southpole[1] = sp + bound;

		S1_North = np;
		S1_South = sp;
	}
	if (sensornum == 2) {
		S2_Northpole[0] = np - bound;
		S2_Northpole[1] = np + bound;

		S2_Southpole[0] = sp - bound;
		S2_Southpole[1] = sp + bound;

		S2_North = np;
		S2_South = sp;
	}
	
	southRegion = Neutral[1] + stablerange;
	northRegion = Neutral[0] - stablerange;
	Neutral[0] = nu - bound;
	Neutral[1] = nu + bound;
	Mid = nu;
}

void MagRotaryEncoder::initialize_encoder() {   //initialize encoder
	set_poleStateValues(1, 512 - bound - stepres, 512, 512 + bound + stepres); // set the peak ADC values for the (sensor1, northpole, neutralstate, southpole)
	set_poleStateValues(2, 512 - bound - stepres, 512, 512 + bound + stepres); // set the peak ADC values for the (sensor2, northpole, neutralstate, southpole)
	recaliberate_startPosition();
	prevsensor1 = sensor1;
	prevsensor2 = sensor2;
}



void MagRotaryEncoder::recaliberate_startPosition() {  //sets the start position based on the ADC values
	haptics(0);
		
		if (sensor1 < Neutral[0] && sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {  //sensor1 = north , sensor2 = neutral
			startposition = 1;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor2 = Mid;
				prevstartposition = startposition;
			}			
		}
		else if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
			startposition = 2;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor1 = Mid;
				prevstartposition = startposition;
			}
		}
		else if (sensor1 > Neutral[1] && sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) { //sensor1 = south , sensor2 = neutral
			startposition = 3;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor2 = Mid;
				prevstartposition = startposition;
			}
		}
		else if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1] && sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
			startposition = 4;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor1 = Mid;
				prevstartposition = startposition;
			}
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
	
	
	if (startposition == 1) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor2 < northRegion) {
				rotation_action(0);
				haptics(1);
				startposition = 0;//4;
			}
			else if (sensor2 > southRegion) {
				rotation_action(1);
				haptics(1);
				startposition = 0;//2;
			}
		}
		
	}
	
	else if (startposition == 2) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor1 < northRegion) {
				rotation_action(0);
					haptics(1);
					startposition = 0;//1;
			}
			else if (sensor1 > southRegion) {
				rotation_action(1);
					haptics(1);
					startposition = 0;//3;
			}
		}
	}
	
	else if (startposition == 3) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor2 > southRegion) {
				rotation_action(0);
					haptics(1);
					startposition = 0;//2;
			}
			else if (sensor2 < northRegion) {
				rotation_action(1);
					haptics(1);
					startposition = 0;//4;
			}
		}
	}

	else if (startposition == 4) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor1 > southRegion) {
				rotation_action(0);
					haptics(1);
					startposition = 0;//3;
			}
			else if (sensor1 < northRegion) {
				rotation_action(1);
					haptics(1);
					startposition = 0;//1;
			}
		}
	}
	
	else {
		get_sensorValue(1);
		get_sensorValue(2);
	}

	tempcount = count;
	recaliberate_startPosition();
	return tempcount;
}

/*
int MagRotaryEncoder::detect_rotation() {  // openloop rotation encoding function


	if (startposition == 1) {
		get_sensorValue(1);
		get_sensorValue(2);
		tempanavaldiff = sensor2 - prevsensor2;
		if (tempanavaldiff < 0) { //remove -ve sign
			tempanavaldiff = -1 * tempanavaldiff;
		}
		if (tempanavaldiff > stablerange) { //check for significant change in analog value
			if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {
				if (sensor2 >= S2_Northpole[0] && sensor2 <= S2_Northpole[1]) {
					rotation_action(0);
					haptics(1);
					startposition = 4;
				}
				else if (sensor2 >= S2_Southpole[0] && sensor2 <= S2_Southpole[1]) {
					rotation_action(1);
					haptics(1);
					startposition = 2;
				}
				prevsensor2 = sensor2;
			}
		}

	}

	else if (startposition == 2) {
		get_sensorValue(1);
		get_sensorValue(2);
		tempanavaldiff = sensor1 - prevsensor1;
		if (tempanavaldiff < 0) { //remove -ve sign
			tempanavaldiff = -1 * tempanavaldiff;
		}
		if (tempanavaldiff > stablerange) { //check for significant change in analog value
			if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {
				if (sensor1 >= S1_Northpole[0] && sensor1 <= S1_Northpole[1]) {
					rotation_action(0);
					haptics(1);
					startposition = 1;
				}
				else if (sensor1 >= S1_Southpole[0] && sensor1 <= S1_Southpole[1]) {
					rotation_action(1);
					haptics(1);
					startposition = 3;
				}
				prevsensor1 = sensor1;
			}
		}
	}

	else if (startposition == 3) {
		get_sensorValue(1);
		get_sensorValue(2);
		tempanavaldiff = sensor2 - prevsensor2;
		if (tempanavaldiff < 0) { //remove -ve sign
			tempanavaldiff = -1 * tempanavaldiff;
		}
		if (tempanavaldiff > stablerange) { //check for significant change in analog value
			if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {
				if (sensor2 >= S2_Southpole[0] && sensor2 <= S2_Southpole[1]) {
					rotation_action(0);
					haptics(1);
					startposition = 2;
				}
				else if (sensor2 >= S2_Northpole[0] && sensor2 <= S2_Northpole[1]) {
					rotation_action(1);
					haptics(1);
					startposition = 4;
				}
				prevsensor2 = sensor2;
			}
		}
	}

	else if (startposition == 4) {
		get_sensorValue(1);
		get_sensorValue(2);
		tempanavaldiff = sensor1 - prevsensor1;
		if (tempanavaldiff < 0) { //remove -ve sign
			tempanavaldiff = -1 * tempanavaldiff;
		}
		if (tempanavaldiff > stablerange) { //check for significant change in analog value
			if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {
				if (sensor1 >= S1_Southpole[0] && sensor1 <= S1_Southpole[1]) {
					rotation_action(0);
					haptics(1);
					startposition = 3;
				}
				else if (sensor1 >= S1_Northpole[0] && sensor1 <= S1_Northpole[1]) {
					rotation_action(1);
					haptics(1);
					startposition = 1;
				}
				prevsensor1 = sensor1;
			}
		}
	}


	tempcount = count;
	if (tempcount != 0) {
		Serial.print(sensor1);
		Serial.print("\t");
		Serial.print(sensor2);
		Serial.print("\t");
		Serial.print(startposition);
		Serial.print("\t");
		Serial.println(tempcount);
	}

	recaliberate_startPosition();
	return tempcount;
}
*/

int MagRotaryEncoder::detect_rotationHR() {  // openloop rotation encoding function 
	int sensvalrange = 0;
	
	
	if (startposition == 1) {   //sensor2 is in neutral
		get_sensorValue(1);
		get_sensorValue(2);
		if (!(sensor2 < Neutral[1] && sensor2 > Neutral[0])) { //analog value is not in the neutral range
			// check for actual rotation
			if (prevsensor2 != Mid) {
				if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution)) {
					//get distance from north and south pole magnets
					int distance = prevsensor2 - sensor2;
					//check rotation direction with distance values
					if (distance > 0) {  //moved towards magnet northpole
						prevsensor2 = sensor2 + setresolution - (setresolution - 1);
						rotation_action(0);
						haptics(1);
					}
					else if (distance < 0) {  //moved towards magnet southpole
						prevsensor2 = sensor2 - setresolution + (setresolution - 1);
						rotation_action(1);
						haptics(1);
					}
				}
			}
			else {
				prevsensor2 = sensor2;
			}
		}

	}
	
	 else if (startposition == 2) {  //sensor1 is in neutral
		get_sensorValue(1);
		get_sensorValue(2);
		if (!(sensor1 < Neutral[1] && sensor1 > Neutral[0])) { //analog value is not in the neutral range
			// check for actual rotation
			if (prevsensor1 != Mid) {
				// check for actual rotation
				if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution)) {
					//get distance from north and south pole magnets
					int distance = prevsensor1 - sensor1;
					//check rotation direction with distance values
					if (distance > 0) {  //moved towards magnet northpole
						prevsensor1 = sensor1 + setresolution - (setresolution - 1);			
						rotation_action(0);
						haptics(1);
					}
					else if (distance < 0) {  //moved towards magnet southpole
						prevsensor1 = sensor1 - setresolution + (setresolution - 1);			
						rotation_action(1);
						haptics(1);
					}
				}
			}
			else {
				prevsensor1 = sensor1;
			}
		}
	}
	
	else if (startposition == 3) {  //sensor2 is in neutral
		get_sensorValue(1);
		get_sensorValue(2);
		if (!(sensor2 < Neutral[1] && sensor2 > Neutral[0])) { //analog value is not in the neutral range
			// check for actual rotation
			if (prevsensor2 != Mid) {
				// check for actual rotation
				if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution)) {
					int distance = prevsensor2 - sensor2;
					//check rotation direction with distance values
					if (distance > 0) {  //moved towards magnet northpole
						prevsensor2 = sensor2 + setresolution - (setresolution - 1);
						rotation_action(1);
						haptics(1);
					}
					else if (distance < 0) {  //moved towards magnet southpole
						prevsensor2 = sensor2 - setresolution + (setresolution - 1);
						rotation_action(0);
						haptics(1);
					}
				}
			}
			else {
				prevsensor2 = sensor2;
			}
		}
	}
	
	else if (startposition == 4) {  //sensor1 is in neutral
		get_sensorValue(1);
		get_sensorValue(2);
		if (!(sensor1 < Neutral[1] && sensor1 > Neutral[0])) { //analog value is not in the neutral range
			// check for actual rotation
			if (prevsensor1 != Mid) {
				// check for actual rotation
				if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution)) {
					//get distance from north and south pole magnets
					int distance = prevsensor1 - sensor1;
					//check rotation direction with distance values
					if (distance > 0) {  //moved towards magnet northpole
						prevsensor1 = sensor1 + setresolution - (setresolution - 1);;
						rotation_action(1);
						haptics(1);
					}
					else if (distance < 0) {  //moved towards magnet southpole
						prevsensor1 = sensor1 - setresolution + (setresolution - 1);;
						rotation_action(0);
						haptics(1);
					}
				}
			}
			else {
				prevsensor1 = sensor1;
			}
		}
	}
	
	
	
	else {
	get_sensorValue(1);
	get_sensorValue(2);
	}
	tempcount = count;
	/*
	countt += tempcount;
	if (tempcount != 0) {
		Serial.print(setresolution);
		Serial.print("\t");
		Serial.print(sensor1);
		Serial.print("\t");
		Serial.print(prevsensor1);
		Serial.print("\t");
		Serial.print(sensor2);
		Serial.print("\t");
		Serial.print(prevsensor2);
		Serial.print("\t");
		Serial.print(startposition);
		Serial.print("\t");
		Serial.print(tempcount);
		Serial.print("\t");
		Serial.println(countt);
		
	}
	*/
	recaliberate_startPosition();
	return tempcount;



}

void MagRotaryEncoder::set_resolution(int percent) {
	if (percent == 100) {
		setresolution = 1;
	}
	else {
		setresolution = stepres - ((percent * 0.01) * stepres);
	}
			
}

int MagRotaryEncoder::detect_rotationWithRate() {  // openloop rotation encoding function 

	if (startposition == 1) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor2 < northRegion) {
				rotation_action(0);
				haptics(1);
				startposition = 0;//4;
			}
			else if (sensor2 > southRegion) {
				rotation_action(1);
				haptics(1);
				startposition = 0;//2;
			}
		}

	}

	else if (startposition == 2) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor1 < northRegion) {
				rotation_action(0);
				haptics(1);
				startposition = 0;//1;
			}
			else if (sensor1 > southRegion) {
				rotation_action(1);
				haptics(1);
				startposition = 0;//3;
			}
		}
	}

	else if (startposition == 3) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor1 >= Neutral[0] && sensor1 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor2 > southRegion) {
				rotation_action(0);
				haptics(1);
				startposition = 0;//2;
			}
			else if (sensor2 < northRegion) {
				rotation_action(1);
				haptics(1);
				startposition = 0;//4;
			}
		}
	}

	else if (startposition == 4) {
		get_sensorValue(1);
		get_sensorValue(2);
		if (sensor2 >= Neutral[0] && sensor2 <= Neutral[1]) {//if sec sensor goes neutral
			if (sensor1 > southRegion) {
				rotation_action(0);
				haptics(1);
				startposition = 0;//3;
			}
			else if (sensor1 < northRegion) {
				rotation_action(1);
				haptics(1);
				startposition = 0;//1;
			}
		}
	}

	else {
		get_sensorValue(1);
		get_sensorValue(2);
	} 

	//calculate rate of rotation
	if (count != 0) {
		rotationrate = millis() - timetracker;  //rotation step rate
		if (rotationrate > 0) {  //avoid negative values
			

			if (rotationrate >= timetomultiply) { //not fast anough for multiplier
			timetracker = millis();
				tempcount = count;
		recaliberate_startPosition();
		return tempcount;
			}
			else {
				float calcmul = 1 - ((float)rotationrate / (float)timetomultiply); 
				tempcount = count * multiplier * calcmul;
				recaliberate_startPosition();
				timetracker = millis();
				return tempcount;
			}
			
		}
		else {
			rotationrate = 0;
			timetracker = millis();
		}	
		
	}
	else {
		tempcount = count;
		recaliberate_startPosition();
		return tempcount;
	}
	tempcount = count;
	recaliberate_startPosition();
	return tempcount;
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

void MagRotaryEncoder::haptics(int state) {   //viberation feedback function
	if (state == 1) {
		analogWrite(haptics_pin, haptics_strength);
		haptics_ontime = millis();
		haptics_state = 1;
	}
	else {
		haptics_offtime = millis();
		if (((haptics_offtime - haptics_ontime) >= haptics_duration) && haptics_state == 1) {
			analogWrite(haptics_pin, 0);
			haptics_state = 0;
		}

	}
	/*
	analogWrite(haptics_pin, haptics_strength);
	delay(haptics_duration);
	analogWrite(haptics_pin, 0);
	*/
}