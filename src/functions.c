/*************************************************************
* author: Candice PERSIL                                     *
* version: 1                                                 *	
*						function.c 							 *
* 			Basic functions to generate a reverb.			 *
*															 *
**************************************************************/
#include "functions.h"
#include "sndfile.h"
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

//#define B0 		1 //other constant value
//#define AM		1 //other constant value

float b0;
float am;
//static float D[M]; /* initialized to zero */
static float *D;					// table which contains values of the input sound
static long ptr=0;  /* read-write offset */
uint32_t M;

// Combfilters
static float *paramB0; //[] = {123, 523, 345, 222, 164,253}; //values of b0 for different comb filters
static float *paramAm; //[] = {0.6, 0.7, 0.3, 0.5, 0.2, 0.3}; //values of Am for different comb filters

// function of a delay line
float delayline(float x)
{
  float y = D[ptr]; /* read operation */
  D[ptr++] = x;      /* write operation */
  if (ptr >= M) { 
	ptr -= M; 
  } /* wrap ptr */
  return y;
}

// function of an allpass filter 
// found from a transfer function
float allpass_filter(float input, float b0, float am){
	float tmp = delayline(input);
	return (b0 + tmp) / (1 + am*tmp);
}

 float get_gain(float t, float RVT);

// g = 0.001 ^(tau / RVT)
 float get_gain(float t, float RVT)
{
    return pow(0.001, t/RVT);
}

// function of a feedback comb filter 
// found from a transfer function
// Different comb filters will have to be used 
// The variation should be the constant values b0 and Am?
float feedback_comb_filter(float input, float b0, float am){
	float tmp = delayline(input);
	return b0/(1+am*tmp);
}


float sum_comb_filters(float input, float *paramB0, float *paramAm){
	// initialisation
	float sum = 0;

    for (int i = 0; i < 1; i++) {
		sum += feedback_comb_filter(input, paramB0[i], paramAm[i]);
	}
	return sum;
}

float late_reflections_network(float input){

    //return allpass_filter(input, b0, am);
    return sum_comb_filters(input, paramB0, paramAm);

    return delayline(
            allpass_filter(
                    sum_comb_filters(input, paramB0, paramAm)
                    , b0, am
            )
    );
}

void reverb_time(float *samples, SF_INFO *sfInfo)
{
    float rvt[] = {1.1, 1.5, 1.8, 1.2};
    float tau[] = {0.0297, 0.0371, 0.0411,0.0437};

    uint32_t  filters=sizeof(rvt)/ sizeof(rvt[0]);
    paramB0 = calloc(sizeof(rvt[0]), filters);
    paramAm = calloc(sizeof(rvt[0]), filters);

    for(uint32_t i=0;i< filters;i++){
        //paramB0[i] = rvt[i];
        //paramB0[i] = rvt[i];
        paramB0[i] = tau[i];

        //paramB0[i] = get_gain(tau[i], rvt[i]);

        //paramAm[i] = get_gain(tau[i], rvt[i]);
        //paramAm[i] = tau[i];
        paramAm[i] = rvt[i];
    }
    am = 0.005;
    b0 = 0.09683;
    D = samples;
    M = sfInfo->frames*sfInfo->channels;
    float *d = calloc(sizeof(float), M);
    memcpy(d, D, M* sizeof(float));
    for (uint32_t i=0; i<M; i++){
        samples[i] += late_reflections_network(samples[i]);
    }
    M= 200;
    for (uint32_t i=0; i<M; i++){
        printf("%f \t%f\n",samples[i], d[i]);
    }

}



// produce 	a reverb effect in an existing array
// delay_line
/*
void delay_line(float *buffer){
	int delayMilliseconds = 500; // half a second
	int delaySamples = (int)((float)delayMilliseconds * sample_rate); // assumes 44100 Hz sample rate
	int i;
	for (i = 0; i < buffer.length - delaySamples; i++)
	{
		// WARNING: overflow potential
		buffer[i + delaySamples] += (float)((float)buffer[i] * decay);
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