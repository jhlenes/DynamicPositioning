/**************************************************
 *
 * FILENAME:	pid_controller.c
 *
 * DESCRIPTION:
 * 			Implementation of a PID-controller.
 *
 * PUBLIC FUNCTIONS:
 * 			PIDdata pid_compute(float input, float setpoint)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 *
 **************************************************/

#include <stdio.h>
#include "headers/my_utils.h"
#include "headers/pid_controller.h"

// PID coefficients
#define Kp 0.06
#define Ki 0.1
#define Kd 0.02 // Prøv 0.02 for 0.02s, og 0.01 for 0.01s
/* Tuning PID-controller
 *  MAX: 94 , MIN: 84 , {0.4, 0.4, 1.8}
 * 	MAX: 107, MIN: 101, {0.15, 0.2, 0.07}, BEST: {0.06, 0.1, 0.09}
 */

/**************************************************
 * NAME: PIDdata pid_compute(float input, float setpoint)
 *
 * DESCRIPTION:
 * 			Applies the PID-regulator control loop algorithm.
 * 			See https://en.wikipedia.org/wiki/PID_controller for more info about PID-controllers.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	float input:			The input value to regulate.
 *      	float setpoint:			The setpoint to follow.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	PIDdata:				The power to be applied in order to regulate the process. In addition,
 *        							every term of the power is also returned
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 01.04.2017
 **************************************************/
PIDdata pid_compute(float input, float setpoint)
{
	static unsigned long lastTime = 0;
	static float lastInput = 0;
	static float integralTerm = MAX_OUTPUT;

	if (lastTime == 0)
		lastTime = nano_time();

	unsigned long timeNow = nano_time();

	// time difference in seconds
	float dt = from_nanos(timeNow - lastTime);

	float error = setpoint - input;

	integralTerm += Ki * error * dt;

	// ensure value is in bounds
	if (integralTerm > MAX_OUTPUT)
		integralTerm = MAX_OUTPUT;
	else if (integralTerm < MIN_OUTPUT)
		integralTerm = MIN_OUTPUT;

	// by using change in input instead of change in error,
	// we get no output spikes when the set point changes
	float dInput = (input - lastInput) / dt;

	float Pterm = Kp * error;
	float Dterm = -Kd * dInput;
	float output = Pterm + integralTerm + Dterm;

	// ensure output is in bounds
	if (output > MAX_OUTPUT)
		output = MAX_OUTPUT;
	else if (output < MIN_OUTPUT)
		output = MIN_OUTPUT;

	// remember some variables for next iteration
	lastInput = input;
	lastTime = timeNow;

	PIDdata res = { output, Pterm, integralTerm, Dterm };

	return res;
}
