/**************************************************
 * FILENAME:	pid_controller.c
 *
 * DESCRIPTION:
 * 		Implementation of a PID-controller.
 *
 * PUBLIC FUNCTIONS:
 * 		PIDdata pid_compute(float input, float setpoint)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 **************************************************/

#include "headers/pid_controller.h"
#include "headers/time_utils.h"

// PID coefficients, found by tuning
#define Kp 0.059
#define Ki 0.050
#define Kd 0.035

#define N 10 // number of derivatives to average

/**************************************************
 * NAME: static float average(float array[])
 *
 * DESCRIPTION:
 * 		Calculates the average value of an array
 * INPUTS:
 * 		PARAMETERS:
 *      	float array[]:	The array to average.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	float:	The average value of the array
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 01.04.2017
 **************************************************/
static float average(float array[])
{
	float sum = 0.0;
	for (int j = 0; j < N; j++)
	{
		float value = array[j];
		sum += value;
	}
	return sum / N;
}

/**************************************************
 * NAME: PIDdata pid_compute(float input, float setpoint)
 *
 * DESCRIPTION:
 * 		Applies the PID-regulator control loop algorithm.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	float input:			The input value to regulate.
 *      	float setpoint:			The setpoint to follow.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	PIDdata:	A struct containing the power output required to
 *        				regulate the system and the PID terms.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 01.04.2017
 **************************************************/
PIDdata pid_compute(float input, float setpoint)
{
	static unsigned long lastTime;
	static float lastInput;
	static float integralTerm = MAX_OUTPUT;	// MAX_OUTPUT means no power
	static float derivativeTerms[N];
	static int i = 0;

	if (lastTime == 0UL)
	{
		lastTime = nano_time();
		lastInput = input;
	}

	// get time passed
	unsigned long timeNow = nano_time();
	float dt = nano_to_sec(timeNow - lastTime);

	float error = setpoint - input;

	// calculate the terms
	float proportionalTerm = Kp * error;
	integralTerm += Ki * error * dt;

	// ensure value is in bounds
	if (integralTerm > MAX_OUTPUT)
		integralTerm = MAX_OUTPUT;
	else if (integralTerm < MIN_OUTPUT)
		integralTerm = MIN_OUTPUT;

	float dInput = (input - lastInput) / dt;
	derivativeTerms[i++] = -Kd * dInput;
	if (i >= N)
		i = 0;
	float derivativeTerm = average(derivativeTerms);

	float output = proportionalTerm + integralTerm + derivativeTerm;

	// ensure output is in bounds
	if (output > MAX_OUTPUT)
		output = MAX_OUTPUT;
	else if (output < MIN_OUTPUT)
		output = MIN_OUTPUT;

	// remember some variables for next iteration
	lastInput = input;
	lastTime = timeNow;

	PIDdata res = { output, proportionalTerm, integralTerm, derivativeTerm };

	return res;
}
