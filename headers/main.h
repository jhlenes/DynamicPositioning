/*
 * main.h
 *
 *  Created on: Mar 20, 2017
 *      Author: henrik
 */

#ifndef HEADERS_MAIN_H_
#define HEADERS_MAIN_H_

void set_program_status(_Bool status);

typedef struct
{
	float servoValue;
	float sensorValue;
	float setPoint;
	float timePassed;
} Data;

#endif /* HEADERS_MAIN_H_ */
