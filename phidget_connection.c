/*
 *
 *	Implements functions for connecting to phidgets,
 *	and closing existing connections.
 *
 */

#include <stdio.h>
#include <phidget21.h>
#include "headers/phidget_connection.h"
#include "headers/responsive_analog_read.h"

static CPhidgetInterfaceKitHandle kitHandle;	// Declare an InterfaceKit handle
static CPhidgetServoHandle servoHandle;			// Declare a servo handle

/*
 * Is called whenever a phidget is attached. Prints name and serial number to the console.
 *
 *  handle: a reference to the device
 *  userPtr:
 *
 *  returns: 0
 */
int CCONV AttachHandler(CPhidgetHandle handle, void *userPtr)
{
	int serialNo;
	const char *deviceName;

	CPhidget_getDeviceName(handle, &deviceName);
	CPhidget_getSerialNumber(handle, &serialNo);

	printf("%s %10d attached!\n", deviceName, serialNo);

	return 0;
}

/*
 * Is called whenever a phidget is detached. Prints name and serial number to the console.
 *
 *  handle: a reference to the device
 *  userPtr:
 *
 *  returns: 0
 */
int CCONV DetachHandler(CPhidgetHandle handle, void *userPtr)
{
	int serialNo;
	const char *deviceName;

	CPhidget_getDeviceName(handle, &deviceName);
	CPhidget_getSerialNumber(handle, &serialNo);

	printf("%s %10d detached!\n", deviceName, serialNo);

	return 0;
}

/*
 * Is called whenever a phidget generates an error. Prints name and serial number to the console.
 *
 *  handle: a reference to the device
 *  userPtr:
 *
 *  returns: 0
 */
int CCONV ErrorHandler(CPhidgetHandle handle, void *userPtr, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s", ErrorCode, unknown);
	return 0;
}

/*
 *	Create an interface kit object, set up event handlers and open the phidget.
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int setup_interface_kit_connection()
{

	// Create the InterfaceKit object
	CPhidgetInterfaceKit_create(&kitHandle);

	// Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software,
	// or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) kitHandle, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) kitHandle, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle) kitHandle, ErrorHandler, NULL);

	// Open the interfacekit for device connections, -1 opens any.
	CPhidget_open((CPhidgetHandle) kitHandle, -1);

	// Get the program to wait for the interface kit device to be attached
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

/*
 *	Create an servo motor object, set up event handlers and open the phidget.
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int setup_servo_motor_connection()
{
	// Create the servo object
	CPhidgetServo_create(&servoHandle);

	// Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software,
	// or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) servoHandle, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) servoHandle, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle) servoHandle, ErrorHandler, NULL);

	// Open the servoHandle for device connections, -1 opens any
	CPhidget_open((CPhidgetHandle) servoHandle, -1);

	// Get the program to wait for an servoHandle device to be attached
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

/*
 *	Sets up the interface kit and servo motor
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int connect_phidgets(void)
{
	// Setup a connection to the phidgets we are going to use
	if (setup_interface_kit_connection())
		return 1;
	if (setup_servo_motor_connection())
		return 1;
	return 0;
}

/*
 * Gets the sensor value from the interface kit
 *
 * 	returns:	the sensor value (0-1000)
 */
int get_sensor_value(void)
{
	int sensorValue;
	CPhidgetInterfaceKit_getSensorValue(kitHandle, 0, &sensorValue);

	// reduce noise
	sensorValue = responsive_analog_read(sensorValue);
	return sensorValue;
}

/*
 * Gets the minimum position and maximum position of the servo
 */
void get_servo_min_max(double *min, double *max)
{
	CPhidgetServo_getPositionMin(servoHandle, 0, min);
	CPhidgetServo_getPositionMax(servoHandle, 0, max);
	return;
}

/*
 * Sets the servo position
 */
void set_servo_position(double position)
{
	CPhidgetServo_setPosition(servoHandle, 0, position);
	return;
}

/*
 * Closes the connections to the phidgets
 */
void close_connections(void)
{
	CPhidget_close((CPhidgetHandle) kitHandle);
	CPhidget_delete((CPhidgetHandle) kitHandle);

	CPhidget_close((CPhidgetHandle) servoHandle);
	CPhidget_delete((CPhidgetHandle) servoHandle);

	return;
}

