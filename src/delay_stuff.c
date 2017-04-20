//
// Created by Tomas Strand on 18/04/2017.
//
// Modified by Candice Persil on 19/04/2017

#include "delay_stuff.h"



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


void init_delay(delay_line_s *dl, double delay_ms, double *input, SF_INFO *sf, double gain)
{
    dl->delay_ms = delay_ms;
    dl->input = input;
    dl->sf = sf;
    dl->ptr = 0;
    dl->delay_samples = (uint32_t) (sf->samplerate / (delay_ms));
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

    delay_line_s dl[18];
    for(uint8_t d=0;d<18;d++){
        init_delay(&dl[d], ER_TAPS[d], input, sf, ER_GAINS[d]);
    }

    uint32_t M = sf->frames*sf->channels;
    for (uint32_t i=0; i<M; i++){
        double x = input[i];
        double processed = 0;
        for(uint8_t d=0;d<18;d++){
          processed += process_delay(&dl[d], x);
        }

      input[i] = processed;
    }
    for(uint8_t d=0;d<18;d++){
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
    init_delay(&comb[c], comb_delays[c], input, sf, g);
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
  clock_t start_all, end_early, end_memcpy, end_comb, end_allpass, end_use, end_all;
  start_all = clock();
  double *early_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
  memccpy(early_reflections, samples, sfinfo->channels*sfinfo->frames, sizeof(double));
  just_delays(early_reflections, sfinfo);

  double *late_reflections=calloc(sizeof(double), sfinfo->channels*sfinfo->frames);
  memccpy(late_reflections, early_reflections, sfinfo->channels*sfinfo->frames, sizeof(double));
  comb_filters(late_reflections, sfinfo);
  allpass(late_reflections, sfinfo);
  double dry=0.3;
  double wet=1-dry;

  for(uint32_t i=0; i<sfinfo->channels*sfinfo->frames;i++){
    samples[i] = samples[i] * dry + late_reflections[i]*wet;
  }
  free(early_reflections);
  free(late_reflections);
  end_all = clock();
  double cpu_time_used = ((double) (end_all - start_all)) / CLOCKS_PER_SEC;
  double audio_length = (double)sfinfo->frames/(double)sfinfo->samplerate;
  printf("CPU-time used: %5.2f Audio length: %5.2f \n", cpu_time_used, audio_length);
  if(audio_length>cpu_time_used)
    printf("Should work realtime\n");
  else
    printf("We're too slow\n");

}
