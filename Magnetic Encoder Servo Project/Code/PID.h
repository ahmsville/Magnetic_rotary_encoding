#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

typedef struct
{

	/* Controller gains */
	float Kp;
	float Ki;
	float Kd;

	/* Derivative low-pass filter time constant */
	float tau;

	/* Output limits */
	float limMin;
	float limMax;

	/* Integrator limits */
	float limMinInt;
	float limMaxInt;

	/* Sample time (in seconds) */
	float T;

	/* Controller "memory" */
	float integrator;
	float prevError; /* Required for integrator */
	float differentiator;
	float prevMeasurement; /* Required for differentiator */

	/* Controller output */
	float out;

} PIDController;

void PIDController_Init(PIDController *pid);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);
void PIDController_Init(PIDController *pid)
{

	/* Clear controller variables */
	pid->integrator = 0.0f;
	pid->prevError = 0.0f;

	pid->differentiator = 0.0f;
	pid->prevMeasurement = 0.0f;

	pid->out = 0.0f;
}

float PIDController_Update(PIDController *pid, float setpoint, float measurement)
{

	/*
	 * Error signal
	 */
	float error = setpoint - measurement;

	/*
	 * Proportional
	 */
	float proportional = pid->Kp * error;

	/*
	 * Integral
	 */
	pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

	/* Anti-wind-up via integrator clamping */
	if (pid->limMaxInt != 0 && pid->limMinInt != 0)
	{
		if (pid->integrator > pid->limMaxInt)
		{

			pid->integrator = pid->limMaxInt;
		}
		else if (pid->integrator < pid->limMinInt)
		{

			pid->integrator = pid->limMinInt;
		}
	}

	/*
	 * Derivative (band-limited differentiator)
	 */

	pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement) /* Note: derivative on measurement, therefore minus sign in front of equation! */
							+ (2.0f * pid->tau - pid->T) * pid->differentiator) /
						  (2.0f * pid->tau + pid->T);

	/*
	 * Compute output and apply limits
	 */
	pid->out = proportional + pid->integrator + pid->differentiator;

	if (pid->limMax != 0 && pid->limMin != 0)
	{
		if (pid->out > pid->limMax)
		{

			pid->out = pid->limMax;
		}
		else if (pid->out < pid->limMin)
		{

			pid->out = pid->limMin;
		}
	}

	/* Store error and measurement for later use */
	pid->prevError = error;
	pid->prevMeasurement = measurement;

	/* Return controller output */
	return pid->out;
}

#endif