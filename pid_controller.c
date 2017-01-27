/*
 *
 * Implementation of a simple PID-controller.
 *
 * You must call set_pid_parameters and set_output_limits
 * before calling pid_compute.
 *
 */

#include <time.h>
#include "headers/pid_controller.h"

// PID constants
float Kp, Ki, Kd;

float lastInput = 0.0;
float integralTerm = 0.0;

struct timespec timeSpec;	// Used to get current time
unsigned long lastTime = 0;

// Restrictions on the output
int outMin, outMax;

int pid_compute(float setPoint, float input)
{
	if (lastTime == 0)
		lastTime = millis();

	// get current time in milliseconds
	unsigned long timeNow = millis();

	float dt = timeNow - lastTime;
	dt /= 1000.0;	// Convert to seconds

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

	int output = Kp * error + integralTerm - Kd * dInput;

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

void set_output_limits(float min, double max)
{
	if (min > max)
		return;
	outMin = min;
	outMax = max;
}

/*
 * Returns current time in milliseconds
 */
unsigned long millis(void)
{
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (unsigned long) (timeSpec.tv_sec * 1000 + timeSpec.tv_nsec / 1000000);
}

