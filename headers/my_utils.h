#ifndef HEADERS_MY_UTILS_H_
#define HEADERS_MY_UTILS_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// convert between seconds and nanoseconds
#define FROM_NANOS(A) A/1000000000
#define TO_NANOS(A)   A*1000000000

int kbhit(void);
unsigned long nano_time(void);

#endif /* HEADERS_MY_UTILS_H_ */
