/**************************************************
 * FILENAME:	obj_loader.c
 *
 * DESCRIPTION:
 * 		Contains a function for loading wavefront .obj files into OpenGL.
 *
 * PUBLIC FUNCTIONS:
 * 		GLuint load_obj(char fname[])
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/

#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************************************
 * NAME: GLuint load_obj(char fname[])
 *
 * DESCRIPTION:
 * 		Loads the data from a wavefront .obj file into OpenGl compatible format.
 * 		An .obj file is a geometry definition file format containing vertices,
 * 		normals, texture coordinates and faces. Description of the file format can
 * 		be found here: https://en.wikipedia.org/wiki/Wavefront_.obj_file#File_format
 *
 * INPUTS:
 * 		PARAMETERS:
 *      	char fname[]:	The file name to extract the data from.
 *
 * OUTPUTS:
 *     	RETURNS:
 *        	GLuint:	A reference to a openGL display list containing the model from the .obj file.
 *
 * AUTHOR: Jan Henrik Lenes		LAST CHANGE: 20.03.2017
 **************************************************/
GLuint load_obj(char fname[])
{
	// open file
	FILE *fp;
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file: %s\n", fname);
		exit(1);
	}

	// Count occurrences of the different data types
	int nV = 0;		// number of vertices
	int nVT = 0;	// number of vertex textures
	int nVN = 0;	// number of vertex normals
	int nF = 0;		// number of faces/triangles

	char firstWord[30];
	while (!feof(fp))
	{
		fscanf(fp, "%s%*[^\n]", firstWord);	// reads only first word

		if (strcmp(firstWord, "v") == 0)
		{
			nV++;
		} else if (strcmp(firstWord, "vt") == 0)
		{
			nVT++;
		} else if (strcmp(firstWord, "vn") == 0)
		{
			nVN++;
		} else if (strcmp(firstWord, "f") == 0)
		{
			nF++;
		}
	}
	fclose(fp);

	// open file again, we need to read from the start now that
	// we know how many occurrences there are of each data type.
	fp = fopen(fname, "r");
	if (!fp)
	{
		printf("can't open file: %s\n", fname);
		exit(1);
	}

	// arrays to store data in
	GLfloat vertices[nV][3];
	GLfloat textures[nVT][2];
	GLfloat normals[nVN][3];
	GLuint vertexIndices[3 * nF];
	GLuint textureIndices[3 * nF];
	GLuint normalIndices[3 * nF];

	// counter variables
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;

	char lineHeader[20];
	while (!feof(fp))
	{
		fscanf(fp, "%s", lineHeader);		// reads only first word
		if (strcmp(lineHeader, "v") == 0)	// if vertex
		{
			float x, y, z;
			int n = fscanf(fp, "%f %f %f", &x, &y, &z);
			if (n == 3)
			{
				vertices[i][0] = x;
				vertices[i][1] = y;
				vertices[i++][2] = z;
			}
		} else if (strcmp(lineHeader, "vt") == 0)	// if vertex texture
		{
			float x, y;
			int n = fscanf(fp, "%f %f", &x, &y);
			if (n == 2)
			{
				textures[j][0] = x;
				textures[j++][1] = y;
			}
		} else if (strcmp(lineHeader, "vn") == 0)	// if vertex normal
		{
			float x, y, z;
			int n = fscanf(fp, "%f %f %f", &x, &y, &z);
			if (n == 3)
			{
				normals[k][0] = x;
				normals[k][1] = y;
				normals[k++][2] = z;
			}
		} else if (strcmp(lineHeader, "f") == 0)	// if face
		{
			int a, b, c, d, e, f, g, h, i;
			int n = fscanf(fp, "%d/%d/%d %d/%d/%d %d/%d/%d", &a, &b, &c, &d, &e, &f, &g, &h, &i);
			if (n == 9)
			{
				// OpenGL is 0-indexed, thus -1
				vertexIndices[l] = a - 1;
				textureIndices[l] = b - 1;
				normalIndices[l++] = c - 1;

				vertexIndices[l] = d - 1;
				textureIndices[l] = e - 1;
				normalIndices[l++] = f - 1;

				vertexIndices[l] = g - 1;
				textureIndices[l] = h - 1;
				normalIndices[l++] = i - 1;
			}
		}
	}
	fclose(fp);

	/* Reading data is complete, now it needs to be translated to openGL format.
	 This is done in a display list for increased performance */
	GLuint objDisplayList = glGenLists(1);
	glNewList(objDisplayList, GL_COMPILE);

	// do some transformations
	glTranslatef(0.0, -3.5, 0.0);
	glRotatef(90, 0.0, 1.0, 0.0);
	glRotatef(15, 0.0, 0.0, 1.0);
	glScalef(0.14, 0.14, 0.14);

	glBegin(GL_TRIANGLES);
	for (int m = 0; m < l; m++)
	{
		int normalIndex = normalIndices[m];
		int textureIndex = 2 * textureIndices[m];
		int vertexIndex = vertexIndices[m];

		glNormal3f(normals[normalIndex][0], normals[normalIndex][1], normals[normalIndex][2]);
		glTexCoord2f(textures[textureIndex][0], textures[textureIndex][1]);
		glVertex3f(vertices[vertexIndex][0], vertices[vertexIndex][1], vertices[vertexIndex][2]);
	}
	glEnd();

	glEndList();

	// return reference to display list
	return objDisplayList;
}
