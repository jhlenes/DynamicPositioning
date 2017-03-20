#ifndef HEADERS_PHIDGET_CONNECTION_H_
#define HEADERS_PHIDGET_CONNECTION_H_

int connect_phidgets(void);
int get_sensor_value(void);
void set_servo_position(double position);
void close_connections(void);

#endif /* HEADERS_PHIDGET_CONNECTION_H_ */
