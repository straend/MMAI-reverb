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

void just_delays(double *input, SF_INFO *sf);
void allpass(double *input, SF_INFO *sf);
void comb_filters(double *input, SF_INFO *sf);
void try_moorer(double *samples, SF_INFO *sfinfo);
inline float calcCombGain(const float d_ms, const float rt60);

#endif //MMAI_DELAY_STUFF_H
