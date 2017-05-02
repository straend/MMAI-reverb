//
// Created by Tomas Strand on 18/04/2017.
//

#ifndef MMAI_DELAY_STUFF_H
#define MMAI_DELAY_STUFF_H

#include "sndfile.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>


typedef struct{
    float *input;
    float delay_ms;
    uint64_t delay_samples;
	uint64_t samplerate;
    uint64_t ptr;
    SF_INFO *sf;
    float *delay;
    uint64_t  p;
    float gain;
    float lp_coef;
    float lp_last;

} delay_line_s;
void init_delay(delay_line_s *dl, float delay_ms,
				float *input, SF_INFO *sf, float gain
);
float process_delay(delay_line_s *dl, float x);

void print_stuff(float *one, float *two, uint32_t start, uint32_t samples);

void just_delays(float *input, SF_INFO *sf, float earlyRD);
void allpass(float *input, SF_INFO *sf, float lateRD);
void comb_filters(float *input, SF_INFO *sf,
                  float rt60, float damping
);
void try_moorer(float *samples, SF_INFO *sfinfo, float mix,
				float earlyRD, float lateRD, float rt60, float damping
);

void init_moorer(float *samples, SF_INFO *sfinfo, const uint32_t iter,
				 float mixWet, float earlyRD, float lateRD,
				 float rt60, float damping
);

void process_moorer(const uint32_t iter, float *samples);
void finnish_moorer();

void set_rt60(float rt60);
void set_earlyRD(float earlyRD);
void set_damping(float damping);
void set_cutoff(float cutoff);

float get_rt60_from_volume_area(float volume, float area);



#endif //MMAI_DELAY_STUFF_H
