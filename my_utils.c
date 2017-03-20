/**************************************************
 *
 * FILENAME:	my_utils.c
 *
 * DESCRIPTION:
 * 			Implementations of different utility functions that are useful.
 *
 * PUBLIC FUNCTIONS:
 * 			float from_nanos(unsigned long nanos)
 * 			unsigned long to_nanos(float secs)
 * 			unsigned long nano_time(void)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 *
 **************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/**************************************************
 * NAME: float from_nanos(unsigned long nanos)
 *
 * DESCRIPTION:
 * 			Converts from nanoseconds to seconds.
 *
 * INPUTS:
 * 		PARAMETERS:
 * 			unsigned long nanos:	The time in nanoseconds.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	float:					The time in seconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
float from_nanos(unsigned long nanos)
{
	float secs = nanos / 1000000000.0;
	return secs;
}

/**************************************************
 * NAME: unsigned long to_nanos(float secs)
 *
 * DESCRIPTION:
 * 			Converts from seconds to nanoseconds.
 *
 * INPUTS:
 * 		PARAMETERS:
 * 			float secs:		The time in seconds.
 *
 * OUTPUTS:
 *     	RETURN:
 *        	unsigned long:	The time in nanoseconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
unsigned long to_nanos(float secs)
{
	unsigned long nanos = secs * 1000000000UL;
	return nanos;
}

/**************************************************
 * NAME: unsigned long nano_time(void)
 *
 * DESCRIPTION:
 * 			Returns the current time in nanoseconds.
 *
 * INPUTS:
 * 		none
 *
 * OUTPUTS:
 *     	RETURN:
 *        	unsigned long:	The current time in nanoseconds.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
unsigned long nano_time(void)
{
	struct timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (unsigned long) (to_nanos(timeSpec.tv_sec) + timeSpec.tv_nsec);
}

