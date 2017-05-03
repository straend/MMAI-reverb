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

// Global *ugly* variables for our delay_lines
delay_line_s dl[TAPS];
delay_line_s comb[COMBS];
delay_line_s dla;

void init_delay(delay_line_s *dl, float delay_ms,
				float *input, SF_INFO *sf, float gain
)
{
    dl->delay_ms = delay_ms;
    dl->input = input;
    dl->sf = sf;
    dl->ptr = 0;
    dl->delay_samples = (uint64_t) ((float)(sf->samplerate) * (delay_ms/1000));

    // Preallocate double of needed memory for our delayline
    dl->delay = calloc(sizeof(float), dl->delay_samples*2);
    dl->samplerate = sf->samplerate;
    
    if (NULL == dl->delay){
        printf("FAIL FAIL\n");
    }
    dl->gain = gain;
}

void set_delay(delay_line_s *dl, float delay_ms, float gain)
{
  uint32_t old_delay_samples = dl->delay_samples;
  dl->delay_ms = delay_ms;
  dl->delay_samples = (uint64_t) ((float)(dl->sf->samplerate) * (dl->delay_ms/1000));
  dl->gain = gain;
  // Check if we need to reallocate memory for our delay_line, if so MAKE IT DOUBLE the needed size
  // (reallocs are not nice (or fast))
  // @TODO: do some interpolating or other magic on values if we need more room
  if (dl->delay_samples>old_delay_samples*2){
    dl->delay = realloc(dl->delay, sizeof(float) * dl->delay_samples*2);
  }
}

void set_delay_comb(delay_line_s *dl, float delay_ms, float gain, float cutoff)
{
  set_delay(dl, delay_ms, gain);
  float costh = 2.0 - cos(2.0 * M_PI * cutoff / dl->sf->samplerate);
  dl->lp_coef = sqrt(costh * costh - 1.0) - costh;

}
void set_earlyRD(float earlyRD)
{
  uint8_t d;
  for(d=0;d<TAPS;d++){
    set_delay(&dl[d], ER_TAPS[d]*earlyRD, ER_GAINS[d]);
  }
}
void set_damping(float damping)
{
  uint8_t d;
  for(d=0;d<TAPS;d++){
    dl[d].gain = ER_GAINS[d]*damping;
  }
}

void set_cutoff(float cutoff)
{
  uint8_t d;
  for(d=0;d<TAPS;d++){
    float costh = 2.0 - cos(2.0 * M_PI * cutoff / dl[d].sf->samplerate);
    dl[d].lp_coef = sqrt(costh * costh - 1.0) - costh;
  }
}

/**
 * Sets the rt60 parameter on the fly, (decay time)
 * @param rt60
 */
void set_rt60(float rt60)
{
  uint8_t c;
  for (c = 0; c < COMBS; c++) {
    float g = pow(10.0, ((-3.0 * comb_delays[c]) / (rt60 * 1000.0)));
    comb[c].gain = g;
  }
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


void init_early(float *samples, SF_INFO *sfinfo, float earlyRD)
{
  uint8_t d;
  for(d=0;d<TAPS;d++){
    init_delay(&dl[d], ER_TAPS[d]*earlyRD, samples, sfinfo, ER_GAINS[d]);
  }
}

void init_combs(float *samples, SF_INFO *sfinfo, float rt60, float damping)
{
  uint8_t c;
  for (c = 0; c < COMBS; c++) {
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
  uint32_t i;
  for (i=0; i<iter; i++){
    float x = input[i];
    float processed = 0;
    uint8_t d;
    for(d=0;d<TAPS;d++){
      float y = process_delay(&dl[d], x);
      processed += y;

    }
    //printf("%f \t %f\n", input[i], processed/TAPS);
    input[i] = processed/TAPS;
  }

}
void process_comb_iter(float *input, const uint32_t iter)
{
  uint32_t i;
  for (i=0; i<iter; i++) {
    float x = input[i];
    float processed = 0;
    uint8_t c;
    for (c = 0; c < COMBS; c++) {
      processed += process_comb(&comb[c], x);
    }
    input[i] = processed;
  }
}
void process_allpass_iter(float *input, const uint32_t iter)
{
  uint32_t i;
  for (i=0; i<iter; i++) {
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
                 reverb_settings_s *rs
)
{
  init_early(samples, sfinfo, rs->earlyRD);
  init_combs(samples, sfinfo, rs->rt60, rs->damping);
  init_allpass(samples, sfinfo, rs->lateRD);

  early_reflections=calloc(sizeof(float), iter*sfinfo->channels);
  late_reflections=calloc(sizeof(float), iter*sfinfo->channels);
  _mixWet = rs->wetmix;
}

void process_moorer(const uint32_t iter, float *samples)
{

  memcpy(early_reflections, samples, sizeof(float)*iter);
  process_early_iter(early_reflections, iter);
  memcpy(late_reflections, early_reflections, sizeof(float)*iter);

  process_comb_iter(late_reflections, iter);
  process_allpass_iter(late_reflections, iter);


  float mixDry=1-_mixWet;
  uint32_t i;
  for (i=0; i<iter; i++) {
    float or = samples[i];
    float m = or*mixDry + late_reflections[i]*_mixWet;
    samples[i] = m;//early_reflections[i];
  }
}

void finnish_moorer()
{
  uint8_t d;
  for(d=0;d<TAPS;d++){
    free(dl[d].delay);
  }
  for (d = 0; d < COMBS; d++) {
    free(comb[d].delay);
  }
  free(dla.delay);

  free(early_reflections);
  free(late_reflections);
}

void print_stuff(float *one, float *two, uint32_t start, uint32_t samples)
{
  printf("#####%d####\n", start);
  uint32_t  i;
  for(i=start; i<start+samples; i++){
    printf("%f \t %f \n", one[i], two[i]);
  }

}

/*
   * RT = 0.16 x V / A
   * T = reverberation time, s
   * V = volume of the room, m3
   * A = (Σ surface area (S) x α) = absorption area of the room, m2
   * For a frequency of 100Hz and 4mm glass as materia matCoef = 0.07
*/
float get_rt60_from_volume_area(float volume, float area) {
    float matCoef = 0.07;
    float A = area * matCoef;
    return 0.16 * volume / A;
}
