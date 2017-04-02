#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

#define MIN_OUTPUT 101.0
#define MAX_OUTPUT 107.0

float pid_compute(float input, float setpoint);

#endif /* PID_CONTROLLER_H_ */

