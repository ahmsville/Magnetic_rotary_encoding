#include "MagRotaryEncoding.h"
#include <iostream>
/*
This library was written for magnetic rotation detection
This library also features a haptics controller, that allows you to connect and control a vibration motor as a step count haptic feedback.

....Written By Ahmed Oyenuga (Ahmsville 2019).
*/

#define RISING 0x08
#define FALLING 0x04
volatile bool sleepmode = false;

bool MagRotaryEncoder::passSynctest(int newspos)
{
	// printf("startpos %d newpos %d \n", startposition, newspos);
	if (encoderResolution > 0)
	{
		if (startposition == 1)
		{
			if (newspos == 4 || newspos == 2)
			{
				return true;
			}
		}
		else if (startposition == 2)
		{
			if (newspos == 1 || newspos == 3)
			{
				return true;
			}
		}
		else if (startposition == 3)
		{
			if (newspos == 2 || newspos == 4)
			{
				return true;
			}
		}
		else if (startposition == 4)
		{
			if (newspos == 3 || newspos == 1)
			{
				return true;
			}
		}
		return false;
	}
	else
	{
		return true;
	}
}

int MagRotaryEncoder::singleSensor_INT(int incenter)
{
	if (!use_extended_resolution) // extended res is not used
	{

		get_sensorValue(2); // read sensor 2
		get_sensorValue(1);
		// set new startposition
		if (sensor2 != 0)
		{ // value read successfully, process here
			int temppos = get_encodingState();

			if (temppos == prevstartposition)
			{
				activesensorINT = 1;
				// INTProcessed = false;
				return activesensorINT;
			}
			if (passSynctest(temppos))
			{
				startposition = temppos;
				prevsensor1 = Mid;
				INTProcessed = true;
				// keep count

				rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
				processCOUNT(prevstartposition, startposition);
				prevstartposition = startposition;
				timetracker = (time_us_64() / 1000);

				activesensorINT = 2;
				return activesensorINT;
			}
			else
			{
				count = 10000;
				inSync = false;
				return activesensorINT;
			}
		}
		else
		{ // process in loop
			INTProcessed = false;
			INT1fired = true;
			INT2fired = false;
			rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
			timetracker = (time_us_64() / 1000);
			activesensorINT = 2;
			return activesensorINT;
		}
	}
	else
	{
		recaliberate_startPosition();
	}
	activesensorINT = 1;
	return activesensorINT;
}

int MagRotaryEncoder::processCOUNT(int prevpos, int newpos)
{
	if (newpos == 1)
	{
		if (prevpos == 4)
		{
			count += 1; // clockwise
			setrotationRate();
			haptics(1);
		}
		else if (prevpos == 2)
		{
			count -= 1; // counterclockwise
			setrotationRate();
			haptics(1);
		}
	}
	else if (newpos == 3)
	{
		if (prevpos == 2)
		{
			count += 1; // clockwise
			setrotationRate();
			haptics(1);
		}
		else if (prevpos == 4)
		{
			count -= 1; // counterclockwise
			setrotationRate();
			haptics(1);
		}
	}
	else if (newpos == 2)
	{
		if (prevpos == 1)
		{
			count += 1; // clockwise
			setrotationRate();
			haptics(1);
		}
		else if (prevpos == 3)
		{
			count -= 1; // counterclockwise
			setrotationRate();
			haptics(1);
		}
	}
	else if (newpos == 4)
	{
		if (prevpos == 3)
		{
			count += 1; // clockwise
			setrotationRate();
			haptics(1);
		}
		else if (prevpos == 1)
		{
			count -= 1; // counterclockwise
			setrotationRate();
			haptics(1);
		}
	}
	return count;
}

int MagRotaryEncoder::processPULSE(int currentpos)
{
	//************************************************************************//
	if (currentpos == 4) // sensor 2 active check overlap on sensor one
	{
		// CW
		if (gpio_get(get_sensorINTpin(3)) == 1) // state 3 - pin still high
		{
			processS2_N_INT = false; // disable yourown state
			processS1_S_INT = false;
			processS2_S_INT = true;
			processS1_N_INT = true;
			// set indeterminate flag
			flagState = 3;
		}
		// CCW
		else if (gpio_get(get_sensorINTpin(1)) == 1) // state 1 - pin still high
		{
			processS2_N_INT = false; // disable yourown state
			processS1_N_INT = false;
			processS2_S_INT = true;
			processS1_S_INT = true;
			// set indeterminate flag
			flagState = 1;
		}
		else
		{							 // determinate state
			processS2_N_INT = false; // disable yourown sensor group
			processS1_S_INT = true;
			processS2_S_INT = false;
			processS1_N_INT = true;
			// set determinate flag
			flagState = 0;
		}
	}
	if (currentpos == 2) // sensor 2 active check overlap on sensor one
	{
		// CW
		if (gpio_get(get_sensorINTpin(1)) == 1) // state 1 - pin still high
		{
			processS2_S_INT = false; // disable yourown state
			processS1_N_INT = false;
			processS2_N_INT = true;
			processS1_S_INT = true;
			// set indeterminate flag
			flagState = 1;
		}
		// CCW
		else if (gpio_get(get_sensorINTpin(3)) == 1) // state 3 - pin still high
		{
			processS2_S_INT = false; // disable yourown state
			processS1_S_INT = false;
			processS2_N_INT = true;
			processS1_N_INT = true;
			// set indeterminate flag
			flagState = 3;
		}
		else
		{							 // determinate state
			processS2_N_INT = false; // disable yourown sensor group
			processS1_S_INT = true;
			processS2_S_INT = false;
			processS1_N_INT = true;
			// set determinate flag
			flagState = 0;
		}
	}
	if (currentpos == 3) // sensor 1 active check overlap on sensor two
	{
		// CW
		if (gpio_get(get_sensorINTpin(2)) == 1) // state 2 - pin still high
		{
			processS1_S_INT = false; // disable yourown state
			processS2_S_INT = false;
			processS2_N_INT = true;
			processS1_N_INT = true;
			// set indeterminate flag
			flagState = 2;
		}
		// CCW
		else if (gpio_get(get_sensorINTpin(4)) == 1) // state 4 - pin still high
		{
			processS1_S_INT = false; // disable yourown state
			processS2_N_INT = false;
			processS2_S_INT = true;
			processS1_N_INT = true;
			// set indeterminate flag
			flagState = 4;
		}
		else
		{							// determinate state
			processS2_N_INT = true; // disable yourown sensor group
			processS1_S_INT = false;
			processS2_S_INT = true;
			processS1_N_INT = false;
			// set determinate flag
			flagState = 0;
		}
	}
	if (currentpos == 1) // sensor 1 active check overlap on sensor two
	{
		// CW
		if (gpio_get(get_sensorINTpin(4)) == 1) // state 4 - pin still high
		{
			processS1_N_INT = false; // disable yourown state
			processS2_N_INT = false;
			processS2_S_INT = true;
			processS1_S_INT = true;
			// set indeterminate flag
			flagState = 4;
		}
		// CCW
		else if (gpio_get(get_sensorINTpin(2)) == 1) // state 2 - pin still high
		{
			processS1_N_INT = false; // disable yourown state
			processS2_S_INT = false;
			processS2_N_INT = true;
			processS1_S_INT = true;
			// set indeterminate flag
			flagState = 2;
		}
		else
		{							// determinate state
			processS2_N_INT = true; // disable yourown sensor group
			processS1_S_INT = false;
			processS2_S_INT = true;
			processS1_N_INT = false;
			// set determinate flag
			flagState = 0;
		}
	}
	return flagState;
}

int MagRotaryEncoder::processExINT(int encstate)
{
	if (startposition == 0)
	{
		startposition = encstate;
		return 0;
	}
	if (startposition != encstate)
	{
		rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate

		if (encstate == 1 && processS1_N_INT)
		{
			if (flagState != 0) // encoder reversed interpolate between states
			{
				if (passSynctest(encstate))
				{
					processCOUNT(startposition, encstate);
				}
				else
				{
					processCOUNT(startposition, flagState);
					processCOUNT(flagState, encstate);
				}
			}

			else
			{
				processCOUNT(startposition, encstate);
			}
			processPULSE(encstate);

			timetracker = (time_us_64() / 1000);
			startposition = encstate;
		}
		else if (encstate == 3 && processS1_S_INT)
		{
			if (flagState != 0) // encoder reversed interpolate between states
			{
				if (passSynctest(encstate))
				{
					processCOUNT(startposition, encstate);
				}
				else
				{
					processCOUNT(startposition, flagState);
					processCOUNT(flagState, encstate);
				}
			}
			else
			{
				processCOUNT(startposition, encstate);
			}
			processPULSE(encstate);

			timetracker = (time_us_64() / 1000);
			startposition = encstate;
		}
		else if (encstate == 2 && processS2_S_INT)
		{
			if (flagState != 0) // encoder reversed interpolate between states
			{
				if (passSynctest(encstate))
				{
					processCOUNT(startposition, encstate);
				}
				else
				{
					processCOUNT(startposition, flagState);
					processCOUNT(flagState, encstate);
				}
			}
			else
			{
				processCOUNT(startposition, encstate);
			}
			processPULSE(encstate);

			timetracker = (time_us_64() / 1000);
			startposition = encstate;
		}
		else if (encstate == 4 && processS2_N_INT)
		{
			if (flagState != 0) // encoder reversed interpolate between states
			{
				if (passSynctest(encstate))
				{
					processCOUNT(startposition, encstate);
				}
				else
				{
					processCOUNT(startposition, flagState);
					processCOUNT(flagState, encstate);
				}
			}
			else
			{
				processCOUNT(startposition, encstate);
			}
			processPULSE(encstate);

			timetracker = (time_us_64() / 1000);
			startposition = encstate;
		}
		if (startposition == encstate && count == 0)
		{
			if (encoderResolution > 0)
			{
				printf("missed step\n");
				// count = 10000;
				// inSync = false;
			}
		}
	}

	return startposition;
}

int MagRotaryEncoder::sensor1_INT()
{
	// // printf("s1intinterrupt %d \n",activesensorINT);
	// //  printf("using digital ====> prevstartpos--> %d Newstartpos--> %d \n", prevstartposition, startposition);
	// if (!use_extended_resolution) // extended res is not used
	// {

	// 	get_sensorValue(2); // read sensor 2
	// 	get_sensorValue(1);
	// 	// set new startposition
	// 	if (sensor2 != 0)
	// 	{ // value read successfully, process here
	// 		int temppos = get_encodingState();

	// 		if (temppos == prevstartposition)
	// 		{
	// 			activesensorINT = 1;
	// 			// INTProcessed = false;
	// 			return activesensorINT;
	// 		}
	// 		if (passSynctest(temppos))
	// 		{
	// 			startposition = temppos;
	// 			prevsensor1 = Mid;
	// 			INTProcessed = true;
	// 			// keep count

	// 			rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
	// 			if (startposition == 2)
	// 			{ // new startposition is not == 1 or 4
	// 				if (prevstartposition == 1)
	// 				{ // clockwise
	// 					count++;
	// 				}
	// 				else if (prevstartposition == 3)
	// 				{ // counterclockwise
	// 					count--;
	// 				}
	// 				prevstartposition = startposition;
	// 			}

	// 			else if (startposition == 4)
	// 			{ // new startposition == 4
	// 				if (prevstartposition == 3)
	// 				{ // clockwise
	// 					count++;
	// 				}
	// 				else if (prevstartposition == 1)
	// 				{ // counterclockwise
	// 					count--;
	// 				}
	// 				prevstartposition = startposition;
	// 			}
	// 			prevstartposition = startposition;
	// 			timetracker = (time_us_64() / 1000);

	// 			activesensorINT = 2;
	// 			return activesensorINT;
	// 		}
	// 		else
	// 		{
	// 			inSync = false;
	// 			return activesensorINT;
	// 		}
	// 	}
	// 	else
	// 	{ // process in loop
	// 		INTProcessed = false;
	// 		INT1fired = true;
	// 		INT2fired = false;
	// 		rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
	// 		timetracker = (time_us_64() / 1000);
	// 		activesensorINT = 2;
	// 		return activesensorINT;
	// 	}
	// }
	// else
	// {
	// 	INT1fired = true;
	// 	INT2fired = false;
	// }
	// activesensorINT = 1;
	return activesensorINT;
}

int MagRotaryEncoder::sensor2_INT()
{
	// // printf("s1intinterrupt %d \n",activesensorINT);
	// //  printf("using digital ====> prevstartpos--> %d Newstartpos--> %d \n", prevstartposition, startposition);
	// if (!use_extended_resolution) // extended res is not used
	// {

	// 	get_sensorValue(1); // read sensor 1
	// 	get_sensorValue(2);
	// 	// set new startposition
	// 	if (sensor1 != 0)
	// 	{ // value read successfully, process here
	// 		int temppos = get_encodingState();

	// 		if (temppos == prevstartposition)
	// 		{
	// 			activesensorINT = 2;
	// 			// INTProcessed = false;
	// 			return activesensorINT;
	// 		}
	// 		if (passSynctest(temppos))
	// 		{
	// 			startposition = temppos;
	// 			prevsensor2 = Mid;
	// 			INTProcessed = true;

	// 			// keep count

	// 			rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
	// 			if (startposition == 3)
	// 			{ // new startposition is not == 1 or 4
	// 				if (prevstartposition == 2)
	// 				{ // clockwise
	// 					count++;
	// 				}
	// 				else if (prevstartposition == 4)
	// 				{ // counterclockwise
	// 					count--;
	// 				}
	// 				prevstartposition = startposition;
	// 			}
	// 			else if (startposition == 1)
	// 			{ // new startposition == 1
	// 				if (prevstartposition == 4)
	// 				{ // clockwise
	// 					count++;
	// 				}
	// 				else if (prevstartposition == 2)
	// 				{ // counterclockwise
	// 					count--;
	// 				}
	// 				prevstartposition = startposition;
	// 			}
	// 			prevstartposition = startposition;
	// 			timetracker = (time_us_64() / 1000);
	// 			activesensorINT = 1;
	// 			return activesensorINT;
	// 		}
	// 		else
	// 		{
	// 			inSync = false;
	// 			return activesensorINT;
	// 		}
	// 	}
	// 	else
	// 	{ // process in loop
	// 		INTProcessed = false;
	// 		INT2fired = true;
	// 		INT1fired = false;
	// 		rotationrate = (time_us_64() / 1000) - timetracker; // rotation step rate
	// 		timetracker = (time_us_64() / 1000);
	// 		activesensorINT = 1;
	// 		return activesensorINT;
	// 	}
	// }
	// else
	// {
	// 	INT2fired = true;
	// 	INT1fired = false;
	// }
	// activesensorINT = 2;
	return activesensorINT;
}

MagRotaryEncoder::MagRotaryEncoder(int s1, int ch1, int s2, int ch2)
{ // set sensor pins

	adc_init();
	sensor1_pin = s1;
	sensor1_adc_channel = ch1;
	adc_gpio_init(s1);
	sensor2_pin = s2;
	sensor2_adc_channel = ch2;
	adc_gpio_init(s2);
	useInterrupt = false;

	// printf("s1s %d s1n %d s2s %d s2n %d \n",s1,ch1,s2,ch2);
}

MagRotaryEncoder::MagRotaryEncoder(int s1, int ch1, int s2, int ch2, int s1INTpin, int s2INTpin)
{ // set sensor pins
	adc_init();
	sensor1_pin = s1;
	sensor1_adc_channel = ch1;
	adc_gpio_init(s1);
	sensor2_pin = s2;
	sensor2_adc_channel = ch2;
	adc_gpio_init(s2);
	SensorINTpin[0] = s1INTpin;
	gpio_init(s1INTpin);
	gpio_set_dir(s1INTpin, GPIO_IN);
	gpio_pull_up(s1INTpin);
	SensorINTpin[1] = s2INTpin;
	gpio_init(s2INTpin);
	gpio_set_dir(s2INTpin, GPIO_IN);
	gpio_pull_up(s2INTpin);
	useInterrupt = true;
}

void MagRotaryEncoder::setExclINTmode(int S1_INT1, int S1_INT2, int S2_INT1, int S2_INT2, bool exclint)
{
	SensorINTpin[2] = S1_INT1;
	gpio_init(S1_INT1);
	gpio_set_dir(S1_INT1, GPIO_IN);
	gpio_pull_up(S1_INT1);

	SensorINTpin[0] = S1_INT2;
	gpio_init(S1_INT2);
	gpio_set_dir(S1_INT2, GPIO_IN);
	gpio_pull_up(S1_INT2);

	SensorINTpin[1] = S2_INT1;
	gpio_init(S2_INT1);
	gpio_set_dir(S2_INT1, GPIO_IN);
	gpio_pull_up(S2_INT1);

	SensorINTpin[3] = S2_INT2;
	gpio_init(S2_INT2);
	gpio_set_dir(S2_INT2, GPIO_IN);
	gpio_pull_up(S2_INT2);
	useInterrupt = true;
	exclInterrupt = true;
}

void MagRotaryEncoder::setResistorDivider(float R1, float R2, float vcc)
{
	float newR2 = R1;
	float newR1 = R1 + R2;

	Neutral[0] = (ADCMaxRes / vcc) * (vcc * newR2 / (newR1 + newR2));

	newR1 = R1;
	newR2 = R1 + R2;

	Neutral[1] = (ADCMaxRes / vcc) * (vcc * newR2 / (newR1 + newR2));
	bound = Neutral[1] - (ADCMaxRes / 2);
}

int MagRotaryEncoder::get_sensorINTpin(int sensornum)
{
	if (sensornum == 1)
	{
		return SensorINTpin[0];
	}
	else if (sensornum == 2)
	{
		return SensorINTpin[1];
	}
	else if (sensornum == 3)
	{
		return SensorINTpin[2];
	}
	else if (sensornum == 4)
	{
		return SensorINTpin[3];
	}
	return 0;
}

void MagRotaryEncoder::set_adcresolution(int res)
{
	if (res == 12)
	{
		ADCMaxRes = 4095;
		absoluteNeutral = 2048;
		bound = 400;
		maxsway = 252;
	}
	else if (res == 10)
	{
		ADCMaxRes = 1023;
		absoluteNeutral = 512;
		bound = 10;
		maxsway = 50;
	}
}

void MagRotaryEncoder::invertCount(bool inv)
{
	invertcount = inv;
}

void MagRotaryEncoder::set_poleStateValues(int sensornum, int np, int nu, int sp)
{ // set ADC values for the poles (northpole, neutral, southpole)
	if (sensornum == 1)
	{
		S1_Neutral[0] = nu - bound;
		S1_Neutral[1] = nu + bound;
		S1_North = S1_Neutral[0];
		S1_South = S1_Neutral[1];
		S1_absoluteneutral = nu;
	}
	if (sensornum == 2)
	{
		S2_Neutral[0] = nu - bound;
		S2_Neutral[1] = nu + bound;
		S2_North = S2_Neutral[0];
		S2_South = S2_Neutral[1];
		S2_absoluteneutral = nu;
	}

	southRegion = Neutral[1] + stablerange;
	northRegion = Neutral[0] - stablerange;
	Neutral[0] = nu - bound;
	Neutral[1] = nu + bound;
	Mid = nu;
}

void MagRotaryEncoder::initialize_encoder()
{																												   // initialize encoder
	set_poleStateValues(1, absoluteNeutral - bound - stepres, absoluteNeutral, absoluteNeutral + bound + stepres); // set the peak ADC values for the (sensor1, northpole, neutralstate, southpole)
	set_poleStateValues(2, absoluteNeutral - bound - stepres, absoluteNeutral, absoluteNeutral + bound + stepres); // set the peak ADC values for the (sensor2, northpole, neutralstate, southpole)
	get_sensorValue(1);
	get_sensorValue(2);
	recaliberate_startPosition();
	prevsensor1 = sensor1;
	prevsensor2 = sensor2;
}

int MagRotaryEncoder::CalibrateSensors(int snum)
{
	int slevel = 0;
	locksensorlevels = true;
	if (snum == 1)
	{
		for (size_t i = 0; i < 10; i++)
		{
			adc_select_input(sensor1_adc_channel);
			slevel += adc_read();
		}
		S1_absoluteneutral = slevel / 10;
		S1_Neutral[0] = S1_absoluteneutral - bound;
		S1_Neutral[1] = S1_absoluteneutral + bound;
		return S1_absoluteneutral;
	}
	else if (snum == 2)
	{
		for (size_t i = 0; i < 10; i++)
		{
			adc_select_input(sensor2_adc_channel);
			slevel += adc_read();
		}
		S2_absoluteneutral = slevel / 10;
		S2_Neutral[0] = S2_absoluteneutral - bound;
		S2_Neutral[1] = S2_absoluteneutral + bound;
		return S2_absoluteneutral;
	}
	return 0;
}
bool MagRotaryEncoder::LoadCalibrationData(int s1, int s2)
{
	// printf("s1 %d s2 %d \n",s1,s2);
	S1_absoluteneutral = s1;
	S1_Neutral[0] = S1_absoluteneutral - bound;
	S1_Neutral[1] = S1_absoluteneutral + bound;

	S2_absoluteneutral = s2;
	S2_Neutral[0] = S2_absoluteneutral - bound;
	S2_Neutral[1] = S2_absoluteneutral + bound;

	locksensorlevels = true;
	return true;
}

void MagRotaryEncoder::recaliberate_startPosition()
{ // sets the start position based on the ADC values
	haptics(0);
	if (!useInterrupt || use_extended_resolution)
	{ // interrupt detection is not used
		startposition = get_encodingState();
		if (prevstartposition != startposition)
		{ // if startposition didnt change
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

		count = 0;
	}

	if (startposition == 0)
	{ // STARTING
		if (!exclInterrupt)
		{
			get_sensorValue(1); // read sensor 1
			get_sensorValue(2); // read sensor 2
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
			if (comp1 < comp2) // sensor1 is in neutral
			{
				// set new startposition
				if (sensor2 > Neutral[1])
				{ // sensor1 = neutral , sensor2 = south
					startposition = 2;
					prevstartposition = 2;
					prevsensor1 = Mid;
				}
				else if (sensor2 < Neutral[0])
				{ // sensor1 = neutral , sensor2 = north
					startposition = 4;
					prevstartposition = 4;
					prevsensor1 = Mid;
				}
			}
			else if (comp2 < comp1) // sensor2 is in neutral
			{
				// set new startposition
				if (sensor1 > Neutral[1])
				{ // sensor2 = neutral , sensor1 = south
					startposition = 3;
					prevstartposition = 3;
					prevsensor2 = Mid;
				}
				else if (sensor1 < Neutral[0])
				{ // sensor2 = neutral , sensor1 = north
					startposition = 1;
					prevstartposition = 1;
					prevsensor2 = Mid;
				}
			}
			// SerialUSB.println(startposition);
		}
		else
		{
			startposition = get_encodingState();
		}
	}
}
void MagRotaryEncoder::DebugEncoder()
{
	printf("prevstartposition >> %d startposition >> %d abs_s1 >> %d abs_s2 >> %d \n", prevstartposition, startposition, S1_absoluteneutral, S2_absoluteneutral);
	printf("\n");
	printf("stepres >> %d setresolution >> %d bound >> %d \n", stepres, setresolution, bound);
	printf("\n");
	printf("S1 %d S1_S %d S1_N %d S2 %d S2_S %d S2_N %d \n", get_sensorValue(1), gpio_get(get_sensorINTpin(1)), gpio_get(get_sensorINTpin(3)), get_sensorValue(2), gpio_get(get_sensorINTpin(2)), gpio_get(get_sensorINTpin(4)));
	printf("\n");
}

int MagRotaryEncoder::get_sensorValue(int sensornum)
{
	if (sensornum == 1)
	{
		adc_select_input(sensor1_adc_channel);
		sensor1 = adc_read();
		// dynamic sensor value update
		if (!locksensorlevels)
		{
			if (sensor1 > S1_South || sensor1 < S1_North)
			{
				adc_select_input(sensor2_adc_channel);
				sensor2 = adc_read();
				if (sensor2 < absoluteNeutral + maxsway && sensor2 > absoluteNeutral - maxsway)
				{
					if (sensor1 > S1_South)
					{
						S1_South = sensor1;
						S2_absoluteneutral = sensor2;
						S2_Neutral[0] = S2_absoluteneutral - bound;
						S2_Neutral[1] = S2_absoluteneutral + bound;
						if (use_extended_resolution)
						{
							calibrateStepRes_HR();
						}
					}
					else if (sensor1 < S1_North)
					{
						S1_North = sensor1;
						S2_absoluteneutral = sensor2;
						S2_Neutral[0] = S2_absoluteneutral - bound;
						S2_Neutral[1] = S2_absoluteneutral + bound;
						if (use_extended_resolution)
						{
							calibrateStepRes_HR();
						}
					}
				}
			}
		}

		if (use_extended_resolution)
		{
			sensor1 = (sensor1 * alpha) + (prevsmoothsensor1 * (1 - alpha));
			prevsmoothsensor1 = sensor1;
		}
		return sensor1;
	}
	else if (sensornum == 2)
	{
		adc_select_input(sensor2_adc_channel);
		sensor2 = adc_read();
		// dynamic sensor value update
		if (!locksensorlevels)
		{
			if (sensor2 > S2_South || sensor2 < S2_North)
			{
				adc_select_input(sensor1_adc_channel);
				sensor1 = adc_read();
				if (sensor1 < absoluteNeutral + maxsway && sensor1 > absoluteNeutral - maxsway)
				{
					if (sensor2 > S2_South)
					{
						S2_South = sensor2;
						S1_absoluteneutral = sensor1;
						S1_Neutral[0] = S1_absoluteneutral - bound;
						S1_Neutral[1] = S1_absoluteneutral + bound;
						if (use_extended_resolution)
						{
							calibrateStepRes_HR();
						}
					}
					else if (sensor2 < S2_North)
					{
						S2_North = sensor2;
						S1_absoluteneutral = sensor1;
						S1_Neutral[0] = S1_absoluteneutral - bound;
						S1_Neutral[1] = S1_absoluteneutral + bound;
						if (use_extended_resolution)
						{
							calibrateStepRes_HR();
						}
					}
				}
			}
		}

		if (use_extended_resolution)
		{
			sensor2 = (sensor2 * alpha) + (prevsmoothsensor2 * (1 - alpha));
			prevsmoothsensor2 = sensor2;
		}
		return sensor2;
	}
	return 0;
}

int MagRotaryEncoder::get_encodingState()
{

	if (!exclInterrupt)
	{
		if (sensor1 < S1_Neutral[0] && sensor2 >= (S2_Neutral[0]) && sensor2 <= (S2_Neutral[1]))
		{ // sensor1 = north , sensor2 = neutral
			return 1;
		}
		else if (sensor1 >= (S1_Neutral[0]) && sensor1 <= (S1_Neutral[1]) && sensor2 > S2_Neutral[1])
		{ // sensor1 = neutral , sensor2 = south
			return 2;
		}
		else if (sensor1 > S1_Neutral[1] && sensor2 >= (S2_Neutral[0]) && sensor2 <= (S2_Neutral[1]))
		{ // sensor1 = south , sensor2 = neutral
			return 3;
		}
		else if (sensor1 >= (S1_Neutral[0]) && sensor1 <= (S1_Neutral[1]) && sensor2 < S2_Neutral[0])
		{ // sensor1 = neutral , sensor2 = north
			return 4;
		}
	}
	else
	{
		// printf("here %d \n",gpio_get(SensorINTpin[2]));
		// digital only detection
		if (gpio_get(SensorINTpin[2]) == 1)
		{

			if (gpio_get(SensorINTpin[1]) == gpio_get(SensorINTpin[3]))
			{
				return 3;
			}
		}
		else if (gpio_get(SensorINTpin[1]) == 1)
		{

			if (gpio_get(SensorINTpin[0]) == gpio_get(SensorINTpin[2]))
			{
				return 2;
			}
		}
		else if (gpio_get(SensorINTpin[0]) == 1)
		{

			if (gpio_get(SensorINTpin[1]) == gpio_get(SensorINTpin[3]))
			{
				return 1;
			}
		}
		else if (gpio_get(SensorINTpin[3]) == 1)
		{

			if (gpio_get(SensorINTpin[0]) == gpio_get(SensorINTpin[2]))
			{
				return 4;
			}
		}
		return startposition;
	}

	return startposition;
}

int MagRotaryEncoder::get_encodingState(int incenter)
{

	if (incenter == 1)
	{
		if (sensor2 > S2_Neutral[1])
		{ // sensor1 = neutral , sensor2 = south
			return 2;
		}

		else if (sensor2 < S2_Neutral[0])
		{ // sensor1 = neutral , sensor2 = north
			return 4;
		}
	}
	else if (incenter == 2)
	{
		if (sensor1 > S1_Neutral[1])
		{ // sensor2 = neutral , sensor1 = south
			return 3;
		}

		else if (sensor1 < S1_Neutral[0])
		{ // sensor2 = neutral , sensor1 = north
			return 1;
		}
	}

	return startposition;
}

int MagRotaryEncoder::get_currentSensorValue(int sensornum)
{
	if (sensornum == 1)
	{
		return sensor1;
	}
	else if (sensornum == 2)
	{
		return sensor2;
	}
	return 0;
}

int MagRotaryEncoder::get_encResCount(int retstep)
{
	// keep counter value in the encoder's resolution range
	if (retstep < 0)
	{
		for (int i = 0; i > retstep; i--)
		{
			if ((encoderPosition - 1) >= 1)
			{
				encoderPosition = encoderPosition - 1;
			}
			else if ((encoderPosition - 1) < 1)
			{
				encoderPosition = encoderResolution;
			}
		}
	}
	else if (retstep > 0)
	{
		for (int i = 0; i < retstep; i++)
		{
			if ((encoderPosition + 1) <= encoderResolution)
			{
				encoderPosition = encoderPosition + 1;
			}
			else if ((encoderPosition + 1) > encoderResolution)
			{
				encoderPosition = 1;
			}
		}
	}
	if (encoderPosition == 1)
	{
		int passmark = 0;
		inSync = false;
		// printf("storedstart %d actualstart %d \n", storedstartposition, startposition);
		if (storedstartposition == startposition)
		{

			passmark += 1;
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
	else
	{
		encoderPosition = -1;
		return encoderPosition;
	}
}

void MagRotaryEncoder::set_encoderResolution(int res)
{
	encoderResolution = res;
	encoderPosition = 0;
	storedstartposition = 0;
	storedsensor1state = 0;
	storedsensor2state = 0;
	inSync = false;
}

int MagRotaryEncoder::setToStart()
{
	// printf("Neutral 1 %d Neutral 0 %d \n", Neutral[1], Neutral[0]);
	encoderPosition = 1;

	startposition = 0;
	get_sensorValue(1);
	get_sensorValue(2);
	recaliberate_startPosition();
	count = 0;
	// printf("newsetstart %d bound %d \n",startposition,bound);
	if (!exclInterrupt)
	{
		if (startposition != 0)
		{
			storedstartposition = startposition;
			storedsensor1state = sensor1;
			storedsensor2state = sensor2;
			inSync = true;
			return startposition;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		printf("waiting for interrupt......");
		while (startposition == 0)
		{
			recaliberate_startPosition();
		}
		storedstartposition = startposition;
		storedsensor1state = sensor1;
		storedsensor2state = sensor2;
		inSync = true;
		return startposition;
	}
}

int MagRotaryEncoder::detect_rotation()
{ // openloop rotation encoding function
	// printf("s1s %d s1n %d s2s %d s2n %d \n",SensorINTpin[0],SensorINTpin[2],SensorINTpin[1],SensorINTpin[3]);
	if (!useInterrupt)
	{ // interrupt detection is not used
		// printf("s1absneutral %d, s2absneutral %d, bound %d \n", S1_absoluteneutral, S2_absoluteneutral, bound);
		get_sensorValue(1);
		get_sensorValue(2);
		int newstate = get_encodingState();
		if (newstate != startposition)
		{
			if (!passSynctest(newstate))
			{
				count = 10000;
				inSync = false;
			}
		}

		// printf("prev %d newstartpos %d \n",startposition,newstate);
		processCOUNT(startposition, newstate); // printf("startpos %d, s1 %d,s2 %d \n",startposition,sensor1,sensor2);

		if (count != 0)
		{
			if (invertcount)
			{
				count *= -1;
			}
			if (encoderResolution > 0)
			{
				if (inSync)
				{
					tempcount = count;
					recaliberate_startPosition();
					tempcount = get_encResCount(tempcount);
					// SerialUSB.print(tempcount);
				}
				else
				{
					recaliberate_startPosition();
					tempcount = -1;
				}
			}
			else
			{
				if (returnRateCount)
				{
					applyrateMultiplier();
				}
				tempcount = count;
				recaliberate_startPosition();
			}
			count = 0;
			return tempcount;
		}
		else
		{
			tempcount = count;
			recaliberate_startPosition();
			return 0;
		}
	}
	else
	{
		if (!exclInterrupt)
		{
			if (INTProcessed)
			{ // processed in interrupt

				if (count != 0)
				{
					if (invertcount)
					{
						count *= -1;
					}

					if (encoderResolution > 0)
					{
						if (inSync)
						{
							tempcount = count;
							recaliberate_startPosition();
							tempcount = get_encResCount(tempcount);
							// SerialUSB.print(tempcount);
						}
						else
						{
							tempcount = -1;
						}
					}
					else
					{
						if (returnRateCount)
				{
					applyrateMultiplier();
				}
						tempcount = count;
						recaliberate_startPosition();
					}
					count = 0;
					return tempcount;
				}
				else
				{
					tempcount = count;
					recaliberate_startPosition();
					return 0;
				}
			}
			else
			{
				if (!gpio_get(SensorINTpin[1])) // int1 active
				{
					get_sensorValue(2);
					startposition = get_encodingState(1);
					if (startposition != prevstartposition)
					{
						setrotationRate();
						processCOUNT(prevstartposition, startposition);
						prevstartposition = startposition;
						INTProcessed = true;
					}
				}
				else if (!gpio_get(SensorINTpin[0])) // int2 active
				{
					get_sensorValue(1);
					startposition = get_encodingState(2);
					if (startposition != prevstartposition)
					{
						setrotationRate();
						processCOUNT(prevstartposition, startposition);
						prevstartposition = startposition;
						INTProcessed = true;
					}
				}
				if (count != 0)
				{
					if (invertcount)
					{
						count *= -1;
					}

					if (encoderResolution > 0)
					{
						if (inSync)
						{
							tempcount = count;
							tempcount = get_encResCount(tempcount);
							// SerialUSB.print(tempcount);
						}
						else
						{
							tempcount = -1;
						}
					}
					else
					{
						if (returnRateCount)
				{
					applyrateMultiplier();
				}
						tempcount = count;
						// recaliberate_startPosition();
					}
					count = 0;
					return tempcount;
				}
				else
				{
					tempcount = count;
					recaliberate_startPosition();
					return 0;
				}
			}
			tempcount = count;
			// recaliberate_startPosition();
			return 0;
		}
		else
		{

			if (count != 0)
			{
				if (invertcount)
				{
					count *= -1;
				}
				if (encoderResolution > 0)
				{
					if (inSync)
					{
						tempcount = count;
						recaliberate_startPosition();
						tempcount = get_encResCount(tempcount);
						// SerialUSB.print(tempcount);
					}
					else
					{
						tempcount = -1;
					}
				}
				else
				{
					if (returnRateCount)
				{
					applyrateMultiplier();
				}
					tempcount = count;
					// recaliberate_startPosition();
				}
				count = 0;
				return tempcount;
			}
			if (!INTProcessed)
			{
				// recaliberate_startPosition();
			}
			return 0;
		}
	}
}
void MagRotaryEncoder::setrotationRate()
{
	rotationrate = (time_us_64() / 1000) - timetracker;
	timetracker = (time_us_64() / 1000);
}
void MagRotaryEncoder::applyrateMultiplier()
{
	if (rotationrate > 0)
	{ // avoid negative values
		if (rotationrate < timetomultiply)
		{ // not fast anough for multiplier
			float calcmul = 1 - ((float)rotationrate / (float)timetomultiply);
			count = count * multiplier * calcmul;
			// timetracker = (time_us_64()/1000);
		}
	}
}
int MagRotaryEncoder::detect_rotationWithRate(bool act)
{ 
	returnRateCount = act;
}

int MagRotaryEncoder::detect_rotationHR()
{ // openloop rotation encoding function

	int sensvalrange = 0;

	if (!useInterrupt || use_extended_resolution)
	{ // interrupt detection is not used
		if (startposition == 1)
		{ // sensor2 is in neutral
			get_sensorValue(1);
			get_sensorValue(2);
			if (!(sensor2 < S2_Neutral[1] && sensor2 > S2_Neutral[0]))
			{ // analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor2 != Mid)
				{
					if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution))
					{
						// get distance from north and south pole magnets
						distance = prevsensor2 - sensor2;
						// check rotation direction with distance values
						if (distance > 0)
						{ // moved towards magnet northpole
							prevsensor2 = sensor2 + setresolution - (setresolution - 1);
							rotation_action(0);
							haptics(1);
						}
						else if (distance < 0)
						{ // moved towards magnet southpole
							prevsensor2 = sensor2 - setresolution + (setresolution - 1);
							rotation_action(1);
							haptics(1);
						}
					}
				}
				else
				{
					prevsensor2 = sensor2;
				}
			}
		}

		else if (startposition == 2)
		{ // sensor1 is in neutral
			get_sensorValue(1);
			get_sensorValue(2);
			if (!(sensor1 < S1_Neutral[1] && sensor1 > S1_Neutral[0]))
			{ // analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor1 != Mid)
				{
					// check for actual rotation
					if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution))
					{
						// get distance from north and south pole magnets
						distance = prevsensor1 - sensor1;
						// check rotation direction with distance values
						if (distance > 0)
						{ // moved towards magnet northpole
							prevsensor1 = sensor1 + setresolution - (setresolution - 1);
							rotation_action(0);
							haptics(1);
						}
						else if (distance < 0)
						{ // moved towards magnet southpole
							prevsensor1 = sensor1 - setresolution + (setresolution - 1);
							rotation_action(1);
							haptics(1);
						}
					}
				}
				else
				{
					prevsensor1 = sensor1;
				}
			}
		}

		else if (startposition == 3)
		{ // sensor2 is in neutral
			get_sensorValue(1);
			get_sensorValue(2);
			if (!(sensor2 < S2_Neutral[1] && sensor2 > S2_Neutral[0]))
			{ // analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor2 != Mid)
				{
					// check for actual rotation
					if (sensor2 > (prevsensor2 + setresolution) || sensor2 < (prevsensor2 - setresolution))
					{
						distance = prevsensor2 - sensor2;
						// check rotation direction with distance values
						if (distance > 0)
						{ // moved towards magnet northpole
							prevsensor2 = sensor2 + setresolution - (setresolution - 1);
							rotation_action(1);
							haptics(1);
						}
						else if (distance < 0)
						{ // moved towards magnet southpole
							prevsensor2 = sensor2 - setresolution + (setresolution - 1);
							rotation_action(0);
							haptics(1);
						}
					}
				}
				else
				{
					prevsensor2 = sensor2;
				}
			}
		}

		else if (startposition == 4)
		{ // sensor1 is in neutral
			get_sensorValue(1);
			get_sensorValue(2);
			if (!(sensor1 < S1_Neutral[1] && sensor1 > S1_Neutral[0]))
			{ // analog value is not in the neutral range
				// check for actual rotation
				if (prevsensor1 != Mid)
				{
					// check for actual rotation
					if (sensor1 > (prevsensor1 + setresolution) || sensor1 < (prevsensor1 - setresolution))
					{
						// get distance from north and south pole magnets
						distance = prevsensor1 - sensor1;
						// check rotation direction with distance values
						if (distance > 0)
						{ // moved towards magnet northpole
							prevsensor1 = sensor1 + setresolution - (setresolution - 1);
							;
							rotation_action(1);
							haptics(1);
						}
						else if (distance < 0)
						{ // moved towards magnet southpole
							prevsensor1 = sensor1 - setresolution + (setresolution - 1);
							;
							rotation_action(0);
							haptics(1);
						}
					}
				}
				else
				{
					prevsensor1 = sensor1;
				}
			}
		}

		else
		{
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
	return 0;
}

void MagRotaryEncoder::set_resolution(float percent)
{
	if (percent == 100)
	{
		setresolution = 1;
		use_extended_resolution = true;
	}
	else if (percent == 0)
	{
		use_extended_resolution = false;
	}
	else
	{
		setresolution = stepres - ((percent * 0.01) * stepres);
		use_extended_resolution = true;
	}
	setHresolutionpercent = percent;
}

void MagRotaryEncoder::calibrateStepRes_HR()
{
	stepres = S1_absoluteneutral - S1_South;
	if (stepres < 0) // make sure its not negative
	{
		stepres *= (-1);
	}
	set_resolution(setHresolutionpercent);
}

void MagRotaryEncoder::rotation_action(int act)
{ // sets action for clockwise and anticlockwise rotations
	if (act == 1)
	{
		count++;
	}
	else if (act == 0)
	{
		count--;
	}
}

int MagRotaryEncoder::get_bound(int sensor_num) // calculate ideal bound value for hall sensor
{
	use_extended_resolution = false;
	int loops = 2000;
	int minval = 10000;
	int maxval = 0;
	int ret = 0;
	printf("reading ADC");

	if (useInterrupt)
	{
		printf("\n");
		printf("waiting for interrupt - rotate the knob...\n");
		while (gpio_get(get_sensorINTpin(sensor_num)) == 0) // wait for interrupt
		{
			/* code */
		}
		for (size_t i = 0; i < 10000; i++)
		{
			printf("interrupt detected, reading sensor values\n");
			while (gpio_get(get_sensorINTpin(sensor_num)) == 1) // get upper and lower limit of center region for analog and digital detection
			{

				ret = get_sensorValue(sensor_num);
				if (ret < minval)
				{
					minval = ret;
				}
				if (ret > maxval)
				{
					maxval = ret;
				}
				if (i % (loops / 100) == 0)
				{
					printf(".");
				}
			}
		}
		use_extended_resolution = false;
	}
	else
	{
		for (size_t i = 0; i < loops; i++)
		{

			ret = get_sensorValue(sensor_num); // get upper and lower noise limits for abalog only detection
			if (ret < minval)
			{
				minval = ret;
			}
			if (ret > maxval)
			{
				maxval = ret;
			}
			sleep_ms(10);
			if (i % (loops / 10) == 0)
			{
				printf(".");
			}
		}
		use_extended_resolution = true;
	}
	printf("\n");
	printf("maximum value >> %d minimum value >> %d  \n", maxval, minval);
	int idealbound = maxval - minval;
	printf("ideal bound >> %d \n", idealbound);

	return idealbound;
}
void MagRotaryEncoder::set_bound(int b)
{ // this value determines the upper and lower limit of the ADC values
	bound = b;
	S1_Neutral[0] = S1_absoluteneutral - bound;
	S1_Neutral[1] = S1_absoluteneutral + bound;
	S2_Neutral[0] = S2_absoluteneutral - bound;
	S2_Neutral[1] = S2_absoluteneutral + bound;

	Neutral[0] = absoluteNeutral - bound;
	Neutral[1] = absoluteNeutral + bound;
}
void MagRotaryEncoder::useinterruptdetection(bool act)
{
	useInterrupt = act;
}

void MagRotaryEncoder::set_haptics(int pin, int duration, int strength)
{ // use to set haptics variables (arduino pwm pin, duration of haptics(ms), strength from 0-255)
	haptics_pin = pin;
	haptics_duration = duration;
	haptics_strength = strength;

	
	gpio_set_function(pin, GPIO_FUNC_PWM);
	uint slice_num = pwm_gpio_to_slice_num(pin);

	// Set period of 4 cycles (0 to 3 inclusive)
	pwm_set_wrap(slice_num, 255);
	// Set channel A output high for one cycle before dropping
	pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);
	// Set the PWM running
	pwm_set_enabled(slice_num, true);
}

void MagRotaryEncoder::haptics(int state)
{ // viberation feedback function
	if (state == 1)
	{
		pwm_set_gpio_level(haptics_pin, haptics_strength);
		haptics_ontime = (time_us_64() / 1000);
		haptics_state = 1;
	}
	else
	{
		haptics_offtime = (time_us_64() / 1000);
		if (((haptics_offtime - haptics_ontime) >= haptics_duration) && haptics_state == 1)
		{
			pwm_set_gpio_level(haptics_pin, 0);
			haptics_state = 0;
		}
	}
	/*
	analogWrite(haptics_pin, haptics_strength);
	delay(haptics_duration);
	analogWrite(haptics_pin, 0);
	*/
}

void MagRotaryEncoder::setsleep(bool slpact)
{
	sleepmode = slpact;
}

bool MagRotaryEncoder::readsleep()
{
	return sleepmode;
}
