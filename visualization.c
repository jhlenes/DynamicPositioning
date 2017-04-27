/**************************************************
 * FILENAME:	animation.c
 *
 * DESCRIPTION:
 * 		This file contains everything related to visualization. The graphics
 * 		are made in OpenGL. This file also handles keyboard events.
 *
 * PUBLIC FUNCTIONS:
 * 			void *start_animation(void*)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 03.04.2017
 **************************************************/

#include <GL/freeglut.h>
#include <stdlib.h>

#include "headers/main.h"
#include "headers/obj_loader.h"
#include "headers/pid_controller.h"

// constants used for drawing
#define WINDOW_WIDTH 10.0
#define BOAT_WIDTH 3.5
#define SETLINE_WIDTH 0.02
#define SETLINE_HEIGHT 1.4

#define KEY_ENTER 13

#define SETPOINT_INCREMENT 3

/* Functions in OpenGL are predefined to a specific format.
 * External variables are therefore necessary. */
static BoatData *boatData;	// data from control loop
static GLuint speedboat;  	// display list ID for boat
static GLuint setline;    	// display list ID for setline

/**************************************************
 * NAME: static void drawPowerArrow(void)
 *
 * DESCRIPTION:
 * 		Draws an arrow depicting the power output from the boat.
 *
 * INPUTS:
 * 		EXTERNALS:
 * 			BoatData *boatData:		A collection of data for the boat
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
static void drawPowerArrow(void)
{
	// calculate position and color of arrow based on the power
	float arrowX = ((*boatData).servoValue - MAX_OUTPUT) / ( MIN_OUTPUT - MAX_OUTPUT)
			* 4.0 - 2.0;
	float arrowColor = 0.9
			* (1 - ((*boatData).servoValue - MAX_OUTPUT) / (MIN_OUTPUT - MAX_OUTPUT));

	glLoadIdentity();
	glTranslatef(-0.5, 0.0, 0.0);
	glColor3f(1.0, arrowColor, arrowColor * 0.6);	// simple color mapping
	glBegin(GL_QUADS);		// main body of arrow
	glVertex3f(-2.0, 1.0, 0.0);
	glVertex3f(-2.0, 0.0, 0.0);
	glVertex3f(arrowX, 0.0, 0.0);
	glVertex3f(arrowX, 1.0, 0.0);
	glEnd();
	glBegin(GL_TRIANGLES);	// arrowhead
	glVertex3f(arrowX, -0.5, 0.0);
	glVertex3f(arrowX + 1.0, 0.5, 0.0);
	glVertex3f(arrowX, 1.5, 0.0);
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);	// surrounding rectangle
	glVertex3f(-2.0, 1.5, 0.0);
	glVertex3f(-2.0, -0.5, 0.0);
	glVertex3f(3.0, -0.5, 0.0);
	glVertex3f(3.0, 1.5, 0.0);
	glEnd();
}

/**************************************************
 * NAME: static void display(void)
 *
 * DESCRIPTION:
 * 		This function is called over and over and is responsible for drawing
 * 		everything.
 *
 * INPUTS:
 *     	EXTERNALS:
 *      	Data *boatData:		A struct containing data from the current run.
 *      	GLuint speedboat:	ID for the speedboat display list.
 *      	GLuint setline:		ID for the setline display list.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 02.04.2017
 **************************************************/
static void display(void)
{
	// convert from our values to window coordinates
	static const float TO_WINDOW_COORDS = -(WINDOW_WIDTH - BOAT_WIDTH) / TANK_WIDTH;

	// calculate the updated positions for the boat and setpoint
	float boatX = ((*boatData).sensorValue - (*boatData).startpoint + TANK_WIDTH / 2.0)
			* TO_WINDOW_COORDS;
	float setpointX = ((*boatData).setpoint - (*boatData).startpoint + TANK_WIDTH / 2.0)
			* TO_WINDOW_COORDS;

	// clear window and select the modelview matrix
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	// draw the boat
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glTranslatef(boatX, 0.0, 0.0);
	glCallList(speedboat);

	// draw the setline
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	if (abs((*boatData).setpoint - (*boatData).sensorValue) < 5)
		glColor3f(0.0, 1.0, 0.0);	// green
	else
		glColor3f(1.0, 0.0, 0.0);	// red
	glTranslatef(setpointX, 0.0, 0.0);
	glCallList(setline);

	drawPowerArrow();

	glFlush();
}

/**************************************************
 * NAME: static void keyboard(unsigned char key, int x, int y)
 *
 * DESCRIPTION:
 * 		This is the keyboard function handed to glut. It handles regular
 * 		keypresses.
 *
 * INPUTS:
 *     	PARAMETERS:
 *     		unsigned char key:	Value of the key pressed
 *     		int x:            	Mouse pointer position.
 *     		int y:            	Mouse pointer position.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case KEY_ENTER:
		glutLeaveMainLoop();	// terminate program, causes close_func() to be run
		break;
	}
}

/**************************************************
 * NAME: static void special_keyboard(int key, int x, int y)
 *
 * DESCRIPTION:
 * 		This is the special keyboard function handed to glut. It handles
 * 		special keypresses, e.g arrow keys, F1, F2...
 *
 * INPUTS:
 *     	PARAMETERS:
 *     		int key:			Value of the key pressed
 *     		int x:				Mouse pointer position.
 *     		int y:				Mouse pointer position.
 *		EXTERNALS:
 * 			BoatData *boatData:		A struct containing data from the current run.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 01.04.2017
 **************************************************/
static void special_keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		(*boatData).setpoint += SETPOINT_INCREMENT;
		if ((*boatData).setpoint > (*boatData).startpoint)
			(*boatData).setpoint = (*boatData).startpoint;
		break;
	case GLUT_KEY_RIGHT:
		(*boatData).setpoint -= SETPOINT_INCREMENT;
		if ((*boatData).setpoint < (*boatData).startpoint - TANK_WIDTH)
			(*boatData).setpoint = (*boatData).startpoint - TANK_WIDTH;
		break;
	}
}

/**************************************************
 * NAME: static void close_func()
 *
 * DESCRIPTION:
 *		This function will run when the openGL window is closed. It sets
 * 		the main loop condition to false, causing the program to finish.
 *
 * INPUTS:
 *		EXTERNALS:
 *			BoatData *boatData:	A struct containing data from the current run.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void close_func()
{
	(*boatData).programRunning = false;
}

/**************************************************
 * NAME: static void setupLighting()
 *
 * DESCRIPTION:
 * 		Sets up the lighting conditions. Standard openGL lighting is Gouraud shading
 * 		which is fast but not the best looking alternative. The functions sets the
 * 		light's position and color, and specifies the material's reflection parameters.
 *
 * INPUTS:
 *		none
 *
 * OUTPUTS:
 *		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void setupLighting()
{

	GLfloat light_position[] = { 5.0, 5.0, 3.0, 0.0 };

	GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	GLfloat shininess[] = { 0.0 };

	GLfloat mat[] = { 1.0, 0.2, 0.2, 1.0 };
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

/**************************************************
 * NAME: static void init(void)
 *
 * DESCRIPTION:
 * 		Does some configuring that only needs to be done once before the program starts.
 *
 * INPUTS:
 *     	none
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void init(void)
{
	//  select clearing (background) color
	glClearColor(0.0, 119.0 / 255, 190.0 / 255, 0.0);

	//  initialize viewing values
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);

	setupLighting();

	// get a display list for the boat
	speedboat = load_obj("data/boat.obj");

	// create display list for setline
	setline = glGenLists(1);
	glNewList(setline, GL_COMPILE);
	glTranslatef(0.0, -3.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-SETLINE_WIDTH, -SETLINE_HEIGHT, 4.0);
	glVertex3f(SETLINE_WIDTH, -SETLINE_HEIGHT, 4.0);
	glVertex3f(SETLINE_WIDTH, SETLINE_HEIGHT, 4.0);
	glVertex3f(-SETLINE_WIDTH, SETLINE_HEIGHT, 4.0);
	glEnd();
	glEndList();
}

/**************************************************
 * NAME: void *start_animation(void *void_ptr)
 *
 * DESCRIPTION:
 *		Starting point for the graphics. Initializes window with parameters,
 * 		sets event handlers for keyboard and sets display function. This
 * 		function is started from a new thread, thus the format of the function.
 *
 * INPUTS:
 *		void *void_ptr:	A pointer to a 'BoatData' struct containing data
 *                     	from the current run.
 *
 * OUTPUTS:
 *		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void *start_animation(void *void_ptr)
{
	boatData = (BoatData*) void_ptr;

	// no input args supported
	int argc = 0;
	char *argv[0];
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(750, 550);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Dynamic Positioning");

	init();

	// don't exit on window close, some things needs to be done afterwards e.g. plotting
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// set glut functions
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutCloseFunc(close_func);

	// start
	glutMainLoop();

	return NULL;
}

