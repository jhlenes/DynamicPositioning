#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

float pid_compute(float input);
void set_pid_parameters(float P, float I, float D);
void set_pid_set_point(float value);
void set_pid_output_limits(float min, float max);

#endif /* PID_CONTROLLER_H_ */

