/*
 *
 * Implementations of different utility functions that are useful in this project
 *
 */

#include "headers/my_utils.h"

/*
 * Performs non-blocking checking on the standard input.
 * Use like this:
 *
 * 	while(!kbhit())
 *	{
 *   	// do certain operation..
 *	}
 *	// user hits enter.
 *
 */
int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(0, &fds);
	select(1, &fds, NULL, NULL, &tv);
	return FD_ISSET(0, &fds);
}

/*
 * Returns current time in nanoseconds
 */
unsigned long nano_time(void)
{
	struct timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (unsigned long) (TO_NANOS(timeSpec.tv_sec) + timeSpec.tv_nsec);
}

