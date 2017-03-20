/*
 * animation.c
 *
 *  Created on: Mar 2, 2017
 *      Author: henrik
 */

#include <stdbool.h>
#include <string.h>
#include "headers/main.h"
#include "headers/animation.h"

#define SETLINE_WIDTH 0.03
#define SETLINE_HEIGHT 1.4
#define KEY_ENTER 13

static Data *boatData;

static GLuint speedboat;

void display(void)
{
	// Update variables
	// sensor: left-right: 480 - 206
	float boatX = -(float) (*boatData).sensorValue / 1000 * 10.0 + 5.0;
	float setPointX = -(float) (*boatData).setPoint / 1000 * 10.0 + 5.0;

	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw the boat
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glEnable(GL_LIGHTING);
	glTranslatef(boatX, -4.0, 0.0);
	glRotatef(90, 0.0, 1.0, 0.0);
	glRotatef(15, 0.0, 0.0, 1.0);
	glScalef(0.2, 0.2, 0.2);

	glCallList(speedboat);

	// Draw setline
	glLoadIdentity();
	glDisable(GL_LIGHTING);
	glTranslatef(setPointX, -3.5, 0.0);
	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex3f(-SETLINE_WIDTH, -SETLINE_HEIGHT, 4.0);
	glVertex3f(SETLINE_WIDTH, -SETLINE_HEIGHT, 4.0);
	glVertex3f(SETLINE_WIDTH, SETLINE_HEIGHT, 4.0);
	glVertex3f(-SETLINE_WIDTH, SETLINE_HEIGHT, 4.0);
	glEnd();

	glFlush();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case KEY_ENTER:
		glutLeaveMainLoop();
		break;
	}
}

void special_keyboard(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		(*boatData).setPoint += 5.0;
		break;
	case GLUT_KEY_RIGHT:
		(*boatData).setPoint -= 5.0;
		break;
	}
}

void close_func()
{
	set_program_status(false);
}

void init(void)
{
	/*  select clearing (background) color       */
	glClearColor(0.0, 119.0 / 255, 190.0 / 255, 0.0);

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-5.0, 5.0, -5.0, 5.0, -5.0, 5.0);
}

void loadObj(char fname[])
{
	FILE *fp;
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file: %s\n", fname);
		exit(1);
	}

	int nV = 0;
	//int nVT = 0;
	int nVN = 0;
	int nF = 0;

	char firstWord[30];
	while (!feof(fp))
	{
		fscanf(fp, "%s%*[^\n]", firstWord);	// reads only first word

		if (strcmp(firstWord, "v") == 0)
		{
			nV++;
		} /*else if (strcmp(firstWord, "vt") == 0)
		 {
		 nVT++;
		 }*/else if (strcmp(firstWord, "vn") == 0)
		{
			nVN++;
		} else if (strcmp(firstWord, "f") == 0)
		{
			nF++;
		}
	}
	//printf("vertices: %d    faces: %d\n", nV, nF);
	fclose(fp);
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file: %s\n", fname);
		exit(1);
	}

	GLfloat vertices[nV][3];
	//GLfloat textures[nVT][2];
	GLfloat normals[nVN][3];

	GLuint vertexIndices[3 * nF];
	//GLuint uvIndices[3 * nF];
	GLuint normalIndices[3 * nF];

	int i = 0;
	//int j = 0;
	int k = 0;
	int l = 0;

	char lineHeader[20];
	while (!feof(fp))
	{
		fscanf(fp, "%s", lineHeader);	// reads only first word
		if (strcmp(lineHeader, "v") == 0)
		{
			float x, y, z;
			int n = fscanf(fp, "%f %f %f", &x, &y, &z);
			if (n == 3)
			{
				vertices[i][0] = x;
				vertices[i][1] = y;
				vertices[i++][2] = z;
			}
		} /*else if (strcmp(lineHeader, "vt") == 0)
		 {
		 float x, y;
		 int n = fscanf(fp, "%f %f", &x, &y);
		 if (n == 2)
		 {
		 textures[j][0] = x;
		 textures[j++][1] = y;
		 }
		 } */else if (strcmp(lineHeader, "vn") == 0)
		{
			float x, y, z;
			int n = fscanf(fp, "%f %f %f", &x, &y, &z);
			if (n == 3)
			{
				normals[k][0] = x;
				normals[k][1] = y;
				normals[k++][2] = z;
			}
		} else if (strcmp(lineHeader, "f") == 0)
		{
			int a, b, c, d, e, f, g, h, i;
			int n = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &a, &b, &c, &d, &e, &f, &g, &h, &i);
			if (n == 9)
			{
				vertexIndices[l] = a - 1;
				//uvIndices[l] = b - 1;
				normalIndices[l++] = c - 1;

				vertexIndices[l] = d - 1;
				//uvIndices[l] = e - 1;
				normalIndices[l++] = f - 1;

				vertexIndices[l] = g - 1;
				//uvIndices[l] = h - 1;
				normalIndices[l++] = i - 1;
			}
		}
	}
	fclose(fp);

	speedboat = glGenLists(1);
	glNewList(speedboat, GL_COMPILE);

	glBegin(GL_TRIANGLES);

	for (int m = 0; m < l; m++)
	{
		int normalIndex = normalIndices[m];
		//int uvIndex = 2 * uvIndices[m];
		int vertexIndex = vertexIndices[m];

		glNormal3f(normals[normalIndex][0], normals[normalIndex][1], normals[normalIndex][2]);
		//glTexCoord2f(textures[uvIndex], textures[uvIndex + 1]);
		glVertex3f(vertices[vertexIndex][0], vertices[vertexIndex][1], vertices[vertexIndex][2]);
	}

	glEnd();

	glEndList();

}

void setupLightning()
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

	loadObj("data/boat.obj");
	setupLightning();

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

