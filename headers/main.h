#ifndef HEADERS_MAIN_H_
#define HEADERS_MAIN_H_

#include <stdbool.h>

#define MIN_OUTPUT 101
#define MAX_OUTPUT 107

typedef struct
{
	float servoValue;
	float sensorValue;
	float setPoint;
	float timePassed;
	_Bool programRunning;
} Data;

#endif /* HEADERS_MAIN_H_ */
