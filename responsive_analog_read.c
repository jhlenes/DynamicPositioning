/**************************************************
 *
 * FILENAME:	responive_analog_read.c
 *
 * DESCRIPTION:
 * 			Implementation of a noise-reduction algorithm. It uses the method of exponential moving average
 * 			to reduce the noise. It's a very simple algorithm that behaves similar to an average of the last
 * 			few input values, but it weights recent input values much more strongly, and the influence of older
 * 			input values gradually moves toward zero. You end up with a smooth elastic kind of behaviour
 * 			as the output drifts toward the current value of the input.
 *
 * 			It also ensures responsiveness by reducing accuracy
 * 			when it is not needed, i.e. when the value changes rapidly.
 *
 * PUBLIC FUNCTIONS:
 * 			int responsive_analog_read(int newValue)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 *
 **************************************************/

#include <stdlib.h>	// Function: abs()

// resolution of values
static const int ANALOG_RESOLUTION = 1000;

// value between 0 and 1 that controls how smooth the output is
static const float SMOOTHNESS_MULTIPLIER = 0.01;

/**************************************************
 * NAME: static float smoothness_cruve(float diff)
 *
 * DESCRIPTION:
 * 			Calculates a number between 0 and 1 that determines how
 * 			aggressively to smooth out noise. High changes in sensor values
 * 			require less smothing, i.e. they need to be responsive.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	float diff:		The difference in last two sensor values.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	float:			A smoothness factor in the range 0 to 1.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static float smoothness_curve(float diff)
{
	float y = 1.0 / (diff + 1.0);
	y = (1.0 - y) * 2.0;
	if (y > 1.0)
		return 1.0;
	return y;
}

/**************************************************
 * NAME: int responsive_analog_read(int newValue)
 *
 * DESCRIPTION:
 * 			Applies the exponential running average algorithm to values and
 * 			return the smoothed out value.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	float newValue:		The new value which will be smoothed.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	int:				The smoothed value.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
int responsive_analog_read(int newValue)
{
	// the output value, it contains the history of the values
	static float smoothValue = 0;

	float diff = abs(newValue - smoothValue);

	float smoothnessFactor = smoothness_curve(diff * SMOOTHNESS_MULTIPLIER);

	// calculate the exponential moving average based on the smoothnessFactor
	smoothValue += (newValue - smoothValue) * smoothnessFactor;

	// ensure output is in bounds
	if (smoothValue < 0.0)
	{
		smoothValue = 0.0;
	} else if (smoothValue > ANALOG_RESOLUTION - 1)
	{
		smoothValue = ANALOG_RESOLUTION - 1;
	}

	return (int) smoothValue;
}

