/*
 *
 * Implementation of a noise-reduction algorithm. It uses the exponential moving average
 * to reduce the noise. It also ensures responsiveness by reducing accuracy
 * when it is not needed, i.e. when the value changes rapidly.
 *
 */

#include <stdlib.h>	// Function: abs()

const int ANALOG_RESOLUTION = 1024;

// value between 0 and 1 that controls how smooth the output is
const float SNAP_MULTIPLIER = 0.007;

// the output value
float smoothValue = 0;

/*
 * Calculates a number between 0 and 1 that determines how
 * aggressively to smooth out noise. A high x results in little smoothing.
 */
float snap_curve(float x)
{
	float y = 1.0 / (x + 1.0);
	y = (1.0 - y) * 2.0;
	if (y > 1.0)
		return 1.0;
	return y;
}

/*
 * Returns the smoothed value based on values received.
 */
int responsive_analog_read(int newValue)
{

	// get difference between new input value and current smooth value
	unsigned int diff = abs(newValue - smoothValue);

	// multiply the input by SNAP_MULTIPLER so input values fit the snap curve better.
	float snap = snap_curve(diff * SNAP_MULTIPLIER);

	// calculate the exponential moving average based on the snap
	smoothValue += (newValue - smoothValue) * snap;

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

