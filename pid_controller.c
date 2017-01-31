/*
 *
 * Implementation of a simple PID-controller.
 *
 * You must call set_pid_parameters and set_output_limits
 * before calling pid_compute.
 *
 */

#include "headers/pid_controller.h"
#include "headers/my_utils.h"

// PID coefficients
static float Kp, Ki, Kd;

static int setPoint;

// Restrictions on the output
static int outMin, outMax;

int pid_compute(int input)
{
	// static variables are initialized only once
	static unsigned long lastTime = 0;
	static int lastInput = 0;
	static float integralTerm = 0.0;

	if (lastTime == 0)
		lastTime = millis();

	// get current time in milliseconds
	unsigned long timeNow = millis();

	// time difference
	float dt = (float) (timeNow - lastTime);
	dt /= 1000.0;	// Convert to seconds

	// Calculate the terms
	float error = (float) (setPoint - input);

	integralTerm += Ki * error * dt;

	// ensure value is in bounds
	if (integralTerm > outMax)
		integralTerm = (float) outMax;
	else if (integralTerm < outMin)
		integralTerm = (float) outMin;

	// by using change in input instead of change in error,
	// we get no output spikes when the set point changes
	float dInput = (float) (input - lastInput) / dt;

	int output = (int) (Kp * error + integralTerm - Kd * dInput);

	// ensure output is in bounds
	if (output > outMax)
		output = outMax;
	else if (output < outMin)
		output = outMin;

	// remember some variables for next iteration
	lastInput = input;
	lastTime = timeNow;

	return output;
}

void set_pid_parameters(float P, float I, float D)
{
	Kp = P;
	Ki = I;
	Kd = D;
}

void set_pid_set_point(int value)
{
	setPoint = value;
}

void set_pid_output_limits(int min, int max)
{
	if (min > max)
		return;
	outMin = min;
	outMax = max;
}

