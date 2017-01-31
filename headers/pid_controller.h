#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

int pid_compute(int input);
void set_pid_parameters(float P, float I, float D);
void set_pid_set_point(int value);
void set_pid_output_limits(int min, int max);

#endif /* PID_CONTROLLER_H_ */

