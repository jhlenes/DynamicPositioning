#ifndef HEADERS_PHIDGET_CONNECTION_H_
#define HEADERS_PHIDGET_CONNECTION_H_

#include <stdio.h>
#include <phidget21.h>

#include "responsive_analog_read.h"

int connect_phidgets(void);
int get_sensor_value(void);
void get_servo_min_max(double *min, double *max);
void set_servo_position(double position);
void get_servo_position(double *position);
void close_connections(void);

#endif /* HEADERS_PHIDGET_CONNECTION_H_ */
