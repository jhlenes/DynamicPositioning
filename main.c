/*
 *
 * The main program: the main() method combines everything in this project
 *
 */

#include <stdlib.h>
#include "headers/pid_controller.h"
#include "headers/my_utils.h"
#include "headers/phidget_connection.h"

void test_pid(void);

int main(void)
{

	if (connect_phidgets())
		return 1;	// Could not connect

	// Initialize setPoint to current location
	float setPoint = get_sensor_value();

	// setup PID-controller
	set_pid_parameters(3.0, 2.0, 1.0);

	double min, max;
	get_servo_min_max(&min, &max);
	set_pid_output_limits((float) min, (float) max);

	// main loop
	while (!kbhit()) // while enter is not pressed
	{
		// read position
		int sensorValue = get_sensor_value();

		// calculate new servo position
		int output = pid_compute(setPoint, sensorValue);

		// set the new servo position
		set_servo_position((double) output);
	}

	close_connections();

	//test_pid();

}

void test_pid(void)
{
	// Setup PID-controller
	set_pid_parameters(3.0, 2.0, 1.0);
	set_pid_output_limits(-100, 100);

	unsigned long lastTime = millis();

	int setPoint = 531;
	float value = 47;
	while (!kbhit())
	{
		unsigned long now = millis();
		float dt = now - lastTime;
		dt /= 1000.0;
		lastTime = now;

		int output = pid_compute(setPoint, value);
		value += ((float) output) * dt;
		printf("Set point: %3d \t Value: %4i\n", setPoint, (int) value);
		if (abs((int) value) > 1000)
			break;
	}
	printf("User hit enter!");
}
