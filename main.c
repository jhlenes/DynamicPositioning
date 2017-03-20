/*
 *
 * The main program: the main() method combines everything in this project.
 *
 * TODO: Make new thread that handles printing to screen and file
 * Use more #defines
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

#include <stdbool.h>

#include "headers/pid_controller.h"
#include "headers/my_utils.h"
#include "headers/phidget_connection.h"
#include "headers/animation.h"
#include "headers/main.h"

#define MIN_OUTPUT 84
#define MAX_OUTPUT 94
#define TANK_WIDTH 280

static _Bool programRunning = true;

static const struct timespec LOOP_DELAY = { 0, 10000000L };	// 0.01 seconds
static const struct timespec PRINT_DELAY = { 0, 100000000L };	// 0.1 second

void test_pid(int argc, char *argv[]);
void *printer_func(void *void_ptr);

int main(int argc, char *argv[])
{

	test_pid(argc, argv);
	return 0;

	if (connect_phidgets())
		return 1;	// Could not connect

	Data boatData = { 0.0, 0.0, 0.0, 0.0 };

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
	while (programRunning) // while enter is not pressed
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
}

void test_pid(int argc, char *argv[])
{

	Data boatData = { 0.0, 0.0, 0.0, 0.0 };

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
	while (programRunning)
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

void *printer_func(void *void_ptr)
{
	// open file
	FILE *fp = fopen("output.dat", "w");
	fprintf(fp, "# This file contains the data from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output", "setpoint");

	while (programRunning)
	{
		nanosleep(&PRINT_DELAY, NULL);
		Data *boatData = (Data*) void_ptr;

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

void set_program_status(_Bool status)
{
	programRunning = status;
}

