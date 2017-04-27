/**************************************************
 * FILENAME:	time_utils.c
 *
 * DESCRIPTION:
 * 		Implementations of different utility functions that are useful.
 *
 * PUBLIC FUNCTIONS:
 * 		float nano_to_sec(unsigned long nanos)
 * 		unsigned long sec_to_nano(float secs)
 * 		unsigned long nano_time(void)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/

#include <sys/time.h>
#include <time.h>

/**************************************************
 * NAME: float nano_to_sec(unsigned long nanos)
 *
 * DESCRIPTION:
 *		Converts from nanoseconds to seconds.
 *
 * INPUTS:
 *		PARAMETERS:
 *			unsigned long nanos:	The time in nanoseconds.
 *
 * OUTPUTS:
 *		RETURN:
 *			float:	The time in seconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
float nano_to_sec(unsigned long nanos)
{
	float secs = nanos / 1000000000.0;
	return secs;
}

/**************************************************
 * NAME: unsigned long sec_to_nano(float secs)
 *
 * DESCRIPTION:
 *		Converts from seconds to nanoseconds.
 *
 * INPUTS:
 *		PARAMETERS:
 *			float secs:		The time in seconds.
 *
 * OUTPUTS:
 *		RETURN:
 *			unsigned long:	The time in nanoseconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
unsigned long sec_to_nano(float secs)
{
	unsigned long nanos = secs * 1000000000UL;
	return nanos;
}

/**************************************************
 * NAME: unsigned long nano_time(void)
 *
 * DESCRIPTION:
 *		Returns the current time in nanoseconds.
 *
 * INPUTS:
 *		none
 *
 * OUTPUTS:
 *		RETURN:
 *			unsigned long:	The current time in nanoseconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
unsigned long nano_time(void)
{
	struct timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (unsigned long) (sec_to_nano(timeSpec.tv_sec) + timeSpec.tv_nsec);
}

