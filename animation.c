/**************************************************
 *
 * FILENAME:	animation.c
 *
 * DESCRIPTION:
 * 			This file contains implementations for graphics based on OpenGL.
 * 			It also handles keyboard events.
 *
 *
 * PUBLIC FUNCTIONS:
 * 			void *start_animation(void*)
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 *
 **************************************************/

#include <GL/freeglut.h>

#include "headers/main.h"
#include "headers/obj_loader.h"
#include "headers/pid_controller.h"

#define SETLINE_WIDTH 0.03
#define SETLINE_HEIGHT 1.4
#define KEY_ENTER 13

static Data *boatData;

// OpenGL display list id
static GLuint speedboat;
static GLuint setline;

/**************************************************
 * NAME: static void display(void)
 *
 * DESCRIPTION:
 * 			This function display stuff on the screen. It will run over and over
 * 			handling the graphics.
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
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 21.03.2017
 **************************************************/
static void display(void)
{
	// Update variables
	// sensor: left-right: 480 - 206
	float boatX = -(float) (*boatData).sensorValue / 1000 * 10.0 + 5.0;
	float setPointX = -(float) (*boatData).setPoint / 1000 * 10.0 + 5.0;
	float arrowX = ((*boatData).servoValue - (float) MAX_OUTPUT)
			/ ((float) MIN_OUTPUT - (float) MAX_OUTPUT) * 4.0 - 2.0;
	float arrowColor = 1
			- ((*boatData).servoValue - (float) MAX_OUTPUT)
					/ ((float) MIN_OUTPUT - (float) MAX_OUTPUT);

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Draw arrow depicting power ouput
	glLoadIdentity();
	glTranslatef(-0.5, 0.0, 0.0);
	glColor3f(1.0, arrowColor, arrowColor);
	glBegin(GL_QUADS);
	glVertex3f(-2.0, 1.0, 0.0);
	glVertex3f(-2.0, 0.0, 0.0);
	glVertex3f(arrowX, 0.0, 0.0);
	glVertex3f(arrowX, 1.0, 0.0);
	glEnd();
	glBegin(GL_TRIANGLES);
	glVertex3f(arrowX, -0.5, 0.0);
	glVertex3f(arrowX + 1.0, 0.5, 0.0);
	glVertex3f(arrowX, 1.5, 0.0);
	glEnd();
	glColor3f(0.0, 0.0, 0.0);
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(-2.0, 1.5, 0.0);
	glVertex3f(-2.0, -0.5, 0.0);
	glVertex3f(3.0, -0.5, 0.0);
	glVertex3f(3.0, 1.5, 0.0);

	glEnd();

	// Draw boat
	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glTranslatef(boatX, 0.0, 0.0);
	glCallList(speedboat);

	// Draw setline
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glTranslatef(setPointX, 0.0, 0.0);
	glCallList(setline);

	glFlush();
}

/**************************************************
 * NAME: static void keyboard(unsigned char key, int x, int y)
 *
 * DESCRIPTION:
 * 			This is the keyboard function handed to glut. It handles regular
 * 			keypresses.
 *
 * INPUTS:
 *     	PARAMETERS:
 *     		unsigned char key:	Value of the key pressed
 *     		int x:				Mouse pointer position.
 *     		int y:				Mouse pointer position.
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
		glutLeaveMainLoop();
		break;
	}
}

/**************************************************
 * NAME: static void special_keyboard(int key, int x, int y)
 *
 * DESCRIPTION:
 * 			This is the special keyboard function handed to glut. It handles
 * 			special keypresses, e.g arrow keys, F1, F2...
 *
 * INPUTS:
 *     	PARAMETERS:
 *     		int key:			Value of the key pressed
 *     		int x:				Mouse pointer position.
 *     		int y:				Mouse pointer position.
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void special_keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		(*boatData).setPoint += 5.0;
		if ((*boatData).setPoint > 1000)
			(*boatData).setPoint = 1000;
		set_pid_setpoint((*boatData).setPoint);
		break;
	case GLUT_KEY_RIGHT:
		(*boatData).setPoint -= 5.0;
		if ((*boatData).setPoint < 0)
			(*boatData).setPoint = 0;
		set_pid_setpoint((*boatData).setPoint);
		break;
	}
}

/**************************************************
 * NAME: static void close_func()
 *
 * DESCRIPTION:
 * 			This function will run when the openGL window is closed. It sets
 * 			the main loop condition to false, causing the program to finish.
 *
 * INPUTS:
 *     	none
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
 * NAME: static void setupLightning()
 *
 * DESCRIPTION:
 * 			Sets up the lighting conditions. Standard openGL lighting is Gouraud shading
 * 			which is fast but not the best looking alternative. The functions sets the
 * 			light's position and color, and specifies the material's reflection parameters.
 *
 * INPUTS:
 *     	none
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
static void setupLightning()
{

	GLfloat light_position[] = { 5.0, 5.0, 3.0, 0.0 }; // OK: { 5.0, 5.0, -5.0, 0.0 }

	//GLfloat ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
	//GLfloat specular[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat shininess[] = { 0.0 };

	GLfloat mat[] = { 1.0, 0.2, 0.2, 1.0 };
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

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
 * 			Does some configuring that only needs to be done once before the program starts.
 *
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

	setupLightning();

	speedboat = load_obj("data/boat.obj");

	// Create display list for setline
	setline = glGenLists(1);
	glNewList(setline, GL_COMPILE);
	glTranslatef(0.0, -3.0, 0.0);
	glColor3f(0.0, 1.0, 0.0);
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
 * 			Starting point for the graphics. Initializes window with parameters,
 * 			sets event handlers for keyboard, sets display function...
 *
 *
 * INPUTS:
 *     	void *void_ptr:		A pointer to the boat data from the current run
 *
 * OUTPUTS:
 * 		none
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
void *start_animation(void *void_ptr)
{
	boatData = (Data*) void_ptr;

	// No input args supported
	int argc = 0;
	char *argv[0];
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 400);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Dynamic Positioning");

	init();

	// Don't exit on window close, some things needs to be done afterwards e.g. plotting
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// Set glut functions
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutCloseFunc(close_func);

	// Start
	glutMainLoop();

	return NULL;
}

