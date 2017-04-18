//
// Created by Tomas Strand on 18/04/2017.
//

#include "delay_stuff.h"

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sndfile.h"

typedef struct{
    double *input;
    double delay_ms;
    uint32_t delay_samples;
    uint32_t ptr;
    SF_INFO *sf;
    double *delay;
    uint32_t  p;
    double gain;
} delay_line_s;


void init_delay(delay_line_s *dl, double delay_ms, double *input, SF_INFO *sf, double gain)
{
    dl->delay_ms = delay_ms;
    dl->input = input;
    dl->sf = sf;
    dl->ptr = 0;
    dl->delay_samples = (uint32_t) (sf->samplerate / (delay_ms));
    printf("Delay_samples: %d \t samplerate:: %d\n", dl->delay_samples, sf->samplerate);
    dl->delay = calloc(sizeof(double), dl->delay_samples);
    if (NULL == dl->delay){
        printf("FAIL FAIL\n");
    }
    dl->gain = gain;

}

double process_delay(delay_line_s *dl, double x)
{
    double y = dl->delay[dl->ptr];
    dl->delay[dl->ptr++] = x + (y * dl->gain);

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return y;
}

void just_delays(double *input, SF_INFO *sf)
{
    delay_line_s dl1, dl2, dl3;

    init_delay(&dl1, 50, input, sf, 0.667);
    init_delay(&dl2, 200, input, sf, 0.33);
    init_delay(&dl3, 100, input, sf, 0.13);

    uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
        double x = input[i];
        input[i] += process_delay(&dl1, x);
        input[i] += process_delay(&dl2, x);
        input[i] += process_delay(&dl3, x);
    }
    free(dl1.delay);
    free(dl2.delay);
    free(dl3.delay);
}