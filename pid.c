/*
 ============================================================================
 Name        : pid.c
 Author      : Jan Henrik Lenes
 Description : Implementation of PID-algorithm
 ============================================================================
 */

#include <time.h>

// PID constants
float Kp, Ki, Kd;

float lastInput = 0.0;
float ITerm = 0.0;

struct timespec timeSpec;
unsigned long lastTime = 0;

// Restrictions on the output
int outMin, outMax;

int sampleTime = 1000;	// 1 second

int pidCompute(float setPoint, float measuredValue)
{

	// Get current time in milliseconds
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	unsigned long timeNow = timeSpec.tv_sec * 1000 + timeSpec.tv_nsec / 1000000;

	int dt = timeNow - lastTime;

	if (dt >= sampleTime)
	{

		// Calculate the terms
		float error = setPoint - measuredValue;
		ITerm += Ki * error;
		if (ITerm > outMax)
			ITerm = outMax;
		else if (ITerm < outMin)
			ITerm = outMin;
		// By using dInput instead of dError, we get no output spikes when the set point changes
		float dInput = measuredValue - lastInput;

		int output = Kp * error + ITerm - Kd * dInput;
		if (output > outMax)
			output = outMax;
		else if (output < outMin)
			output = outMin;

		// Remember some variables for next iteration
		lastInput = measuredValue;
		lastTime = timeNow;

		return output;
	}

	return -1;
}

void setPIDParameters(float P, float I, float D)
{
	float sampleTimeInSeconds = (float) sampleTime / 1000.0;
	Kp = P;

	// Incorporate sample time into coefficients to decrease number of calculations
	Ki = I * sampleTimeInSeconds;
	Kd = D / sampleTimeInSeconds;
}

void setSampleTime(int newSampleTime)
{
	if (newSampleTime > 0)
	{
		float ratio = (float) newSampleTime / (float) sampleTime;
		Ki *= ratio;
		Kd /= ratio;
		sampleTime = (int) newSampleTime;
	}
}

void setOutputLimits(float min, double max)
{
	if (min > max)
		return;
	outMin = min;
	outMax = max;
}

