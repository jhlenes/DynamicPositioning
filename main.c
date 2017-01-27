
#include <stdlib.h>
#include "headers/pid_controller.h"
#include "headers/responsive_analog_read.h"
#include "headers/connection.h"

void test_pid(void);

CPhidgetInterfaceKitHandle kitHandle;	// Declare an InterfaceKit handle
CPhidgetServoHandle servoHandle;		// Declare a servo handle

int main(void)
{
	//connect_phidgets();
	//close_connections();

	test_pid();
}

void test_pid(void)
{
	// Setup PID-controller
	set_pid_parameters(3.0, 2.0, 1.0);
	set_output_limits(-20, 20);

	unsigned long lastTime = millis();

	int setPoint = 115;
	float value = 47;
	while (1 == 1)
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
}
