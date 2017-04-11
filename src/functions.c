/*************************************************************
* author: Candice PERSIL                                     *
* version: 1                                                 *	
*						function.c 							 *
* 			Basic functions to generate a reverb.			 *
*															 *
**************************************************************/
#include "function.h"

type allpass_filter(type input){
	
	
	return output;
}

// produce 	a reverb effect in an existing array
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

static double D[M]; /* initialized to zero */
static long ptr=0;  /* read-write offset */

double delayline(double x)
{
  double y = D[ptr]; /* read operation */
  D[ptr++] = x;      /* write operation */
  if (ptr >= M) { 
	ptr -= M; 
  } /* wrap ptr */
  return y;
}

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
