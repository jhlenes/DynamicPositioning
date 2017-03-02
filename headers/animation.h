/*
 * animation.h
 *
 *  Created on: Mar 2, 2017
 *      Author: henrik
 */

#ifndef HEADERS_ANIMATION_H_
#define HEADERS_ANIMATION_H_

#include "../DynamicPositioning.h"

void *start_animation(void *void_ptr);


typedef struct
{
	int motorValue;
	int position;
	int setPoint;
} AnimationData;

void update_animation_data(AnimationData newData);


#endif /* HEADERS_ANIMATION_H_ */
