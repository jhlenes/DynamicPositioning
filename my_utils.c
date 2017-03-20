/**************************************************
 *
 * FILENAME:	my_utils.c
 *
 * DESCRIPTION:
 * 			Implementations of different utility functions that are useful.
 *
 * PUBLIC FUNCTIONS:
 * 			unsigned long nano_time(void)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 *
 **************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "headers/my_utils.h"

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
	return (unsigned long) (TO_NANOS(timeSpec.tv_sec) + timeSpec.tv_nsec);
}

