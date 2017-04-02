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
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
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

// Delays determining how often to perform some actions
static const struct timespec LOOP_DELAY = { 0, 20000000L };	// 0.02 seconds
static const struct timespec PRINT_DELAY = { 0, 100000000L };	// 0.1 second

/**************************************************
 * NAME: static void plot(char *filename)
 *
 * DESCRIPTION:
 * 			Plots data in gnuplot. The data consists
 * 			of position, setpoint and power plotted against time.
 *
 * INPUTS:
 * 		char *filename:		The file containing the data to be plotted
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
static void plot(char *filename)
{
	FILE *gnuplot = popen("gnuplot --persist", "w");

	// Configure
	fprintf(gnuplot, "set ytics autofreq nomirror tc lt 1\n"
			"set xlabel 'time [s]'\n"
			"set ylabel 'position' tc lt 1\n"
			"set y2range [0:%f]\n"
			"set y2tics autofreq nomirror tc lt 2\n"
			"set y2label 'power' tc lt 2\n", (MAX_OUTPUT - MIN_OUTPUT) * 1.33);

	// Plot power output, position and setpoint
	fprintf(gnuplot, "plot \"%s\" u 1:3 w lines t \"power\" linetype 2 axes x1y2, "
			"\"%s\" u 1:2 w lines t \"position\" linetype 1, "
			"\"%s\" u 1:4 w lines t \"setpoint\" linetype 1 dashtype 2 \n", filename, filename,
			filename);

	pclose(gnuplot);
	FILE *gnuplot2 = popen("gnuplot --persist", "w");

	// New plot for the PID terms
	fprintf(gnuplot2, "set xlabel 'time [s]\n"
			"plot \"%s\" u 1:3 w lines t \"power\" dashtype 2, "
			"\"%s\" u 1:5 w lines t \"P-term\", "
			"\"%s\" u 1:6 w lines t \"I-term\", "
			"\"%s\" u 1:7 w lines t \"D-term\"\n", filename, filename, filename, filename);

	pclose(gnuplot2);
}
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
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 **************************************************/
static void *printer_func(void *void_ptr)
{
	BoatData *data = (BoatData*) void_ptr;

	FILE *fp = fopen("output.dat", "w");

	// File header
	fprintf(fp, "# This file contains the data from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output", "setpoint",
			"P-term", "I-term", "D-term");

	while ((*data).programRunning)
	{
		nanosleep(&PRINT_DELAY, NULL);

		// Print to screen
		printf("setpoint: %5.1f sensorValue: %5.1f servoValue: %5.1f\n", (*data).setpoint,
				(*data).sensorValue, (*data).servoValue);

		// Write to file
		fprintf(fp, " \t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\n", (*data).timePassed,
				(*data).sensorValue,
				MAX_OUTPUT - (*data).servoValue, (*data).setpoint, (*data).pid.Pterm,
				MAX_OUTPUT - (*data).pid.Iterm, (*data).pid.Dterm);
	}

	fclose(fp);
	return NULL;
}

/**************************************************
 * NAME: static void test_pid(void)
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
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
static void test_pid(void)
{

	BoatData boatData = { .programRunning = true };	// omitted fields are initialized to default values

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, &boatData);

	// Print values to screen in own thread
	pthread_t printThread;
	pthread_create(&printThread, NULL, printer_func, &boatData);

	unsigned long lastTime = nano_time();
	unsigned long startTime = lastTime;

	boatData.setpoint = 500;
	float value = boatData.setpoint + TANK_WIDTH / 2.0;
	boatData.startpoint = value;

	while (boatData.programRunning)
	{
		nanosleep(&LOOP_DELAY, NULL);

		// get exact time since last loop
		unsigned long now = nano_time();
		float dt = from_nanos(now - lastTime);

		lastTime = now;

		PIDdata pid = pid_compute(value, boatData.setpoint);
		value += (pid.output - ((MAX_OUTPUT + MIN_OUTPUT) / 2.0 + 1.0)) * 35.0 * dt;

		float timePassed = from_nanos(now - startTime);

		boatData.sensorValue = value;
		boatData.servoValue = pid.output;
		boatData.timePassed = timePassed;
		boatData.pid = pid;
	}

	pthread_join(animationThread, NULL);
	pthread_join(printThread, NULL);

	plot("output.dat");
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
 *      	LOOP_DELAY:			The delay between each loop
 *
 * OUTPUTS:
 *		RETURNS:
 *			int:				0 if successful, 1 if failure
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
int main()
{
	//plot("../../DPresults/pid.dat");
	plot("output.dat");
	//	test_pid();
	return 0;

	if (connect_phidgets())
		return 1;	// Could not connect

	BoatData boatData = { .programRunning = true };	// omitted fields are initialized to default values

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, &boatData);

	// Print values to screen in own thread
	pthread_t printThread;
	pthread_create(&printThread, NULL, printer_func, &boatData);

	boatData.startpoint = (float) get_sensor_value();

	// Initialize setpoint to middle of the tank
	boatData.setpoint = (float) get_sensor_value() - TANK_WIDTH / 2.0;

	unsigned long startTime = nano_time();

	// main loop
	while (boatData.programRunning)
	{
		// sleep
		nanosleep(&LOOP_DELAY, NULL);

		// read position
		float sensorValue = (float) get_sensor_value();

		// calculate new servo position
		PIDdata pid = pid_compute(sensorValue, boatData.setpoint);

		// set the new servo position
		set_servo_position((double) pid.output);

		float timePassed = from_nanos(nano_time() - startTime);

		// update the data
		boatData.sensorValue = sensorValue;
		boatData.servoValue = pid.output;
		boatData.timePassed = timePassed;
	}

	set_servo_position(0.0);			// turn off motor
	close_connections();				// close phidget connections

	// Join threads
	pthread_join(animationThread, NULL);
	pthread_join(printThread, NULL);

	// plot results
	plot("output.dat");

	return 0;
}

