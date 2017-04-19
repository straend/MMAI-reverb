//
// Created by Tomas Strand on 18/04/2017.
//
// Modified by Candice Persil on 19/04/2017

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

double process_allpass(delay_line_s *dl, double x)
{
    double y = dl->delay[dl->ptr];
    dl->delay[dl->ptr++] = (dl->gain + (x + (y * dl->gain)))/(1 + (dl->gain * (x + (y * dl->gain))));

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return y;
}

double process_comb(delay_line_s *dl, double x)
{
    double y = dl->delay[dl->ptr];
    dl->delay[dl->ptr++] = (dl->gain)/(1 + (dl->gain * (x + (y * dl->gain))));

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
    double dry = 0.3;
    double wet = 0.1;

    uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
        double x = input[i];
        double processed = 0;
        processed += process_delay(&dl1, x);
        processed += process_delay(&dl2, x);
        processed += process_delay(&dl3, x);

        input[i] = x*dry + processed*wet;
    }
    free(dl1.delay);
    free(dl2.delay);
    free(dl3.delay);
}

void allpass(double *input, SF_INFO *sf){
	delay_line_s dla;
	init_delay(&dla, 100, input, sf, 0.5);
	
	uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
		double x = input[i];
        double processed = 0;
        processed += process_allpass(&dla, x);
    }
	free(dla.delay);
	
}

void comb_filters(double *input, SF_INFO *sf){
	delay_line_s dlcb;
	delay_line_s dlc1;
	delay_line_s dlc2;
	delay_line_s dlc3;
	delay_line_s dlc4;
	delay_line_s dlc5;
	init_delay(&dlcb, 75, input, sf, 0.6);
	init_delay(&dlc1, 100, input, sf, 0.5);
	init_delay(&dlc2, 33, input, sf, 0.8);
	init_delay(&dlc3, 63, input, sf, 0.1);
	init_delay(&dlc4, 22, input, sf, 0.22);
	init_delay(&dlc5, 80, input, sf, 0.66);
	
	uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
		double x = input[i];
        double processed = 0;
        processed += process_comb(&dlcb, x);
        processed += process_comb(&dlc1, x);
        processed += process_comb(&dlc2, x);
        processed += process_comb(&dlc3, x);
        processed += process_comb(&dlc4, x);
        processed += process_comb(&dlc5, x);
    }
	free(dlcb.delay);
	free(dlc1.delay);
	free(dlc2.delay);
	free(dlc3.delay);
	free(dlc4.delay);
	free(dlc5.delay);
}
