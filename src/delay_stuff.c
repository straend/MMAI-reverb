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
// in milliseconds
const double ER_TAPS[] = {
    43, 21.5, 22.5, 26.8, 27.0, 29.8, 45.8, 48.5, 57.2, 58.7, 59.5, 61.2,
    70.7, 70.8, 72.6, 74.1, 75.3, 79.7
};
//Early reflection GAIN values to apply to each of the above taps
const double ER_GAINS[] = {
    0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180,
    0.181, 0.176, 0.142, 0.167, 0.134
};
#define TAPS (18)

#define COMBS (6)
// also in milliseconds
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
void just_delays(double *input, SF_INFO *sf, double earlyRD)
{
  delay_line_s dl[TAPS];
  for(uint8_t d=0;d<TAPS;d++){
    init_delay(&dl[d], ER_TAPS[d]*earlyRD, input, sf, ER_GAINS[d]);
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

void allpass(double *input, SF_INFO *sf, double lateRD){
	delay_line_s dla;
	init_delay(&dla, 6*lateRD, input, sf, 0.707);

	uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
		    double x = input[i];
        double processed = 0;
        processed += process_allpass(&dla, x);
        input[i] = processed;
    }
	free(dla.delay);

}


void comb_filters(double *input, SF_INFO *sf, double rt60)
{

  delay_line_s comb[COMBS];

  for (uint8_t c = 0; c < COMBS; c++) {
    double g = pow(10.0, ((-3.0 * comb_delays[c]) / (rt60 * 1000.0)));
    //g=0.2;
    init_delay_comb(&comb[c], comb_delays[c], input, sf, g, comb_damp_freq[c]);
  }

  uint32_t M = sf->frames * sf->channels;
  for (uint32_t i = 0; i < M; i++) {
    double x = input[i];
    double processed = 0;
    for (uint8_t c = 0; c < COMBS; c++) {
      processed += process_comb(&comb[c], x);
    }
    input[i] = processed;
  }
  for (uint8_t c = 0; c < COMBS; c++) {
    free(comb[c].delay);
  }
}
delay_line_s dl[TAPS];
delay_line_s comb[COMBS];
delay_line_s dla;

void init_early(double *samples, SF_INFO *sfinfo)
{
  for(uint8_t d=0;d<TAPS;d++){
    init_delay(&dl[d], ER_TAPS[d], samples, sfinfo, ER_GAINS[d]);
  }
}

void init_combs(double *samples, SF_INFO *sfinfo)
{
  float rt60 = 3.0;

  for (uint8_t c = 0; c < COMBS; c++) {
    double g = pow(10.0, ((-3.0 * comb_delays[c]) / (rt60 * 1000.0)));
    g=0.2;
    init_delay_comb(&comb[c], comb_delays[c], samples, sfinfo, g, comb_damp_freq[c]);
  }
}

void init_allpass(double *samples, SF_INFO *sfinfo)
{
  init_delay(&dla, 6, samples, sfinfo, 0.707);
}

void process_early_iter(double *input, const uint32_t iter)
{
  for (uint32_t i=0; i<iter; i++){
    double x = input[i];
    double processed = 0;
    for(uint8_t d=0;d<TAPS;d++){
      double y = process_delay(&dl[d], x);
      processed += y;

    }
    input[i] = processed;
  }
}
void process_comb_iter(double *input, const uint32_t iter)
{
  for (uint32_t i=0; i<iter; i++) {
    double x = input[i];
    double processed = 0;
    for (uint8_t c = 0; c < COMBS; c++) {
      processed += process_comb(&comb[c], x);
    }
    input[i] = processed;
  }
}
void process_allpass_iter(double *input, const uint32_t iter)
{
  for (uint32_t i=0; i<iter; i++) {
    double processed = 0;
    double x = input[i];
    processed += process_allpass(&dla, x);
    input[i] = processed;
  }
}

double *early_reflections;
double *late_reflections;
uint32_t cur_iter=0;

void init_moorer(double *samples, SF_INFO *sfinfo, const uint32_t iter)
{
  init_early(samples, sfinfo);
  init_combs(samples, sfinfo);
  init_allpass(samples, sfinfo);
  early_reflections=calloc(sizeof(double), iter);
  late_reflections=calloc(sizeof(double), iter);
  cur_iter = 0;
}

void process_moorer(const uint32_t iter, double *samples)
{
  memcpy(early_reflections, samples+cur_iter, sizeof(double)*iter);
  process_early_iter(early_reflections, iter);
  memcpy(late_reflections, early_reflections, sizeof(double)*iter);
  process_comb_iter(late_reflections, iter);
  process_allpass_iter(late_reflections, iter);

  double dry = 0.3;
  double wet = 1-dry;

  for (uint32_t i=0; i<iter; i++) {
    samples[i+cur_iter] = samples[i+cur_iter]*dry + late_reflections[i]*wet;
  }
  cur_iter+=iter;
}

void finnish_moorer()
{
  for(uint8_t d=0;d<TAPS;d++){
    free(dl[d].delay);
  }
  for (uint8_t c = 0; c < COMBS; c++) {
    free(comb[c].delay);
  }
  free(dla.delay);

  free(early_reflections);
  free(late_reflections);
}

void try_moorer(double *samples, SF_INFO *sfinfo, double mixWet/*, double dry, double wet*/, double earlyRD, double lateRD, double rt60)
{

    double *early_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
    memcpy(early_reflections, samples, sfinfo->channels*sfinfo->frames * sizeof(double));
    just_delays(early_reflections, sfinfo, earlyRD);

    double *late_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
    memcpy(late_reflections, early_reflections, sfinfo->channels*sfinfo->frames * sizeof(double));
    comb_filters(late_reflections, sfinfo, rt60);
    allpass(late_reflections, sfinfo, lateRD);

    //proportional mix of dry and wet
    double mixDry=1-mixWet;
    //other possibility is to make vary both
    //dry and wet between 0 and 1
    // double dry=0.7;
    // double wet=0.6;
    for(uint32_t i=0; i<sfinfo->channels*sfinfo->frames;i++){
        //first possibility
	samples[i] = samples[i] * mixDry + late_reflections[i]*mixWet;
	//second possibility
	//samples[i] = samples[i] * dry + late_reflections[i]*wet;
    }

    free(early_reflections);
    free(late_reflections);


}
