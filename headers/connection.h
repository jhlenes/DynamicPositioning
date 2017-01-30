#ifndef HEADERS_CONNECTION_H_
#define HEADERS_CONNECTION_H_

#include <stdio.h>

int connect_phidgets(void);
int get_sensor_value(void);
int get_servo_min_max(double *min, double *max);
int set_servo_position(double position);
int close_connections(void);

#endif /* HEADERS_CONNECTION_H_ */
