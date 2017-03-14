/*
 *
 * The main program: the main() method combines everything in this project
 *
 */

#include "headers/pid_controller.h"
#include "headers/my_utils.h"
#include "headers/phidget_connection.h"
#include "headers/animation.h"

static const struct timespec LOOP_DELAY = { 0, 1000000L };	// 0.001 seconds
static float setPoint;

void test_pid(int argc, char *argv[]);

void alter_set_point(float value)
{
	setPoint += value;
	set_pid_set_point(setPoint);
}
int main(int argc, char *argv[])
{

	test_pid(argc, argv);
	exit(0);

	if (connect_phidgets())
		return 1;	// Could not connect

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, NULL);

	// open output file
	FILE *fp = fopen("output.dat", "w");
	fprintf(fp, "# This file contains the data from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output");

	// setup PID-controller
	set_pid_parameters(-0.5, -0.2, -0.05); // Funker greit: {-0.3, -0.2, -0.05}

	// initialize setPoint to current location
	setPoint = (float) get_sensor_value();
	set_pid_set_point(setPoint);

	// set output bounds
	//double min, max;
	//get_servo_min_max(&min, &max); 	// These values are not worth anything
	set_pid_output_limits(40.0, 115.0);	// TODO: Figure out what values work here

	unsigned long startTime = nano_time();

	// main loop
	while (!kbhit()) // while enter is not pressed
	{
		// sleep
		nanosleep(&LOOP_DELAY, NULL);

		// read position
		float sensorValue = (float) get_sensor_value();

		// calculate new servo position
		float output = pid_compute(sensorValue);

		// set the new servo position
		set_servo_position((double) output);

		// write data to file
		float timePassed = FROM_NANOS((float ) (nano_time() - startTime));
		fprintf(fp, " \t%8f\t%8f\t%8f\n", timePassed, sensorValue, output);

		AnimationData data = { output, sensorValue, setPoint };
		update_animation_data(data);

		// Print to screen
		printf("setpoint: %5.1f sensorValue: %5.1f output: %5.1f\n", setPoint, sensorValue, output);
	}

	set_servo_position(0.0);

	fclose(fp);
	close_connections();

	// plot results in gnuplot
	FILE *gnuplot = popen("gnuplot -persistent", "w");
	fprintf(gnuplot, "plot \"output.dat\" u 1:2 w lines t \"Posisjon\"\n");
	pclose(gnuplot);
}

void test_pid(int argc, char *argv[])
{

	// open file
	FILE *fp = fopen("output.dat", "w");
	fprintf(fp, "# This file contains the data from running the dynamic positioning program.\n"
			"#\t%8s\t%8s\t%8s\n", "time[s]", "sensor", "output");

	// Start animation
	pthread_t animationThread;
	pthread_create(&animationThread, NULL, start_animation, NULL);

	// Setup PID-controller
	set_pid_parameters(10.0, 5.0, 1.0);
	setPoint = 697;
	set_pid_set_point(setPoint);
	set_pid_output_limits(-100.0, 100.0);

	// time handling
	unsigned long lastTime = nano_time();
	unsigned long startTime = lastTime;

	float value = 0.0;
	while (!kbhit())
	{
		nanosleep(&LOOP_DELAY, NULL); // sleep

		// get exact time since last loop
		unsigned long now = nano_time();
		float dt = now - lastTime;
		dt = FROM_NANOS(dt);
		lastTime = now;

		float output = pid_compute(value);
		value += output * dt;

		// Write to file
		float timePassed = FROM_NANOS((float ) (now - startTime));
		fprintf(fp, " \t%8f\t%8f\t%8f\n", timePassed, value, output);

		AnimationData data = { output, value, setPoint };
		update_animation_data(data);

		// Print to screen
		printf("setpoint: %5.1f sensorValue: %5.1f output: %5.1f\n", setPoint, value, output);
	}

	printf("Finished, user hit enter!\n");
	fclose(fp);

	// gnuplot
	FILE *gnuplot = popen("gnuplot --persist", "w");
	fprintf(gnuplot, "plot \"output.dat\" u 1:2 w lines t \"Posisjon\""
			", %f title \"Set point\"\n", setPoint);
	pclose(gnuplot);

}

