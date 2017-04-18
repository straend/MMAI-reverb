/*************************************************************
* author: Candice PERSIL                                     *
* version: 1                                                 *	
*						function.c 							 *
* 			Basic functions to generate a reverb.			 *
*															 *
**************************************************************/
#include "function.h"


//#define B0 		1 //other constant value
//#define AM		1 //other constant value

static double D[M]; /* initialized to zero */
					// table which contains values of the input sound
static long ptr=0;  /* read-write offset */

static double paramB0 = []; //values of b0 for different comb filters
static double paramAm = []; //values of Am for different comb filters

// function of a delay line
double delayline(double x)
{
  double y = D[ptr]; /* read operation */
  D[ptr++] = x;      /* write operation */
  if (ptr >= M) { 
	ptr -= M; 
  } /* wrap ptr */
  return y;
}

// function of an allpass filter 
// found from a transfer function
double allpass_filter(double input double b0, double am){
	double tmp = delayline(input);
	return (b0 + tmp) / (1 + am*tmp);
}

// function of a feedback comb filter 
// found from a transfer function
// Different comb filters will have to be used 
// The variation should be the constant values b0 and Am?
double feedback_comb_filter(double input, double b0, double am){
	double tmp = delayline(input);
	return b0/(1+am*tmp);
}


double sum_comb_filters(double input, double paramB0[], double paramAm[]){
	// initialisation
	double sum = 0;
	
	for (int i = 0; i < 6; i++) {
		sum += feedback_comb_filter(input, paramB0[i], paramAm[i]);
	}
	
	return sum;
}

double late_reflections_network(double input){
	return delayline(allpass_filter(sum_comb_filters(input, paramB0, paramAm), b0, am));
}



// produce 	a reverb effect in an existing array
// delay_line
/*
void delay_line(double *buffer){
	int delayMilliseconds = 500; // half a second
	int delaySamples = (int)((float)delayMilliseconds * sample_rate); // assumes 44100 Hz sample rate
	int i;
	for (i = 0; i < buffer.length - delaySamples; i++)
	{
		// WARNING: overflow potential
		buffer[i + delaySamples] += (double)((float)buffer[i] * decay);
	}
}
*/

// Example of a reverb in C++
/*
void revmodel::processreplace(float *inputL, float *inputR, 
  float *outputL, float *outputR, long numsamples, int skip)
{
  float outL,outR,input;
  int i;

  while(numsamples-- > 0)
    {
      outL = outR = 0;
      input = (*inputL + *inputR) * gain;

      // Accumulate comb filters in parallel
      for(i=0; i<numcombs; i++) {
        outL += combL[i].process(input);
        outR += combR[i].process(input);
      }

      // Feed through allpasses in series
      for(i=0; i<numallpasses; i++) {
        outL = allpassL[i].process(outL);
        outR = allpassR[i].process(outR);
      }

      // Calculate output REPLACING anything already there
      *outputL = outL*wet1 + outR*wet2 + *inputL*dry;
      *outputR = outR*wet1 + outL*wet2 + *inputR*dry;

      // Increment sample pointers, allowing for interleave 
      // (if any)
      inputL += skip; // For stereo buffers, skip = 2
      inputR += skip;
      outputL += skip;
      outputR += skip;
    }
}
*/