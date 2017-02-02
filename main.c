/*
 *
 * The main program: the main() method combines everything in this project
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "headers/pid_controller.h"
#include "headers/my_utils.h"
#include "headers/phidget_connection.h"

void test_pid(void);

int main(void)
{

	/*
	if (connect_phidgets())
		return 1;	// Could not connect

	// initialize setPoint to current location
	float setPoint = (float) get_sensor_value();
	set_pid_set_point(setPoint);

	// setup PID-controller
	set_pid_parameters(3.0, 2.0, 1.0);

	double min, max;
	get_servo_min_max(&min, &max);
	set_pid_output_limits((float) min, (float) max);

	// main loop
	while (!kbhit()) // while enter is not pressed
	{
		// read position
		float sensorValue = (float) get_sensor_value();

		// calculate new servo position
		float output = pid_compute(sensorValue);

		// set the new servo position
		set_servo_position((double) output);
	}

	close_connections();
	*/
	test_pid();

}

void test_pid(void)
{
	// Setup PID-controller
	set_pid_parameters(3.0, 2.0, 1.0);
	float setPoint = 531.0;
	set_pid_set_point(setPoint);
	set_pid_output_limits(-100.0, 100.0);

	unsigned long lastTime = nano_time();

	float value = 47.0;
	while (!kbhit())
	{
		unsigned long now = nano_time();
		float dt = now - lastTime;
		dt = FROM_NANOS(dt);
		lastTime = now;

		float output = pid_compute(value);
		value += output * dt;
		printf("Set point: %3.0f \t Value: %4.1f\n", setPoint, value);

	}
	printf("User hit enter!");
}
