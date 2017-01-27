#ifndef HEADERS_CONNECTION_H_
#define HEADERS_CONNECTION_H_

#include <phidget21.h>
#include <stdio.h>

extern CPhidgetInterfaceKitHandle kitHandle;
extern CPhidgetServoHandle servoHandle;

int connect_phidgets(void);
int close_connections(void);

#endif /* HEADERS_CONNECTION_H_ */
