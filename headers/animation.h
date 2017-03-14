/*
 * animation.h
 *
 *  Created on: Mar 2, 2017
 *      Author: henrik
 */

#ifndef HEADERS_ANIMATION_H_
#define HEADERS_ANIMATION_H_

#include <stdio.h>
#include <pthread.h>
#include <GL/freeglut.h>

typedef struct
{
	int motorValue;
	int position;
	int setPoint;
} AnimationData;

void *start_animation(void *void_ptr);
void update_animation_data(AnimationData newData);

#endif /* HEADERS_ANIMATION_H_ */
