/*
 * connection.c
 * --------------------
 * Establishes a connection to phidgets and adds event handlers.
 *
 */

#include <stdio.h>
#include <phidget21.h>

#define SENSOR_INDEX 1
#define CHANGE_TRIGGER 1

CPhidgetInterfaceKitHandle kitHandle = 0;	// Declare an InterfaceKit handle
CPhidgetServoHandle servoHandle = 0;		// Declare a servo handle
CPhidgetMotorControlHandle motorHandle = 0;	// Declare a motor control handle

/*
 * Function:  AttachHandler
 * --------------------
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
 * Function:  DetachHandler
 * --------------------
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
 * Function:  ErrorHandler
 * --------------------
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
 * Function:  SensorChangeHandler
 * --------------------
 * Is called whenever a sensor value changes more than the change trigger.
 *
 *  handle: a reference to the device
 *  userPtr:
 *  index: the sensor index
 *  sensorValue: a sensor value 0-1000
 *
 *  returns: 0
 */
int CCONV SensorChangeHandler(CPhidgetHandle handle, void *userPtr, int index, int sensorValue)
{
	// TODO: Do regulation

	printf("New value %d from sensor at location %d\n", sensorValue, index);

	return 0;
}

/*
 * Function:  setupInterfaceKitHandle
 * --------------------
 *	Create an interface kit object, set up event handlers and open the phidget.
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int setupInterfaceKitConnection()
{
	// Create the InterfaceKit object
	CPhidgetInterfaceKit_create(&kitHandle);

	// Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) kitHandle, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) kitHandle, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle) kitHandle, ErrorHandler, NULL);

	// Registers a callback that will run if the sensor value changes by more than the sensor change trigger.
	CPhidgetInterfaceKit_set_OnSensorChange_Handler(kitHandle, SensorChangeHandler, NULL);

	// Set the sensor change trigger
	CPhidgetInterfaceKit_setSensorChangeTrigger(kitHandle, SENSOR_INDEX, CHANGE_TRIGGER);

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
 * Function:  setupServoMotorHandle
 * --------------------
 *	Create an servo motor object, set up event handlers and open the phidget.
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int setupServoMotorConnection()
{
	// Create the servo object
	CPhidgetServo_create(&servoHandle);

	// Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) servoHandle, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) servoHandle, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle) servoHandle, ErrorHandler, NULL);

	/*
	 //Registers a callback that will run when the motor position is changed.
	 //Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	 CPhidgetServo_set_OnPositionChange_Handler(servoHandle, PositionChangeHandler, NULL);
	 */

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
 * Function:  setupMotorControlHandle
 * --------------------
 *	Create an motor control object, set up event handlers and open the phidget.
 *
 *  returns: 	0 if successful
 *  			1 if failed
 */
int setupMotorControlConnection()
{

	// Create the motor control object
	CPhidgetMotorControl_create(&motorHandle);

	// Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle) motorHandle, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle) motorHandle, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle) motorHandle, ErrorHandler, NULL);

	// Open the motorHandle for device connections, -1 opens any
	CPhidget_open((CPhidgetHandle) motorHandle, -1);

	// Get the program to wait for an motorHandle device to be attached
	int result;
	const char *err;
	printf("Waiting for motor control to be attached...\n");
	if ((result = CPhidget_waitForAttachment((CPhidgetHandle) motorHandle, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 1;
	}
	return 0;
}

int closeConnections()
{
	CPhidget_close((CPhidgetHandle) kitHandle);
	CPhidget_delete((CPhidgetHandle) kitHandle);

	CPhidget_close((CPhidgetHandle) servoHandle);
	CPhidget_delete((CPhidgetHandle) servoHandle);

	CPhidget_close((CPhidgetHandle) motorHandle);
	CPhidget_delete((CPhidgetHandle) motorHandle);

	return 0;
}

int main()
{
	// Setup a connection to the phidgets we are going to use
	if (setupInterfaceKitConnection())
		return 1;
	if (setupServoMotorConnection())
		return 1;
	if (setupMotorControlConnection())
		return 1;

	// Read interface kit event data
	printf("Reading...\n");

	// Keep displaying interface kit data until user input is read
	printf("Press any key to go to exit\n");
	getchar();

	// Since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	closeConnections();

	// All done, exit
	return 0;
}

