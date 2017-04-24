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
    double *input;
    double delay_ms;
    uint64_t delay_samples;
    uint64_t ptr;
    SF_INFO *sf;
    double *delay;
    uint64_t  p;
    double gain;
    double lp_coef;
    double lp_last;

} delay_line_s;
void init_delay(delay_line_s *dl, double delay_ms, double *input, SF_INFO *sf, double gain);
double process_delay(delay_line_s *dl, double x);


void just_delays(double *input, SF_INFO *sf);
void allpass(double *input, SF_INFO *sf);
void comb_filters(double *input, SF_INFO *sf);
void try_moorer(double *samples, SF_INFO *sfinfo, double mix);

void init_moorer(double *samples, SF_INFO *sfinfo, const uint32_t iter);
void process_moorer(const uint32_t iter, double *samples);
void finnish_moorer();


#endif //MMAI_DELAY_STUFF_H
