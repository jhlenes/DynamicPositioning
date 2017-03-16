/*
 *
 * Implementation of a simple PID-controller.
 *
 * You must call set_pid_parameters and set_output_limits
 * before calling pid_compute.
 *
 */

#include "headers/pid_controller.h"

// PID coefficients
static float Kp, Ki, Kd;

static float setPoint;

// Restrictions on the output
static float outMin, outMax;

float pid_compute(float input)
{
	// static variables are initialized only once
	static unsigned long lastTime = 0;
	static float lastInput = 0;
	static float integralTerm = 0.0;

	if (lastTime == 0)
		lastTime = nano_time();

	// get current time in nanoseconds
	unsigned long timeNow = nano_time();

	// time difference
	float dt = (float) (timeNow - lastTime);
	dt = FROM_NANOS(dt);	// Convert to seconds

	// Calculate the terms
	float error = setPoint - input;

	integralTerm += Ki * error * dt;

	// ensure value is in bounds
	if (integralTerm > outMax)
		integralTerm = outMax;
	else if (integralTerm < outMin)
		integralTerm = outMin;

	// by using change in input instead of change in error,
	// we get no output spikes when the set point changes
	float dInput = (input - lastInput) / dt;

	float output = Kp * error + integralTerm - Kd * dInput;

	// ensure output is in bounds
	if (output > outMax)
		output = outMax;
	else if (output < outMin)
		output = outMin;

	// remember some variables for next iteration
	lastInput = input;
	lastTime = timeNow;

	// Print to screen
	//printf("Kp: %5.1f Ki: %5.1f Kd: %5.1f\n", Kp * error, integralTerm, Kd * dInput);

	return output;
}

void set_pid_parameters(float P, float I, float D)
{
	Kp = P;
	Ki = I;
	Kd = D;
}

void set_pid_set_point(float value)
{
	setPoint = value;
}

void set_pid_output_limits(float min, float max)
{
	if (min > max)
		return;
	outMin = min;
	outMax = max;
}

