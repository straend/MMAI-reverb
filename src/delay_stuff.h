//
// Created by Tomas Strand on 18/04/2017.
//

#ifndef MMAI_DELAY_STUFF_H
#define MMAI_DELAY_STUFF_H

#include "sndfile.h"

void just_delays(double *input, SF_INFO *sf);
void allpass(double *input, SF_INFO *sf);
void comb_filters(double *input, SF_INFO *sf);

#endif //MMAI_DELAY_STUFF_H
