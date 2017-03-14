/*
 * animation.c
 *
 *  Created on: Mar 2, 2017
 *      Author: henrik
 */

#include "headers/animation.h"
void alter_set_point(float value); // Sends keyboard input to main loop

#define SETLINE_WIDTH 0.03

static AnimationData animationData = { 0, 0, 0 };

void display(void)
{
	// Update variables
	// sensor: left-right: 480 - 206
	float boatX = -(float) animationData.position / 1023 * 10.0 + 5.0;
	float setPointX = -(float) animationData.setPoint / 1023 * 10.0 + 5.0;

	/*  clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// Draw the boat
	glLoadIdentity();
	glTranslatef(boatX, 0.0, 0.0);
	glColor3f(0.6, 0.6, 0.6);
	glBegin(GL_POLYGON);
	glVertex3f(-1.5, 0.75, 0.0);
	glVertex3f(-1.5, -0.75, 0.0);
	glVertex3f(0.5, -0.75, 0.0);
	glVertex3f(1.5, 0.0, 0.0);
	glVertex3f(0.5, 0.75, 0.0);
	glEnd();

	glLoadIdentity();
	glTranslatef(setPointX, 0.0, 0.0);
	glColor3f(1.0, 0.3, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-SETLINE_WIDTH, -1.5, 0.0);
	glVertex3f(SETLINE_WIDTH, -1.5, 0.0);
	glVertex3f(SETLINE_WIDTH, 1.5, 0.0);
	glVertex3f(-SETLINE_WIDTH, 1.5, 0.0);
	glEnd();

	/*  don't wait!
	 *  start processing buffered OpenGL routines
	 */
	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{

}

void special_keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		alter_set_point(10.0);
		break;
	case GLUT_KEY_RIGHT:
		alter_set_point(-10.0);
		break;
	}
}

void init(void)
{
	/*  select clearing (background) color       */
	glClearColor(0.0, 119.0 / 255, 190.0 / 255, 0.0);

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0, 5.0, -1.0, 1.0);
}

void *start_animation(void *void_ptr)
{
	// No input args supported
	int argc = 0;
	char *argv[0];
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(750, 500);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Dynamic Positioning");
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special_keyboard);
	glutMainLoop();
	return NULL;
}

void update_animation_data(AnimationData newData)
{
	animationData = newData;
}

