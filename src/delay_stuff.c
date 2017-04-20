//
// Created by Tomas Strand on 18/04/2017.
//
// Modified by Candice Persil on 19/04/2017

#include "delay_stuff.h"




// https://github.com/keithhearne/VSTPlugins/blob/master/MoorerReverb/Source/ERTapDelayLine.h
//Early Reflection TAP delay values as presented in Moorer's 1979 paper
//values extracted from impulse measurements in Boston Symphony Hall
//These are the delay times for each of the 18 tap readIndex
//The first tap is unit gain for the input signal, not shown here
const double ER_TAPS[18] = {
    0.0043, 0.0215, 0.0225, 0.0268, 0.0270, 0.0298, 0.0458, 0.0485, 0.0572, 0.0587, 0.0595, 0.0612,
    0.0707, 0.0708, 0.0726, 0.0741, 0.0753, 0.0797
};
//Early reflection GAIN values to apply to each of the above taps
const double ER_GAINS[18] = {
    0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180,
    0.181, 0.176, 0.142, 0.167, 0.134
};

const double comb_delays[] = {50, 56, 61, 68, 72, 78};
const double comb_damp_freq[] = {1942, 1362, 1312, 1574, 981, 1036};

void init_delay(delay_line_s *dl, double delay_ms, double *input, SF_INFO *sf, double gain)
{
    dl->delay_ms = delay_ms;
    dl->input = input;
    dl->sf = sf;
    dl->ptr = 0;
    dl->delay_samples = (uint64_t) ((double)(sf->samplerate) * (delay_ms/1000));
    dl->delay = calloc(sizeof(double), dl->delay_samples);
    if (NULL == dl->delay){
        printf("FAIL FAIL\n");
    }
    dl->gain = gain;
    //printf("Gain: %5.4f \t delay_ms: %5.5f \t samples: %d \t samplerat: %d\n", dl->gain, dl->delay_ms, dl->delay_samples, sf->samplerate);
}

void init_delay_comb(delay_line_s *dl, double delay_ms, double *input, SF_INFO *sf, double gain, double cutoff)
{
    init_delay(dl, delay_ms, input, sf, gain);
    double costh = 2.0 - cos(2.0 * M_PI * cutoff / sf->samplerate);
    dl->lp_coef = sqrt(costh * costh - 1.0) - costh;
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
    //dl->delay[dl->ptr++] = (dl->gain + (x + (y * dl->gain)))/(1 + (dl->gain * (x + (y * dl->gain))));
    double to_dl = x + (dl->gain*y);
    double out = -dl->gain*to_dl + y;
    dl->delay[dl->ptr++] = to_dl;

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return out;
}

double process_comb(delay_line_s *dl, double x)
{

    /*
     * prev = in * (1 + coef) - (prev * coef);
     * return prev;
    */

    double y = dl->delay[dl->ptr];
    double to_lp = y * dl->gain;
    double from_lp = to_lp * (1+dl->lp_coef) - (dl->lp_last * dl->lp_coef);
    dl->lp_last = from_lp;

    double OUT = from_lp + x;
    //dl->delay[dl->ptr++] = (dl->gain)/(1 + (dl->gain * (x + (y * dl->gain))));
    //dl->delay[dl->ptr++] = (1-dl->gain)/(1 + (dl->gain * y));
    dl->delay[dl->ptr++] = OUT;

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return y;
}
#define TAPS (18)
void just_delays(double *input, SF_INFO *sf)
{
    delay_line_s dl[TAPS];
    for(uint8_t d=0;d<TAPS;d++){
        init_delay(&dl[d], ER_TAPS[d]*1000, input, sf, ER_GAINS[d]);
    }

    uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
        double x = input[i];
        double processed = 0;
        for(uint8_t d=0;d<TAPS;d++){
            double y = process_delay(&dl[d], x);
            processed += y;

        }
        input[i] = processed;
    }
    for(uint8_t d=0;d<TAPS;d++){
      free(dl[d].delay);
    }
}

void allpass(double *input, SF_INFO *sf){
	delay_line_s dla;
	init_delay(&dla, 6, input, sf, 0.707);

	uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
		double x = input[i];
        double processed = 0;
        processed += process_allpass(&dla, x);
        input[i] = processed;
    }
	free(dla.delay);

}


void comb_filters(double *input, SF_INFO *sf) {
  float rt60 = 3.0;

  delay_line_s comb[6];

  for (uint8_t c = 0; c < 6; c++) {
    double g = pow(10.0, ((-3.0 * comb_delays[c]) / (rt60 * 1000.0)));
    g=0.2;
    init_delay_comb(&comb[c], comb_delays[c], input, sf, g, comb_damp_freq[c]);
  }

  uint32_t M = sf->frames * sf->channels;
  for (uint32_t i = 0; i < M; i++) {
    double x = input[i];
    double processed = 0;
    for (uint8_t c = 0; c < 6; c++) {
      processed += process_comb(&comb[c], x);
    }
    input[i] = processed;
  }
  for (uint8_t c = 0; c < 6; c++) {
    free(comb[c].delay);
  }
}

void try_moorer(double *samples, SF_INFO *sfinfo)
{

    double *early_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
    memcpy(early_reflections, samples, sfinfo->channels*sfinfo->frames * sizeof(double));
    just_delays(early_reflections, sfinfo);

    double *late_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
    memcpy(late_reflections, early_reflections, sfinfo->channels*sfinfo->frames * sizeof(double));
    comb_filters(late_reflections, sfinfo);
    allpass(late_reflections, sfinfo);

    double dry=0.7;
    double wet=1-dry;
    for(uint32_t i=0; i<sfinfo->channels*sfinfo->frames;i++){
        samples[i] = samples[i] * dry + late_reflections[i]*wet;
    }

    free(early_reflections);
    free(late_reflections);


}
