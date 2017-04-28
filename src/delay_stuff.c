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
const float ER_TAPS[] = {
    43, 21.5, 22.5, 26.8, 27.0, 29.8, 45.8, 48.5, 57.2, 58.7, 59.5, 61.2,
    70.7, 70.8, 72.6, 74.1, 75.3, 79.7
};
//Early reflection GAIN values to apply to each of the above taps
const float ER_GAINS[] = {
    0.841, 0.504, 0.491, 0.379, 0.380, 0.346, 0.289, 0.272, 0.192, 0.193, 0.217, 0.181, 0.180,
    0.181, 0.176, 0.142, 0.167, 0.134
};
#define TAPS (18)

#define COMBS (6)
// also in milliseconds
const float comb_delays[] = {50, 56, 61, 68, 72, 78};
const float comb_damp_freq[] = {1942, 1362, 1312, 1574, 981, 1036};

void init_delay(delay_line_s *dl, float delay_ms,
				float *input, SF_INFO *sf, float gain
)
{
    dl->delay_ms = delay_ms;
    dl->input = input;
    dl->sf = sf;
    dl->ptr = 0;
    dl->delay_samples = (uint64_t) ((float)(sf->samplerate) * (delay_ms/1000));
    dl->delay = calloc(sizeof(float), dl->delay_samples);
    if (NULL == dl->delay){
        printf("FAIL FAIL\n");
    }
    dl->gain = gain;
    //printf("Gain: %5.4f \t delay_ms: %5.5f \t samples: %d \t samplerat: %d\n", dl->gain, dl->delay_ms, dl->delay_samples, sf->samplerate);
}

void init_delay_comb(delay_line_s *dl, float delay_ms,
					float *input, SF_INFO *sf, float gain,
					float cutoff
)
{
    init_delay(dl, delay_ms, input, sf, gain);
    float costh = 2.0 - cos(2.0 * M_PI * cutoff / sf->samplerate);
    dl->lp_coef = sqrt(costh * costh - 1.0) - costh;
}

float process_delay(delay_line_s *dl, float x)
{
    float y = dl->delay[dl->ptr];
    dl->delay[dl->ptr++] = x + (y * dl->gain);
    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return y;
}

float process_allpass(delay_line_s *dl, float x)
{
    float y = dl->delay[dl->ptr];
    //dl->delay[dl->ptr++] = (dl->gain + (x + (y * dl->gain)))/(1 + (dl->gain * (x + (y * dl->gain))));
    float to_dl = x + (dl->gain*y);
    float out = -dl->gain*to_dl + y;
    dl->delay[dl->ptr++] = to_dl;

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return out;
}

float process_comb(delay_line_s *dl, float x)
{

    /*
     * prev = in * (1 + coef) - (prev * coef);
     * return prev;
    */

    float y = dl->delay[dl->ptr];
    float to_lp = y * dl->gain;
    float from_lp = to_lp * (1+dl->lp_coef) - (dl->lp_last * dl->lp_coef);
    dl->lp_last = from_lp;

    float OUT = from_lp + x;
    //dl->delay[dl->ptr++] = (dl->gain)/(1 + (dl->gain * (x + (y * dl->gain))));
    //dl->delay[dl->ptr++] = (1-dl->gain)/(1 + (dl->gain * y));
    dl->delay[dl->ptr++] = OUT;

    if (dl->ptr >= dl->delay_samples) {
        dl->ptr -= dl->delay_samples;
    }
    return y;
}

delay_line_s dl[TAPS];
delay_line_s comb[COMBS];
delay_line_s dla;

void init_early(float *samples, SF_INFO *sfinfo, float earlyRD)
{
  for(uint8_t d=0;d<TAPS;d++){
    init_delay(&dl[d], ER_TAPS[d]*earlyRD, samples, sfinfo, ER_GAINS[d]);
  }
}

void init_combs(float *samples, SF_INFO *sfinfo, float rt60, float damping)
{

  for (uint8_t c = 0; c < COMBS; c++) {
    float g = pow(10.0, ((-3.0 * comb_delays[c]) / (rt60 * 1000.0)));
    init_delay_comb(&comb[c], comb_delays[c], samples, sfinfo, g, comb_damp_freq[c]*damping);
  }
}

void init_allpass(float *samples, SF_INFO *sfinfo, float lateRD)
{
  init_delay(&dla, 6*lateRD, samples, sfinfo, 0.707);
}

void process_early_iter(float *input, const uint32_t iter)
{

  for (uint32_t i=0; i<iter; i++){
    float x = input[i];
    float processed = 0;
    for(uint8_t d=0;d<TAPS;d++){
      float y = process_delay(&dl[d], x);
      processed += y;

    }
    //printf("%f \t %f\n", input[i], processed/TAPS);
    input[i] = processed/TAPS;
  }

}
void process_comb_iter(float *input, const uint32_t iter)
{
  for (uint32_t i=0; i<iter; i++) {
    float x = input[i];
    float processed = 0;
    for (uint8_t c = 0; c < COMBS; c++) {
      processed += process_comb(&comb[c], x);
    }
    input[i] = processed;
  }
}
void process_allpass_iter(float *input, const uint32_t iter)
{
  for (uint32_t i=0; i<iter; i++) {
    float processed = 0;
    float x = input[i];
    processed += process_allpass(&dla, x);
    input[i] = processed;
  }
}

float *early_reflections;
float *late_reflections;
float _mixWet;

void init_moorer(float *samples, SF_INFO *sfinfo, const uint32_t iter,
                 float mixWet, float earlyRD, float lateRD,
                 float rt60, float damping
)
{
  init_early(samples, sfinfo, earlyRD);
  init_combs(samples, sfinfo, rt60, damping);
  init_allpass(samples, sfinfo, lateRD);

  early_reflections=calloc(sizeof(float), iter*sfinfo->channels);
  late_reflections=calloc(sizeof(float), iter*sfinfo->channels);
  _mixWet = mixWet;
}

void process_moorer(const uint32_t iter, float *samples)
{

  memcpy(early_reflections, samples, sizeof(float)*iter);
  process_early_iter(early_reflections, iter);
  memcpy(late_reflections, early_reflections, sizeof(float)*iter);

  process_comb_iter(late_reflections, iter);
  process_allpass_iter(late_reflections, iter);


  float mixDry=1-_mixWet;
  for (uint32_t i=0; i<iter; i++) {
    float or = samples[i];
    float m = or*mixDry + late_reflections[i]*_mixWet;
    samples[i] = m;//early_reflections[i];
  }
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

void print_stuff(float *one, float *two, uint32_t start, uint32_t samples)
{
  printf("#####%d####\n", start);
  for(uint32_t i=start; i<start+samples; i++){
    printf("%f \t %f \n", one[i], two[i]);
  }

}
