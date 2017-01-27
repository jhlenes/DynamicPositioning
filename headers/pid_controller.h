#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

int pid_compute(float setPoint, float measuredValue);
void set_pid_parameters(float P, float I, float D);
void set_output_limits(float min, double max);

unsigned long millis(void);

#endif /* PID_CONTROLLER_H_ */

