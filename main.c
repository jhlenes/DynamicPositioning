/**************************************************
 * FILENAME:	main.c
 *
 * DESCRIPTION:
 *		This is the entry point for the dynamic positioning program. This file
 * 		combines everything. The program runs in a loop, reading sensor data and
 * 		updating the servo motor to counter unwanted changes. It also handles
 * 		printing values to the screen and starts up a new thread which visualizes
 * 		the boat and handles input.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 **************************************************/

#include <pthread.h>
#include <stdio.h>
#include <time.h>

#include "headers/main.h"
#include "headers/phidget_connection.h"
#include "headers/time_utils.h"
#include "headers/visualization.h"

// Constants used for setting the delays
static const struct timespec LOOP_DELAY = { 0, 20000000L };		// 0.02 seconds
static const struct timespec PRINT_DELAY = { 0, 100000000L };	// 0.1 second

/**************************************************
 * NAME: static void plot(char *filename)
 *
 * DESCRIPTION:
 * 		Uses the data from 'filename' to create two plots in gnuplot. The first plot shows
 * 		position, setpoint and power over time. The second plot show the power and the PID
 * 		terms over time.
 *
 * INPUTS:
 * 		char *filename:	The file containing the data to be plotted.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
static void plot(char *filename)
{
	// open pipe to gnuplot
	FILE *gnuplot = popen("gnuplot --persist", "w");

	// configure labels and axis
	fprintf(gnuplot, "set ytics autofreq nomirror tc lt 1\n"
			"set xlabel 'time [s]'\n"
			"set ylabel 'position' tc lt 1\n"
			"set y2range [0:%f]\n"
			"set y2tics autofreq nomirror tc lt 2\n"
			"set y2label 'power' tc lt 2\n", (MAX_OUTPUT - MIN_OUTPUT) * 1.33);

	// plot power output, position and setpoint
	fprintf(gnuplot, "plot \"%s\" u 1:3 w lines t \"power\" linetype 2 axes x1y2, "
			"\"%s\" u 1:2 w lines t \"position\" linetype 1, "
			"\"%s\" u 1:4 w lines t \"setpoint\" linetype 1 dashtype 2 \n",
			filename, filename, filename);
	pclose(gnuplot);

	// open new pipe to gnuplot
	FILE *gnuplot2 = popen("gnuplot --persist", "w");

	// plot the terms of the PID and power output
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
 * 		Prints values to screen and writes them to file in a timed loop. This function
 * 		is run in a separate thread, hence the pointer in the function name and the
 * 		void pointer parameter. This is a format enforced by the thread.
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	void *void_ptr:	A pointer to a 'BoatData' struct containing
 *      					always updated values from the current run.
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

	// create file header
	fprintf(fp, "# Data gathered from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output",
			"setpoint", "P-term", "I-term", "D-term");

	// continue to print to screen and write to file as long as the program is running
	while ((*data).programRunning)
	{
		nanosleep(&PRINT_DELAY, NULL);

		// print to screen
		printf("setpoint: %5.1f sensorValue: %5.1f servoValue: %5.1f\n",
				1000.0 - (*data).setpoint, 1000.0 - (*data).sensorValue, (*data).servoValue);

		// write to file
		fprintf(fp, " \t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\t%8.3f\n",
				(*data).timePassed, 1000.0 - (*data).sensorValue,
				MAX_OUTPUT - (*data).servoValue, 1000.0 - (*data).setpoint, -(*data).pid.Pterm,
				MAX_OUTPUT - (*data).pid.Iterm, -(*data).pid.Dterm);
	}

	fclose(fp);
	return NULL;
}

/**************************************************
 * NAME: int main()
 *
 * DESCRIPTION:
 * 		The main method. Sets up a connection to phidgets, starts threads for
 * 		visualization and printing, and starts the dynamic positioning control loop.
 * 		Upon exit it plots the recorded data.
 *
 * INPUTS:
 *     	none
 *
 * OUTPUTS:
 *		RETURNS:
 *			int:	0 if successful, 1 if failure.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
int main()
{
	if (connect_phidgets())
		return 1;	// could not connect

	// omitted fields are initialized to default values
	BoatData boatData = { .programRunning = true };

	// start thread for visualization
	pthread_t visualizationThread;
	pthread_create(&visualizationThread, NULL, start_animation, &boatData);

	// start thread for printing and recording data
	pthread_t printerThread;
	pthread_create(&printerThread, NULL, printer_func, &boatData);

	boatData.startpoint = get_sensor_value();

	// initialize setpoint to middle of the tank
	boatData.setpoint = get_sensor_value() - TANK_WIDTH / 2;

	// main loop
	unsigned long startTime = nano_time();
	while (boatData.programRunning)
	{
		nanosleep(&LOOP_DELAY, NULL);

		// read position
		float sensorValue = get_sensor_value();

		// calculate new servo value
		PIDdata pid = pid_compute(sensorValue, boatData.setpoint);

		// set the new servo value
		set_servo_position((double) pid.output);

		float timePassed = nano_to_sec(nano_time() - startTime);

		// update the data
		boatData.sensorValue = sensorValue;
		boatData.servoValue = pid.output;
		boatData.timePassed = timePassed;
		boatData.pid = pid;
	}

	set_servo_position(0.0);	// turn off motor
	close_connections();    	// close phidget connections

	// join threads
	pthread_join(visualizationThread, NULL);
	pthread_join(printerThread, NULL);

	// plot results
	plot("output.dat");

	return 0;
}

