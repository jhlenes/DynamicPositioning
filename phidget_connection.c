/**************************************************
 * FILENAME:	phidget_connection.c
 *
 * DESCRIPTION:
 * 		Implementation of several functions required to communicate with phidgets.
 * 		The functions handles creation, reading and setting values, and closing the
 * 		connections. External variables referencing the components are kept to increase
 * 		encapsulation.
 *
 * PUBLIC FUNCTIONS:
 * 		int connect_phidgets(void)
 * 		int get_sensor_value(void)
 * 		void set_servo_position(double)
 * 		void close_connections(void)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/

#include <phidget21.h>
#include <stdio.h>

#include "headers/responsive_analog_read.h"

// IMPORTANT: set this to the id of your sensor on the interface kit
static const int SENSOR_ID = 2;

// handles for identifying the phidgets
static CPhidgetInterfaceKitHandle kitHandle;
static CPhidgetServoHandle servoHandle;

/**************************************************
 * NAME: static int setup_interface_kit_connection(void)
 *
 * DESCRIPTION:
 * 		Creates an interface kit object, opens the device for connections and waits for
 * 		attachment.
 *
 * INPUTS:
 * 		EXTERNALS:
 *      	CPhidgetInterfaceKitHandle kitHandle:	An empty handle which will be attached
 *      											to a device.
 *
 * OUTPUTS:
 *      EXTERNALS:
 *      	CPhidgetInterfaceKitHandle kitHandle:	A handle with a registered phidget.
 *     	RETURNS:
 *        	int:	0 if successful, 1 if failure.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static int setup_interface_kit_connection(void)
{

	// create the InterfaceKit object
	CPhidgetInterfaceKit_create(&kitHandle);

	// open the interfacekit for device connections, -1 opens any serial number available
	CPhidget_open((CPhidgetHandle) kitHandle, -1);

	// get the program to wait for the interface kit device to be attached
	int result;
	const char *err;
	printf("Waiting for interface kit to be attached...\n");
	if ((result = CPhidget_waitForAttachment((CPhidgetHandle) kitHandle, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 1;
	}
	return 0;
}

/**************************************************
 * NAME: static int setup_servo_motor_connection(void)
 *
 * DESCRIPTION:
 * 		Creates a servo object, opens the device for connections and waits for
 * 		attachment.
 *
 * INPUTS:
 *     	EXTERNALS:
 *      	CPhidgetServoHandle servoHandle:	An empty handle which will be attached
 *      										to a device.
 *
 * OUTPUTS:
 *      EXTERNALS:
 *      	CPhidgetServoHandle kitHandle:		A handle with a registered phidget.
 *     	RETURNS:
 *        	int:	0 if successful, 1 if failure.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static int setup_servo_motor_connection(void)
{
	// create the servo object
	CPhidgetServo_create(&servoHandle);

	// open the servoHandle for device connections, -1 opens any serial number available
	CPhidget_open((CPhidgetHandle) servoHandle, -1);

	// get the program to wait for an servoHandle device to be attached
	int result;
	const char *err;
	printf("Waiting for servo to be attached...\n");
	if ((result = CPhidget_waitForAttachment((CPhidgetHandle) servoHandle, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 1;
	}
	return 0;
}

/**************************************************
 * NAME: int connect_phidgets(void)
 *
 * DESCRIPTION:
 * 		Connects to both the interface kit and servo by calling the private
 * 		function setup_interface_kit_connection and setup_servo_motor_connection.
 *
 * INPUTS:
 * 		none
 *
 * OUTPUTS:
 *     	RETURNS:
 *        	int: 0 if successful, 1 if failure.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
int connect_phidgets(void)
{
	// setup a connection to the phidgets we are going to use
	if (setup_interface_kit_connection())
		return 1;
	if (setup_servo_motor_connection())
		return 1;
	return 0;
}

/**************************************************
 * NAME: int get_sensor_value(void)
 *
 * DESCRIPTION:
 * 		Gets the current sensor value from the interface kit.
 *
 * INPUTS:
 * 		EXTERNALS:
 *     		CPhidgetInterfaceKitHandle kitHandle:	A handle with a registered interface kit.
 *
 *
 * OUTPUTS:
 *     	RETURN:
 *        	int:	The sensor value (0-1000).
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
int get_sensor_value(void)
{
	int sensorValue;
	CPhidgetInterfaceKit_getSensorValue(kitHandle, SENSOR_ID, &sensorValue);

	// return noise reduced value
	return responsive_analog_read(sensorValue);
}

/**************************************************
 * NAME: void set_servo_position(double position)
 *
 * DESCRIPTION:
 * 		Sets the position of the servo motor.
 *
 * INPUTS:
 * 		PARAMETERS:
 * 			double position:	The new servo motor position.
 *     	EXTERNALS:
 *     		CPhidgetServoHandle servoHandle:	A handle with a registered servo motor.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void set_servo_position(double position)
{
	CPhidgetServo_setPosition(servoHandle, 0, position);
	return;
}

/**************************************************
 * NAME: void close_connections(void)
 *
 * DESCRIPTION:
 * 		Closes the connection to the phidgets.
 *
 * INPUTS:
 *     	EXTERNALS:
 *     		CPhidgetServoHandle servoHandle:		A handle with a registered servo motor.
 *     		CPhidgetInterfaceKitHandle kitHandle:	A handle with a registered interface kit.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void close_connections(void)
{
	CPhidget_close((CPhidgetHandle) kitHandle);
	CPhidget_delete((CPhidgetHandle) kitHandle);

	CPhidget_close((CPhidgetHandle) servoHandle);
	CPhidget_delete((CPhidgetHandle) servoHandle);

	return;
}

