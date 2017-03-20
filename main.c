/**************************************************
 *
 * FILENAME:	main.c
 *
 * DESCRIPTION:
 * 			This is the entry point for the dynamic positioning program. This file
 * 			combines everything. The program runs in a loop, reading sensor data and updating
 * 			the servo motor to counter unwanted changes. It also handles printing values
 * 			to the screen and starts up a new thread which visualizes the boat and handles input.
 *
 * PUBLIC FUNCTIONS:
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 *
 **************************************************/
/*
 * MAYBE: Use more #defines
 *
 * Rapport: 10 sider ++
 * motivasjon
 * teori
 * hva som ble gjort
 * struktur
 * konklusjon
 *
 *
 */
#include <stdio.h>
#include <pthread.h>

#include "headers/my_utils.h"
#include "headers/phidget_connection.h"
#include "headers/animation.h"
#include "headers/pid_controller.h"
#include "headers/main.h"

// Output limits for the PID, based on testing
#define MIN_OUTPUT 84
#define MAX_OUTPUT 94

// Found by testing
#define TANK_WIDTH 280

// Delays determining how often to perform some actions
static const struct timespec LOOP_DELAY = { 0, 10000000L };		// 0.01 seconds
static const struct timespec PRINT_DELAY = { 0, 100000000L };	// 0.1 second

/**************************************************
 * NAME: static void *printer_func(void *void_ptr)
 *
 * DESCRIPTION:
 * 			Prints values to screen and to file in a timed loop.
 * 			Once program finishes, the data from the file is used
 * 			to create a plot of the position against the setpoint.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	void *void_ptr:				A pointer to the boat data from the current run.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void *printer_func(void *void_ptr)
{
	Data *boatData = (Data*) void_ptr;

	// open file
	FILE *fp = fopen("output.dat", "w");
	fprintf(fp, "# This file contains the data from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output", "setpoint");

	while ((*boatData).programRunning)
	{
		nanosleep(&PRINT_DELAY, NULL);

		// Print to screen
		printf("setpoint: %5.1f sensorValue: %5.1f servoValue: %5.f\n", (*boatData).setPoint,
				(*boatData).sensorValue, (*boatData).servoValue);

		// Write to file
		fprintf(fp, " \t%8.3f\t%8.3f\t%8.3f\t%8.3f\n", (*boatData).timePassed,
				(*boatData).sensorValue, (*boatData).servoValue, (*boatData).setPoint);
	}

	// close file
	fclose(fp);

	// plot in gnuplot
	FILE *gnuplot = popen("gnuplot --persist", "w");
	fprintf(gnuplot, "plot \"output.dat\" u 1:2 w lines t \"Posisjon\", "
			"\"output.dat\" u 1:4 w lines t \"Set point\"\n");
	pclose(gnuplot);

	return NULL;
}

/**************************************************
 * NAME: static void test_pid(int argc, char *argv[])
 *
 * DESCRIPTION:
 * 			Function that simulates the boat. Used for testing purposes.
 *
 * INPUTS:
 * 		none
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void test_pid(void)
{

	Data boatData = { 0.0, 0.0, 0.0, 0.0, true };

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, &boatData);

	// Print values to screen in own thread
	pthread_t printThread;
	pthread_create(&printThread, NULL, printer_func, &boatData);

	// Setup PID-controller
	set_pid_coefficients(0.2, 0.15, -0.01);
	boatData.setPoint = 500;
	set_pid_output_limits((float) MIN_OUTPUT, (float) MAX_OUTPUT);

	// time handling
	unsigned long lastTime = nano_time();
	unsigned long startTime = lastTime;

	float value = 800.0;
	while (boatData.programRunning)
	{
		nanosleep(&LOOP_DELAY, NULL); // sleep

		set_pid_setpoint(boatData.setPoint);

		// get exact time since last loop
		unsigned long now = nano_time();
		float dt = now - lastTime;
		dt = FROM_NANOS(dt);
		lastTime = now;

		float output = pid_compute(value);
		value += (output - ((MAX_OUTPUT + MIN_OUTPUT) / 2.0)) * 30.0 * dt;

		float timePassed = FROM_NANOS((float ) (now - startTime));

		boatData.sensorValue = value;
		boatData.servoValue = output;
		boatData.timePassed = timePassed;
	}

	pthread_join(animationThread, NULL);
	pthread_join(printThread, NULL);
}

/**************************************************
 * NAME: int main(int argc, char *argv[])
 *
 * DESCRIPTION:
 * 			The main method. Sets up a connection to phidgets, starts threads for
 * 			animation and printing, and starts the dynamic positioning control loop.
 *
 * INPUTS:
 *     	EXTERNALS:
 *      	MIN_OUPUT:			Minimum output from PID.
 *      	MAX_OUPUT:			Maximum output from PID.
 *      	TANK_WIDTH:			The width of the tank the boat is in.
 *      	programRunning:		The main loop condition
 *      	LOOP_DELAY:			The delay between each loop
 *
 * OUTPUTS:
 *		RETURNS:
 *			int:				0 if successful, 1 if failure
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
int main(int argc, char *argv[])
{

	test_pid();
	return 0;

	if (connect_phidgets())
		return 1;	// Could not connect

	Data boatData = { 0.0, 0.0, 0.0, 0.0, true };

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, &boatData);

	// Print values to screen in own thread
	pthread_t printThread;
	pthread_create(&printThread, NULL, printer_func, &boatData);

	// setup PID-controller {0.4, 0.4, 1.8}
	set_pid_coefficients(0.4, 0.4, 1.8); //

	// initialize setPoint to current location
	boatData.setPoint = (float) get_sensor_value() - TANK_WIDTH / 2.0;

	// set output bounds
	set_pid_output_limits((float) MIN_OUTPUT, (float) MAX_OUTPUT); // TODO: Figure out what values work here

	unsigned long startTime = nano_time();

	// main loop
	while (boatData.programRunning) // while enter is not pressed
	{
		// sleep
		nanosleep(&LOOP_DELAY, NULL);

		set_pid_setpoint(boatData.setPoint);

		// read position
		float sensorValue = (float) get_sensor_value();

		// calculate new servo position
		float output = pid_compute(sensorValue);

		// set the new servo position
		set_servo_position((double) output);

		// write data to file
		float timePassed = FROM_NANOS((float ) (nano_time() - startTime));

		boatData.sensorValue = sensorValue;
		boatData.servoValue = output;
		boatData.timePassed = timePassed;
	}

	set_servo_position(0.0);			// turn off motor
	close_connections();				// close phidget connections

	// Join threads
	pthread_join(animationThread, NULL);
	pthread_join(printThread, NULL);

	return 0;
}

