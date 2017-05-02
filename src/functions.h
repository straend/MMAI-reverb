/*************************************************************
* author: Candice PERSIL                                     *
* version: 1                                                 *	
*						function.h 							 *
* 				Prototypes of function.c					 *
*															 *
**************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "sndfile.h"

//extern float sample_rate = 44.1f;
//extern float decay = 0.5f;
//extern float buffer[];

//void delay_line(float *buffer);

void reverb_time(float *samples, SF_INFO *sfInfo);


#endif // FUNCTIONS_H