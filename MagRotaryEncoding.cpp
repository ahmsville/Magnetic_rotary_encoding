#include "MagRotaryEncoding.h"

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/


volatile bool sleepmode = false;



void MagRotaryEncoder::sensor1_INT() {
	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(2); //read sensor 2
		//set new startposition
		if (sensor2 != 0) { //value read successfully, process here
			INTProcessed = true;
			if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
				startposition = 2;
				prevsensor1 = Mid;

			}

			else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
				startposition = 4;
				prevsensor1 = Mid;
			}
			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
				if ((startposition - prevstartposition) > 0) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 1) { // new startposition == 1
				if (prevstartposition == 4) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 4) { // new startposition == 4
				if (prevstartposition == 3) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			timetracker = millis();

		}
		else { //process in loop
			INTProcessed = false;
			INT1fired = true;
			INT2fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
		}
	}
	else {
		INT1fired = true;
		INT2fired = false;
	}
}

void MagRotaryEncoder::sensor2_INT() {

	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(1); //read sensor 1
		//set new startposition
		if (sensor1 != 0) { //value read successfully, process here
			INTProcessed = true;
			if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
				startposition = 3;
				prevsensor2 = Mid;

			}

			else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
				startposition = 1;
				prevsensor2 = Mid;
			}
			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
				if ((startposition - prevstartposition) > 0) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 1) { // new startposition == 1
				if (prevstartposition == 4) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 4) { // new startposition == 4
				if (prevstartposition == 3) { //clockwise
					count++;
				}
				else { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			timetracker = millis();
		}
		else { //process in loop
			INTProcessed = false;
			INT2fired = true;
			INT1fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
		}
	}
	else {
		INT2fired = true;
		INT1fired = false;
	}
}






MagRotaryEncoder::MagRotaryEncoder(int s1, int s2) {  //set sensor pins
	sensor1_pin = s1;
	sensor2_pin = s2;
	useInterrupt = false;

}
MagRotaryEncoder::MagRotaryEncoder(int s1, int s2, int s1INTpin, int s2INTpin) {  //set sensor pins
	sensor1_pin = s1;
	sensor2_pin = s2;
	SensorINTpin[0] = s1INTpin;
	SensorINTpin[1] = s2INTpin;
	useInterrupt = true;

}

int MagRotaryEncoder::get_sensorINTpin(int sensornum) {
	if (sensornum == 1) {
		return SensorINTpin[0];
	}
	else if (sensornum == 2) {
		return SensorINTpin[1];
	}
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
	if (!useInterrupt) { //interrupt detection is not used
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


	}
	else {
		if (use_extended_resolution) { //extended res and interrupt is in use, update sensor values from temp values
			if (INT1fired) {
				get_sensorValue(2); //read sensor 2
				//set new startposition
				if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevsensor1 = Mid;
				}
				else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevsensor1 = Mid;
				}
				INT1fired = false;
			}
			else if (INT2fired) {
				get_sensorValue(1); //read sensor 1
				//set new startposition
				if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevsensor2 = Mid;
				}
				else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevsensor2 = Mid;
				}
				INT2fired = false;
			}

		}
	}

	count = 0;
}





int MagRotaryEncoder::get_sensorValue(int sensornum) {
	if (sensornum == 1) {
		sensor1 = adcVal.Read(sensor1_pin);
		return sensor1;
	}
	else if (sensornum == 2) {
		sensor2 = adcVal.Read(sensor2_pin);
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
	if (!useInterrupt) { //interrupt detection is not used

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
	else {
		if (INTProcessed) { //processed in interrupt
			if (count != 0) {
				//SerialUSB.print(prevstartposition);
				//SerialUSB.print("\t");
				//SerialUSB.print(count);
				//SerialUSB.print("\t");
				haptics(1);
				tempcount = count;
				recaliberate_startPosition();
				return tempcount;
			}
			else {
				recaliberate_startPosition();
				return count;
			}
		}
		else {
			if (INT1fired) {
				get_sensorValue(2); //read sensor 2
				//set new startposition
				if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevsensor1 = Mid;

				}
				else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevsensor1 = Mid;
				}
				//keep count
				if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
					if ((startposition - prevstartposition) > 0) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 1) { // new startposition == 1
					if (prevstartposition == 4) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 4) { // new startposition == 4  
					if (prevstartposition == 3) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				INT1fired = false;
			}
			else if (INT2fired) {
				get_sensorValue(1); //read sensor 1
				//set new startposition
				if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevsensor2 = Mid;

				}
				else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevsensor2 = Mid;
				}
				//keep count
				if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
					if ((startposition - prevstartposition) > 0) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 1) { // new startposition == 1
					if (prevstartposition == 4) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 4) { // new startposition == 4  
					if (prevstartposition == 3) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				INT2fired = false;
			}
			if (count != 0) {
				haptics(1);
				tempcount = count;
				recaliberate_startPosition();
				return tempcount;
			}
			else {
				recaliberate_startPosition();
				return count;
			}
		}
	}
}

int MagRotaryEncoder::detect_rotationWithRate() {  // openloop rotation encoding function
	if (!useInterrupt) { //interrupt detection is not used
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
	else {
		if (INTProcessed) {
			//calculate rate of rotation
			if (count != 0) {

				if (rotationrate > 0) {  //avoid negative values


					if (rotationrate >= timetomultiply) { //not fast anough for multiplier

						tempcount = count;
						recaliberate_startPosition();
						return tempcount;
					}
					else {
						float calcmul = 1 - ((float)rotationrate / (float)timetomultiply);
						tempcount = count * multiplier * calcmul;
						recaliberate_startPosition();

						return tempcount;
					}

				}
				else {
					rotationrate = 0;

				}

			}
			else {
				tempcount = count;
				recaliberate_startPosition();
				return tempcount;
			}
		}
		else {
			if (INT1fired) {
				get_sensorValue(2); //read sensor 2
				//set new startposition
				if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevsensor1 = Mid;

				}
				else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevsensor1 = Mid;
				}
				//keep count
				if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
					if ((startposition - prevstartposition) > 0) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 1) { // new startposition == 1
					if (prevstartposition == 4) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 4) { // new startposition == 4  
					if (prevstartposition == 3) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				INT1fired = false;
			}
			else if (INT2fired) {
				get_sensorValue(1); //read sensor 1
				//set new startposition
				if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevsensor2 = Mid;

				}
				else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevsensor2 = Mid;
				}
				//keep count
				if (startposition == 2 || startposition == 3) { //new startposition is not == 1 or 4
					if ((startposition - prevstartposition) > 0) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 1) { // new startposition == 1
					if (prevstartposition == 4) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				else if (startposition == 4) { // new startposition == 4  
					if (prevstartposition == 3) { //clockwise
						count++;
					}
					else { //counterclockwise
						count--;
					}
					prevstartposition = startposition;
				}
				INT2fired = false;
			}
			//calculate rate of rotation
			if (count != 0) {

				if (rotationrate > 0) {  //avoid negative values


					if (rotationrate >= timetomultiply) { //not fast anough for multiplier

						tempcount = count;
						recaliberate_startPosition();
						return tempcount;
					}
					else {
						float calcmul = 1 - ((float)rotationrate / (float)timetomultiply);
						tempcount = count * multiplier * calcmul;
						recaliberate_startPosition();

						return tempcount;
					}

				}
				else {
					rotationrate = 0;

				}

			}
			else {
				tempcount = count;
				recaliberate_startPosition();
				return tempcount;
			}
		}

	}
}

int MagRotaryEncoder::detect_rotationHR() {  // openloop rotation encoding function

	int sensvalrange = 0;

	if (!useInterrupt) { //interrupt detection is not used
		if (startposition == 1) {   //sensor2 is in neutral
			get_sensorValue(1);
			get_sensorValue(2);
			if (!(sensor2 < Neutral[1] && sensor2 > Neutral[0])) { //analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor2 != Mid) {
					if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution)) {
						//get distance from north and south pole magnets
						distance = prevsensor2 - sensor2;
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
						distance = prevsensor1 - sensor1;
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
						distance = prevsensor2 - sensor2;
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
						distance = prevsensor1 - sensor1;
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
			SerialUSB.print(setresolution);
			SerialUSB.print("\t");
			SerialUSB.print(sensor1);
			SerialUSB.print("\t");
			SerialUSB.print(prevsensor1);
			SerialUSB.print("\t");
			SerialUSB.print(sensor2);
			SerialUSB.print("\t");
			SerialUSB.print(prevsensor2);
			SerialUSB.print("\t");
			SerialUSB.print(startposition);
			SerialUSB.print("\t");
			SerialUSB.print(tempcount);
			SerialUSB.print("\t");
			SerialUSB.println(countt);

		}
		*/

		recaliberate_startPosition();
		return tempcount;
	}
	else {  //interrupt assisted

		if (startposition == 1) {   //sensor2 is in neutral


			//get_sensorValue(1);
			get_sensorValue(2);

			if (!(sensor2 < Neutral[1] && sensor2 > Neutral[0])) { //analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor2 != Mid) {
					if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution)) {
						//get distance from north and south pole magnets
						distance = prevsensor2 - sensor2;
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
			//get_sensorValue(2);


			if (!(sensor1 < Neutral[1] && sensor1 > Neutral[0])) { //analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor1 != Mid) {
					// check for actual rotation
					if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution)) {
						//get distance from north and south pole magnets
						distance = prevsensor1 - sensor1;
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


			//get_sensorValue(1);
			get_sensorValue(2);

			if (!(sensor2 < Neutral[1] && sensor2 > Neutral[0])) { //analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor2 != Mid) {
					// check for actual rotation
					if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution)) {
						distance = prevsensor2 - sensor2;
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
			//get_sensorValue(2);


			if (!(sensor1 < Neutral[1] && sensor1 > Neutral[0])) { //analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor1 != Mid) {
					// check for actual rotation
					if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution)) {
						//get distance from north and south pole magnets
						distance = prevsensor1 - sensor1;
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

		tempcount = count;
		/*
				countt += tempcount;
				if (tempcount != 0) {
					SerialUSB.print(setresolution);
					SerialUSB.print("\t");
					SerialUSB.print(sensor1);
					SerialUSB.print("\t");
					SerialUSB.print(prevsensor1);
					SerialUSB.print("\t");
					SerialUSB.print(sensor2);
					SerialUSB.print("\t");
					SerialUSB.print(prevsensor2);
					SerialUSB.print("\t");
					SerialUSB.print(startposition);
					SerialUSB.print("\t");
					SerialUSB.print(tempcount);
					SerialUSB.print("\t");
					SerialUSB.print(countt);
					SerialUSB.print("\t");
					SerialUSB.println(Mid);
				}
				*/
		recaliberate_startPosition();
		return tempcount;
	}


}






void MagRotaryEncoder::set_resolution(int percent) {
	if (percent == 100) {
		setresolution = 1;
		use_extended_resolution = true;
	}
	else if (percent == 0) {
		use_extended_resolution = false;
	}
	else {
		setresolution = stepres - ((percent * 0.01) * stepres);
		use_extended_resolution = true;
	}

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

void MagRotaryEncoder::setsleep(bool slpact) {
	sleepmode = slpact;
}

bool MagRotaryEncoder::readsleep() {
	return sleepmode;
}

