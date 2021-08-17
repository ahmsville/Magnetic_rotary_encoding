<<<<<<< HEAD
#include "MagRotaryEncoding.h"

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/


volatile bool sleepmode = false;

int MagRotaryEncoder::sensor1_INT() {
	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(2); //read sensor 2

		//set new startposition
		if (sensor2 != 0) { //value read successfully, process here

			startposition = get_encodingState(1);
			if (startposition == prevstartposition)
			{
				activesensorINT = 2;
				INTProcessed = false;
				return 2;
			}
			prevsensor1 = Mid;
			INTProcessed = true;
			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 2) { //new startposition is not == 1 or 4
				if (prevstartposition == 1) { //clockwise
					count++;
				}
				else if (prevstartposition == 3) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}

			else if (startposition == 4) { // new startposition == 4
				if (prevstartposition == 3) { //clockwise
					count++;
				}
				else if (prevstartposition == 1) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			prevstartposition = startposition;
			timetracker = millis();
			//SerialUSB.println(startposition);
			/*
			SerialUSB.print("fired1");
			SerialUSB.print("\t");
			SerialUSB.print(count);
			SerialUSB.print("\t");
			SerialUSB.print(sensor2);
			*/
			activesensorINT = 2;
			return 2;
		}
		else { //process in loop
			INTProcessed = false;
			INT1fired = true;
			INT2fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
			activesensorINT = 2;
			return 2;
		}
	}
	else {
		INT1fired = true;
		INT2fired = false;
	}
	activesensorINT = 2;
	return 2;
}

int MagRotaryEncoder::sensor2_INT() {;
	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(1); //read sensor 1

		//set new startposition
		if (sensor1 != 0) { //value read successfully, process here

			startposition = get_encodingState(2);
			if (startposition == prevstartposition)
			{
				activesensorINT = 1;
				INTProcessed = false;
				return 1;
			}
			prevsensor2 = Mid;
			INTProcessed = true;

			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 3) { //new startposition is not == 1 or 4
				if (prevstartposition == 2) { //clockwise
					count++;
				}
				else if (prevstartposition == 4) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 1) { // new startposition == 1
				if (prevstartposition == 4) { //clockwise
					count++;
				}
				else if (prevstartposition == 2) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			prevstartposition = startposition;
			timetracker = millis();
			//SerialUSB.println(startposition);
			/*
			SerialUSB.print("fired2");
			SerialUSB.print("\t");
			SerialUSB.print(count);
			SerialUSB.print("\t");
			SerialUSB.print(sensor1);
			*/
			activesensorINT = 1;
			return 1;
		}
		else { //process in loop
			INTProcessed = false;
			INT2fired = true;
			INT1fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
			activesensorINT = 1;
			return 1;
		}
	}
	else {
		INT2fired = true;
		INT1fired = false;
	}
	activesensorINT = 1;
	return 1;
}

int MagRotaryEncoder::getActiveSensorInterrupt() {
	return activesensorINT;
}




MagRotaryEncoder::MagRotaryEncoder(int s1, int s2) {  //set sensor pins
	sensor1_pin = s1;
	//pinMode(s1, INPUT);
	sensor2_pin = s2;
	//pinMode(s2, INPUT);
	useInterrupt = false;

}
MagRotaryEncoder::MagRotaryEncoder(int s1, int s2, int s1INTpin, int s2INTpin) {  //set sensor pins
	sensor1_pin = s1;
	//pinMode(s1, INPUT);
	sensor2_pin = s2;
	//pinMode(s2, INPUT);
	SensorINTpin[0] = s1INTpin;
	//pinMode(s1INTpin, INPUT);
	SensorINTpin[1] = s2INTpin;
	//pinMode(s2INTpin, INPUT);
	useInterrupt = true;

}

MagRotaryEncoder::MagRotaryEncoder(int s1, int s2, int s1INTpin, int s2INTpin, bool usefalling) {  //set sensor pins
	sensor1_pin = s1;
	//pinMode(s1, INPUT);
	sensor2_pin = s2;
	//pinMode(s2, INPUT);
	SensorINTpin[0] = s1INTpin;
	//pinMode(s1INTpin, INPUT);
	SensorINTpin[1] = s2INTpin;
	//pinMode(s2INTpin, INPUT);
	useInterrupt = true;
	useFallingEdge = usefalling;
}


void MagRotaryEncoder::setResistorDivider(float R1, float R2, float vcc) {
	float newR2 = R1;
	float newR1 = R1 + R2;
	
	Neutral[0] = (1023 / vcc) * (vcc * newR2 / (newR1 + newR2));

	newR1 = R1;
	newR2 = R1 + R2;

	Neutral[1] = (1023 / vcc) * (vcc * newR2 / (newR1 + newR2));
}

int MagRotaryEncoder::get_sensorINTpin(int sensornum) {
	if (sensornum == 1) {
		return SensorINTpin[0];
	}
	else if (sensornum == 2) {
		return SensorINTpin[1];
	}
}

void MagRotaryEncoder::set_adcresolution(int res) {
	if (res == 12)
	{
		absoluteNeutral = 2048;
		adcVal = ADCRead(12);
	}
	else if (res == 10) {
		absoluteNeutral = 512;
		adcVal = ADCRead(10);
	}
	
}

void MagRotaryEncoder::invertCount(bool inv) {
	invertcount = inv;
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
	set_poleStateValues(1, absoluteNeutral - bound - stepres, absoluteNeutral, absoluteNeutral + bound + stepres); // set the peak ADC values for the (sensor1, northpole, neutralstate, southpole)
	set_poleStateValues(2, absoluteNeutral - bound - stepres, absoluteNeutral, absoluteNeutral + bound + stepres); // set the peak ADC values for the (sensor2, northpole, neutralstate, southpole)
	get_sensorValue(1);
	get_sensorValue(2);
	recaliberate_startPosition();
	prevsensor1 = sensor1;
	prevsensor2 = sensor2;

}



void MagRotaryEncoder::recaliberate_startPosition() {  //sets the start position based on the ADC values
	haptics(0);
	if (!useInterrupt) { //interrupt detection is not used
		startposition = get_encodingState();
		if (prevstartposition != startposition) { //if startposition didnt change
			if (startposition == 1 || startposition == 3)
			{
				prevsensor2 = Mid;
			}
			else if (startposition == 2 || startposition == 4)
			{
				prevsensor1 = Mid;
			}
			prevstartposition = startposition;
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
		else if (startposition == 0) { //STARTING
			get_sensorValue(1); //read sensor 1
			get_sensorValue(2); //read sensor 2
			int comp1 = sensor1 - absoluteNeutral;
			int comp2 = sensor2 - absoluteNeutral;
			if (comp1 < 0)
			{
				comp1 = comp1 * (-1);
			}
			if (comp2 < 0)
			{
				comp2 = comp2 * (-1);
			}
			if (comp1 < comp2)//sensor1 is in neutral
			{
				//set new startposition
				if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevstartposition = 2;
					prevsensor1 = Mid;
				}
				else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevstartposition = 4;
					prevsensor1 = Mid;
				}
			}
			else if (comp2 < comp1) //sensor2 is in neutral
			{
				//set new startposition
				if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevstartposition = 3;
					prevsensor2 = Mid;
				}
				else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevstartposition = 1;
					prevsensor2 = Mid;
				}
			}
			//SerialUSB.println(startposition);
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

int MagRotaryEncoder::get_sensorValue(int sensornum, bool ret) {
	if (sensornum == 1) {
		return adcVal.Read(sensor1_pin);
	}
	else if (sensornum == 2) {
		return adcVal.Read(sensor2_pin);
	}
}


int MagRotaryEncoder::get_encodingState() {
	if (!useautosetpoles)
	{
		if (sensor1 < Neutral[0] && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) {  //sensor1 = north , sensor2 = neutral
			return 1;
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
			return 2;
		}
		else if (sensor1 > Neutral[1] && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) { //sensor1 = south , sensor2 = neutral
			return 3;
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
			return 4;
		}
	}
	else {
		if (sensor1 < S1_North && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) {  //sensor1 = north , sensor2 = neutral
			return 1;
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 > S2_South) { //sensor1 = neutral , sensor2 = south
			return 2;
		}
		else if (sensor1 > S1_South && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) { //sensor1 = south , sensor2 = neutral
			return 3;
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 < S2_North) { //sensor1 = neutral , sensor2 = north
			return 4;
		}
	}

	return startposition;
}

int MagRotaryEncoder::get_encodingState(int incenter) {

	if (digitalRead(SensorINTpin[0]) != digitalRead(SensorINTpin[1]))
	{
		if (incenter == 1)
		{
			if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
				return 2;
			}

			else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
				return 4;
			}
		}
		else if (incenter == 2) {
			if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
				return 3;
			}

			else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
				return 1;
			}
		}
	}
		
		
		
	return startposition;
		
		
	
	
	
	
}

int MagRotaryEncoder::get_currentSensorValue(int sensornum) {
	if (sensornum == 1) {
		return sensor1;
	}
	else if (sensornum == 2) {
		return sensor2;
	}
}

int MagRotaryEncoder::get_encResCount(int retstep) {
	//keep counter value in the encoder's resolution range
	if (retstep < 0) {
		for (int i = 0; i > retstep; i--) {
			if ((encoderPosition - 1) >= 1) {
				encoderPosition = encoderPosition - 1;
			}
			else if ((encoderPosition - 1) < 1) {
				encoderPosition = encoderResolution;
			}
		}
	}
	else if (retstep > 0) {
		for (int i = 0; i < retstep; i++) {
			if ((encoderPosition + 1) <= encoderResolution) {
				encoderPosition = encoderPosition + 1;
			}
			else if ((encoderPosition + 1) > encoderResolution) {
				encoderPosition = 1;
			}
		}
	}
	//SerialUSB.println(retstep);
	//validate stored start point data
	//SerialUSB.print("\t");
	//SerialUSB.println(encoderPosition);
	if (encoderPosition == 1) {
		
		//get_sensorValue(1);
		//get_sensorValue(2);
		/*
		SerialUSB.print(storedstartposition);
		SerialUSB.print("\t");
		SerialUSB.print(startposition);
		SerialUSB.print("\t");
		SerialUSB.print(storedsensor1state);
		SerialUSB.print("\t");
		SerialUSB.print(sensor1);
		SerialUSB.print("\t");
		SerialUSB.print(storedsensor2state);
		SerialUSB.print("\t");
		SerialUSB.println(sensor2);
		*/
		int passmark = 0;
		inSync = false;
		if (storedstartposition == startposition)
		{

			passmark += 1;
			/*
			if (storedsensor1state > Neutral[1] && sensor1 > Neutral[1] || storedsensor1state < Neutral[0] && sensor1 < Neutral[0]
				|| storedsensor1state > Neutral[1] && sensor1 > Neutral[1] || storedsensor1state < Neutral[0] && sensor1 < Neutral[0]
				)
			{
				passmark += 1;
			}

			if (storedsensor2state > Neutral[1] && sensor2 > Neutral[1] || storedsensor2state < Neutral[0] && sensor2 < Neutral[0]
				|| storedsensor2state > Neutral[1] && sensor2 > Neutral[1] || storedsensor2state < Neutral[0] && sensor2 < Neutral[0]
				)
			{
				passmark += 1;
			}
			*/
		}
		if (passmark == 1)
		{
			inSync = true;
		}
	}

	if (inSync)
	{
		return encoderPosition;
	}
	else {
		encoderPosition = -1;
		return encoderPosition;
	}

}

void MagRotaryEncoder::set_encoderResolution(int res) {
	encoderResolution = res;
	encoderPosition = 0;
	storedstartposition = 0;
	storedsensor1state = 0;
	storedsensor2state = 0;
	inSync = false;
}

int MagRotaryEncoder::setToStart() {
	encoderPosition = 1;

	startposition = 0;
	//get_sensorValue(1);
	//get_sensorValue(2);
	recaliberate_startPosition();
	count = 0;


	storedstartposition = startposition;
	storedsensor1state = sensor1;
	storedsensor2state = sensor2;
	inSync = true;
	//SerialUSB.println(startposition);
/*
SerialUSB.print(storedstartposition);
SerialUSB.print("\t");
SerialUSB.print(storedsensor1state);
SerialUSB.print("\t");
SerialUSB.println(storedsensor2state);
*/
	return encoderPosition;
}

int MagRotaryEncoder::detect_rotation() {  // openloop rotation encoding function

	if (!useInterrupt) { //interrupt detection is not used
		get_sensorValue(1);
		get_sensorValue(2);
		int newstate = get_encodingState();
		if (startposition == 1) {
			if (newstate == 2)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 4)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 2) {
			if (newstate == 3)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 1)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 3) {
			if (newstate == 4)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 2)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 4) {
			if (newstate == 1)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 3)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		if (count != 0) {
			if (invertcount)
			{
				count *= -1;
			}
			haptics(1);

			if (encoderResolution > 0 && inSync)
			{
				tempcount = count;
				recaliberate_startPosition();
				tempcount = get_encResCount(tempcount);
			}
			else {
				tempcount = count;
				recaliberate_startPosition();
			}
			
			
			return tempcount;
		}
		else {
			tempcount = count;
			recaliberate_startPosition();
			return 0;
		}

	}
	else {
		if (INTProcessed) { //processed in interrupt

			if (count != 0) {
				if (invertcount)
				{
					count *= -1;
				}
				/*
				
				SerialUSB.print("count == ");
				SerialUSB.print("\t");
				SerialUSB.print(startposition);
				SerialUSB.print("\t");
				SerialUSB.println(count);
				*/
				haptics(1);

				if (encoderResolution > 0)
				{
					if (inSync)
					{
						tempcount = count;
						//SerialUSB.print(count);
						//SerialUSB.print("\t");


						//recaliberate_startPosition();
						tempcount = get_encResCount(tempcount);
						//SerialUSB.print(tempcount);
					}
					else {
						tempcount = -1;
					}
					
				}
				else {
					tempcount = count;
					//recaliberate_startPosition();
				}
				count = 0;
				return tempcount;
			}
			else {
				tempcount = count;
				recaliberate_startPosition();
				return 0;
			}
		}
		else
		{
			if (!digitalRead(SensorINTpin[1]))//int1 active
			{
				get_sensorValue(2);
				startposition = get_encodingState(1);
				if (startposition != prevstartposition)
				{
					rotationrate = millis() - timetracker;  //rotation step rate
					if (startposition == 2) { //new startposition is not == 1 or 4
						if (prevstartposition == 1) { //clockwise
							count++;
						}
						else if (prevstartposition == 3) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}

					else if (startposition == 4) { // new startposition == 4
						if (prevstartposition == 3) { //clockwise
							count++;
						}
						else if (prevstartposition == 1) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					prevstartposition = startposition;
					timetracker = millis();
					INTProcessed = true;
				}
				
			}
			else if (!digitalRead(SensorINTpin[0]))//int2 active
			{
				get_sensorValue(1);
				startposition = get_encodingState(2);
				if (startposition != prevstartposition)
				{
					rotationrate = millis() - timetracker;  //rotation step rate
					if (startposition == 3) { //new startposition is not == 1 or 4
						if (prevstartposition == 2) { //clockwise
							count++;
						}
						else if (prevstartposition == 4) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					else if (startposition == 1) { // new startposition == 1
						if (prevstartposition == 4) { //clockwise
							count++;
						}
						else if (prevstartposition == 2) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					prevstartposition = startposition;
					timetracker = millis();
					INTProcessed = true;
				}
			}
			if (count != 0) {
				if (invertcount)
				{
					count *= -1;
				}
				/*
				SerialUSB.print("count == ");
				SerialUSB.print("\t");
				SerialUSB.print(prevstartposition);
				SerialUSB.print("\t");
				SerialUSB.print(startposition);
				SerialUSB.print("\t");
				SerialUSB.println(count);
				*/
				haptics(1);

				if (encoderResolution > 0)
				{
					if (inSync)
					{
						tempcount = count;
						//SerialUSB.print(count);
						//SerialUSB.print("\t");


						//recaliberate_startPosition();
						tempcount = get_encResCount(tempcount);
						//SerialUSB.print(tempcount);
					}
					else {
						tempcount = -1;
					}

				}
				else {
					tempcount = count;
					//recaliberate_startPosition();
				}
				count = 0;
				return tempcount;
			}
			else {
				tempcount = count;
				recaliberate_startPosition();
				return 0;
			}
		}
		tempcount = count;
		//recaliberate_startPosition();
		return 0;
	}
}

int MagRotaryEncoder::detect_rotationWithRate() {  // openloop rotation encoding function
	if (!useInterrupt) { //interrupt detection is not used
		get_sensorValue(1);
		get_sensorValue(2);
		int newstate = get_encodingState();
		if (startposition == 1) {
			if (newstate == 2)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 4)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 2) {
			if (newstate == 3)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 1)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 3) {
			if (newstate == 4)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 2)
			{
				rotation_action(0);
				haptics(1);
			}
		}

		else if (startposition == 4) {
			if (newstate == 1)
			{
				rotation_action(1);
				haptics(1);
			}
			else if (newstate == 3)
			{
				rotation_action(0);
				haptics(1);
			}
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
				haptics(1);
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
			
			if (!digitalRead(SensorINTpin[1]))//int1 active
			{
				get_sensorValue(2);
				startposition = get_encodingState(1);
				if (startposition != prevstartposition)
				{
					rotationrate = millis() - timetracker;  //rotation step rate
					if (startposition == 2) { //new startposition is not == 1 or 4
						if (prevstartposition == 1) { //clockwise
							count++;
						}
						else if (prevstartposition == 3) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}

					else if (startposition == 4) { // new startposition == 4
						if (prevstartposition == 3) { //clockwise
							count++;
						}
						else if (prevstartposition == 1) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					prevstartposition = startposition;
					timetracker = millis();
					INTProcessed = true;
				}

			}
			else if (!digitalRead(SensorINTpin[0]))//int2 active
			{
				get_sensorValue(1);
				startposition = get_encodingState(2);
				if (startposition != prevstartposition)
				{
					rotationrate = millis() - timetracker;  //rotation step rate
					if (startposition == 3) { //new startposition is not == 1 or 4
						if (prevstartposition == 2) { //clockwise
							count++;
						}
						else if (prevstartposition == 4) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					else if (startposition == 1) { // new startposition == 1
						if (prevstartposition == 4) { //clockwise
							count++;
						}
						else if (prevstartposition == 2) { //counterclockwise
							count--;
						}
						prevstartposition = startposition;
					}
					prevstartposition = startposition;
					timetracker = millis();
					INTProcessed = true;
				}
			}

			//calculate rate of rotation
			if (count != 0) {
				haptics(1);
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
	Neutral[0] = absoluteNeutral - bound;
	Neutral[1] = absoluteNeutral + bound;
}
void MagRotaryEncoder::useinterruptdetection(bool act) {   //this value determines the upper and lower limit of the ADC values
	useInterrupt = act;
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

=======
#include "MagRotaryEncoding.h"

/*
This library was written based on the rotation detection method described @ ""
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/


volatile bool sleepmode = false;



int MagRotaryEncoder::sensor1_INT() {
	
	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(2); //read sensor 2
		
		//set new startposition
		if (sensor2 != 0) { //value read successfully, process here
			
			if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
				startposition = 2;
				prevsensor1 = Mid;
				INTProcessed = true;
			}

			else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
				startposition = 4;
				prevsensor1 = Mid;
				INTProcessed = true;
			}
			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 2) { //new startposition is not == 1 or 4
				if (prevstartposition == 1) { //clockwise
					count++;
				}
				else if (prevstartposition == 3) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			
			else if (startposition == 4) { // new startposition == 4
				if (prevstartposition == 3) { //clockwise
					count++;
				}
				else if (prevstartposition == 1) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			prevstartposition = startposition;
			timetracker = millis();
			//SerialUSB.println(startposition);
			/*
			SerialUSB.print("fired1");
			SerialUSB.print("\t");
			SerialUSB.print(count);
			SerialUSB.print("\t");
			SerialUSB.print(sensor2);
			*/
			return 2;
		}
		else { //process in loop
			INTProcessed = false;
			INT1fired = true;
			INT2fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
			return 2;
		}
	}
	else {
		INT1fired = true;
		INT2fired = false;
	}
	return 2;
}

int MagRotaryEncoder::sensor2_INT() {
	
	if (!use_extended_resolution) { //extended res is not used
		get_sensorValue(1); //read sensor 1
		
		//set new startposition
		if (sensor1 != 0) { //value read successfully, process here
			
			if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
				startposition = 3;
				prevsensor2 = Mid;
				INTProcessed = true;
			}

			else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
				startposition = 1;
				prevsensor2 = Mid;
				INTProcessed = true;
			}
			//keep count

			rotationrate = millis() - timetracker;  //rotation step rate
			if (startposition == 3) { //new startposition is not == 1 or 4
				if (prevstartposition == 2) { //clockwise
					count++;
				}
				else if (prevstartposition == 4) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			else if (startposition == 1) { // new startposition == 1
				if (prevstartposition == 4) { //clockwise
					count++;
				}
				else if (prevstartposition == 2) { //counterclockwise
					count--;
				}
				prevstartposition = startposition;
			}
			prevstartposition = startposition;
			timetracker = millis();
			//SerialUSB.println(startposition);
			/*
			SerialUSB.print("fired2");
			SerialUSB.print("\t");
			SerialUSB.print(count);
			SerialUSB.print("\t");
			SerialUSB.print(sensor1);
			*/
			return 1;
		}
		else { //process in loop
			INTProcessed = false;
			INT2fired = true;
			INT1fired = false;
			rotationrate = millis() - timetracker;  //rotation step rate
			timetracker = millis();
			return 1;
		}
	}
	else {
		INT2fired = true;
		INT1fired = false;
	}
	return 1;
}






MagRotaryEncoder::MagRotaryEncoder(int s1, int s2) {  //set sensor pins
	sensor1_pin = s1;
	//pinMode(s1, INPUT);
	sensor2_pin = s2;
	//pinMode(s2, INPUT);
	useInterrupt = false;

}
MagRotaryEncoder::MagRotaryEncoder(int s1, int s2, int s1INTpin, int s2INTpin) {  //set sensor pins
	sensor1_pin = s1;
	//pinMode(s1, INPUT);
	sensor2_pin = s2;
	//pinMode(s2, INPUT);
	SensorINTpin[0] = s1INTpin;
	//pinMode(s1INTpin, INPUT);
	SensorINTpin[1] = s2INTpin;
	//pinMode(s2INTpin, INPUT);
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
		if (sensor1 < Neutral[0] && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) {  //sensor1 = north , sensor2 = neutral
			startposition = 1;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor2 = Mid;
				prevstartposition = startposition;
			}
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
			startposition = 2;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor1 = Mid;
				prevstartposition = startposition;
			}
		}
		else if (sensor1 > Neutral[1] && sensor2 >= (Neutral[0]) && sensor2 <= (Neutral[1])) { //sensor1 = south , sensor2 = neutral
			startposition = 3;
			if (prevstartposition != startposition) { //if startposition didnt change
				prevsensor2 = Mid;
				prevstartposition = startposition;
			}
		}
		else if (sensor1 >= (Neutral[0]) && sensor1 <= (Neutral[1]) && sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
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
		else if (startposition == 0){
			get_sensorValue(1); //read sensor 1
			get_sensorValue(2); //read sensor 2
			int comp1 = sensor1 - 512;
			int comp2 = sensor2 - 512;
			if (comp1 < 0)
			{
				comp1 = comp1 * (-1);
			}
			if (comp2 < 0)
			{
				comp2 = comp2 * (-1);
			}
			if (comp1 < comp2)//sensor1 is in neutral
			{
				//set new startposition
				if (sensor2 > Neutral[1]) { //sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevstartposition = 2;
					prevsensor1 = Mid;
				}
				else if (sensor2 < Neutral[0]) { //sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevstartposition = 4;
					prevsensor1 = Mid;
				}
			}
			else if (comp2 < comp1) //sensor2 is in neutral
			{
				//set new startposition
				if (sensor1 > Neutral[1]) { //sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevstartposition = 3;
					prevsensor2 = Mid;
				}
				else if (sensor1 < Neutral[0]) { //sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevstartposition = 1;
					prevsensor2 = Mid;
				}
			}
			//SerialUSB.println(startposition);
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

int MagRotaryEncoder::get_encResCount(int retstep) {
	//keep counter value in the encoder's resolution range
	if (retstep < 0) {
		for (int i = 0; i > retstep; i--) {
			if ((encoderPosition - 1) >= 1) {
				encoderPosition = encoderPosition - 1;
			}
			else if ((encoderPosition - 1) < 1) {
				encoderPosition = encoderResolution;
			}
		}
	}
	else if (retstep > 0) {
		for (int i = 0; i < retstep; i++) {
			if ((encoderPosition + 1) <= encoderResolution) {
				encoderPosition = encoderPosition + 1;
			}
			else if ((encoderPosition + 1) > encoderResolution) {
				encoderPosition = 1;
			}
		}
	}
	//Serial.println(encoderPosition);
	//validate stored start point data
	//SerialUSB.print("\t");
	//SerialUSB.println(encoderPosition);
	if (encoderPosition == 1) {
		/*
		SerialUSB.print(storedstartposition);
		SerialUSB.print("\t");
		SerialUSB.print(startposition);
		SerialUSB.print("\t");
		SerialUSB.print(storedsensor1state);
		SerialUSB.print("\t");
		SerialUSB.print(sensor1);
		SerialUSB.print("\t");
		SerialUSB.print(storedsensor2state);
		SerialUSB.print("\t");
		SerialUSB.println(sensor2);
		*/
		int passmark = 0;
		inSync = false;
		if (storedstartposition == startposition)
		{
			passmark += 1;
			if (startposition == 1)
			{
				if (storedsensor1state > Neutral[1]  && sensor1 > Neutral[1] || storedsensor1state < Neutral[0] && sensor1 < Neutral[0])
				{
					passmark += 1;
				}
			}
			else if (startposition == 2)
			{
				if (storedsensor2state > Neutral[1] && sensor2 > Neutral[1] || storedsensor2state < Neutral[0] && sensor2 < Neutral[0])
				{
					passmark += 1;
				}
			}
			else if (startposition == 3)
			{
				if (storedsensor1state > Neutral[1] && sensor1 > Neutral[1] || storedsensor1state < Neutral[0] && sensor1 < Neutral[0])
				{
					passmark += 1;
				}
			}
			else if (startposition == 4)
			{
				if (storedsensor2state > Neutral[1] && sensor2 > Neutral[1] || storedsensor2state < Neutral[0] && sensor2 < Neutral[0])
				{
					passmark += 1;
				}
			}
		}
		if (passmark == 2)
		{
			inSync = true;
		}
	}
	
	if (inSync)
	{
		return encoderPosition;
	}
	else {
		encoderPosition = -1;
		return encoderPosition;
	}
	
}

void MagRotaryEncoder::set_encoderResolution(int res) {
	encoderResolution = res;
	storedstartposition = 0;
	storedsensor1state = 0;
	storedsensor2state = 0;
	inSync = false;
}

int MagRotaryEncoder::setToStart() {
	encoderPosition = 1;
	
		get_sensorValue(1);
		get_sensorValue(2);
		recaliberate_startPosition();
		count = 0;
	
		
		storedstartposition = startposition;
		storedsensor1state = sensor1;
		storedsensor2state = sensor2;
		inSync = true;
		//SerialUSB.println(startposition);
	/*
	SerialUSB.print(storedstartposition);
	SerialUSB.print("\t");
	SerialUSB.print(storedsensor1state);
	SerialUSB.print("\t");
	SerialUSB.println(storedsensor2state);
	*/
	return encoderPosition;
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
					//startposition = 0;//4;
				}
				else if (sensor2 > southRegion) {
					rotation_action(1);
					haptics(1);
					//startposition = 0;//2;
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
					//startposition = 0;//1;
				}
				else if (sensor1 > southRegion) {
					rotation_action(1);
					haptics(1);
					//startposition = 0;//3;
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
					//startposition = 0;//2;
				}
				else if (sensor2 < northRegion) {
					rotation_action(1);
					haptics(1);
					//startposition = 0;//4;
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
					//startposition = 0;//3;
				}
				else if (sensor1 < northRegion) {
					rotation_action(1);
					haptics(1);
					//startposition = 0;//1;
				}
			}
		}

		else {
			get_sensorValue(1);
			get_sensorValue(2);
		}

		
		

		if (count != 0) {
			haptics(1);
			
			if (encoderResolution > 0 && inSync)
			{
				tempcount = count;
				recaliberate_startPosition();
				tempcount = get_encResCount(tempcount);
			}
			else {
				tempcount = count;
				recaliberate_startPosition();
			}
			return tempcount;
		}
		else {
			tempcount = count;
			recaliberate_startPosition();
			return tempcount;
		}
		
	}
	else {
		if (INTProcessed) { //processed in interrupt
			
			if (count != 0) {
				/*
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

				SerialUSB.println(count);
				*/
				haptics(1);

				if (encoderResolution > 0 && inSync)
				{
					tempcount = count;
					//SerialUSB.print(count);
					//SerialUSB.print("\t");

					
					//recaliberate_startPosition();
					tempcount = get_encResCount(tempcount);
					//SerialUSB.print(tempcount);
				}
				else {
					tempcount = count;
					//recaliberate_startPosition();
				}
				count = 0;
				return tempcount;
			}
			else {
				tempcount = count;
				//recaliberate_startPosition();
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
			

			if (count != 0) {
				haptics(1);

				if (encoderResolution > 0 && inSync)
				{
					tempcount = count;
					recaliberate_startPosition();
					tempcount = get_encResCount(tempcount);
				}
				else {
					tempcount = count;
					recaliberate_startPosition();
				}
				return tempcount;
			}
			else {
				tempcount = count;
				recaliberate_startPosition();
				return tempcount;
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
				haptics(1);
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
				haptics(1);
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

>>>>>>> 9530389ef0a8ff51d15b361cc644c6b1bbdac1c1
