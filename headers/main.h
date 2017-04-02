#ifndef HEADERS_MAIN_H_
#define HEADERS_MAIN_H_

#include <stdbool.h>

#define TANK_WIDTH 280.0

typedef struct
{
	float servoValue;
	float sensorValue;
	float setpoint;
	float timePassed;
	float startpoint;
	_Bool programRunning;
} BoatData;

#endif /* HEADERS_MAIN_H_ */
