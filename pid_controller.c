/**************************************************
 *
 * FILENAME:	pid_controller.c
 *
 * DESCRIPTION:
 * 			Implementation of a PID-controller. You must call the functions
 * 			set_pid_parameters, set_pid_set_point and set_pid_output_limits before
 * 			using the pid_compute function.
 *
 * PUBLIC FUNCTIONS:
 * 			float pid_compute(float input)
 * 			void set_pid_coefficients(float P, float I, float D)
 * 			void set_pid_setpoint(float value)
 * 			void set_pid_ouput_limits(float min, float max)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 *
 **************************************************/

#include <stdio.h>
#include "headers/my_utils.h"

// PID coefficients
static float Kp, Ki, Kd;

static float setpoint;

// Restrictions on the output
static float outMin, outMax;

/**************************************************
 * NAME: float pid_compute(float input)
 *
 * DESCRIPTION:
 * 			Applies the PID-regulator control loop algorithm.
 * 			See https://en.wikipedia.org/wiki/PID_controller for more info about PID-controllers.
 *
 * INPUTS:
 * 		PARAMETERS:
*      		float input:			The input value to regulate.
 *     	EXTERNALS:
 *      	float Kp, Ki, Kd:		The PID coefficients.
 *			float setpoint:			The setpoint to follow.
 *			float outMin, outMax:	Ouput bounds for regulator.
 *
 * OUTPUTS:
 * 		PARAMETERS:
 *      	none
 *     	RETURN:
 *        	float:					The controller output to be applied in order to regulate the process.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
float pid_compute(float input)
{
	// static variables are initialized only once
	static unsigned long lastTime = 0;
	static float lastInput = 0;
	static float integralTerm = 0.0;

	if (lastTime == 0)
		lastTime = nano_time();

	unsigned long timeNow = nano_time();

	// time difference
	float dt = (float) (timeNow - lastTime);
	dt = FROM_NANOS(dt);	// Convert to seconds

	// Calculate the terms
	float error = setpoint - input;

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

	return output;
}

/**************************************************
 * NAME: void set_pid_coefficients(float P, float I, float D)
 *
 * DESCRIPTION:
 * 			Sets the PID-controller coefficients. Needs to be set before running
 * 			pid_compute.
 *
 * INPUTS:
 * 		PARAMETERS:
*      		float P, I, D:			The new PID coefficients.
 *     	EXTERNALS:
 *      	float Kp, Ki, Kd:		The actual PID coefficients.
 *
 * OUTPUTS:
 * 		PARAMETERS:
 *      	none
 *      EXTERNALS:
 *      	float Kp, Ki, Kd:		The updated PID coefficients
 *     	RETURN:
 *        	none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void set_pid_coefficients(float P, float I, float D)
{
	Kp = P;
	Ki = I;
	Kd = D;
}

/**************************************************
 * NAME: void set_pid_setpoint(float value)
 *
 * DESCRIPTION:
 * 			Updates the setpoint which the regulator regulates after.
 * 			Needs to be set before running pid_compute.
 *
 * INPUTS:
 * 		PARAMETERS:
*      		float value:			The new setpoint.
 *     	EXTERNALS:
 *      	float setpoint:			The actual setpoint.
 *
 * OUTPUTS:
 * 		PARAMETERS:
 *      	none
 *      EXTERNALS:
 *      	float setpoint:			The updated setpoint.
 *     	RETURN:
 *        	none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void set_pid_setpoint(float value)
{
	setpoint = value;
}

/**************************************************
 * NAME: void set_pid_output_limits(float min, float max)
 *
 * DESCRIPTION:
 * 			Sets the ouput limits which the regulator stays inside.
 * 			Needs to be set before running pid_compute.
 *
 * INPUTS:
 * 		PARAMETERS:
*      		float min, max:			The new output limits.
 *     	EXTERNALS:
 *      	float outMin, outMax:	The actual ouput limits.
 *
 * OUTPUTS:
 * 		PARAMETERS:
 *      	none
 *      EXTERNALS:
 *      	float outMin, outMax:	The updated ouput limits.
 *     	RETURN:
 *        	none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void set_pid_output_limits(float min, float max)
{
	if (min > max)
		return;
	outMin = min;
	outMax = max;
}

