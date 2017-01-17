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
int CCONV AttachHandler(CPhidgetHandle handle, void *userPtr) {
	int serialNo;
	const char *deviceName;

	CPhidget_getDeviceName(handle, &deviceName);
	CPhidget_getSerialNumber(handle, &serialNo);

	printf("%s %10d attached!\n", deviceName, serialNo);

	return 0;
}

/*
 * Function:  DetachedHandler
 * --------------------
 * Is called whenever a phidget is detached. Prints name and serial number to the console.
 *
 *  handle: a reference to the device
 *  userPtr:
 *
 *  returns: 0
 */
int CCONV DetachHandler(CPhidgetHandle handle, void *userPtr) {
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
int CCONV ErrorHandler(CPhidgetHandle handle, void *userPtr, int ErrorCode, const char *unknown) {
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
int CCONV SensorChangeHandler(CPhidgetHandle handle, void *userPtr, int index, int sensorValue) {
	// TODO: Do regulation

	printf("New value %d from sensor at location %d\n", sensorValue, index);

	return 0;
}

int main() {

	// Declare an InterfaceKit handle
	CPhidgetInterfaceKitHandle kitHandle = 0;

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
	printf("Waiting for interface kit to be attached....");
	if ((result = CPhidget_waitForAttachment((CPhidgetHandle) kitHandle, 10000))) {
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}

	// Read interface kit event data
	printf("Reading...\n");

	// Keep displaying interface kit data until user input is read
	printf("Press any key to go to exit\n");
	getchar();

	// Since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle) kitHandle);
	CPhidget_delete((CPhidgetHandle) kitHandle);

	// All done, exit
	return 0;

}

