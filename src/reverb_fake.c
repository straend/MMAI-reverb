//
// Created by Tomas Strand on 11/04/2017.
//
#include "reverb_fake.h"
#include <stdint.h>

// https://christianfloisand.wordpress.com/2012/10/18/algorithmic-reverbs-the-moorer-design/

// decay 0.5f
void delay_line(float *buffer, SF_INFO *sfinfo, float decay){
    int delayMilliseconds = 800; // half a second

    int delaySamples = (int)((float)delayMilliseconds * sfinfo->samplerate); // assumes 44100 Hz sample rate
    int i;
    int lim = sfinfo->frames / sfinfo->channels - delaySamples;
    for (i = 0; i < lim; i++)
    {
        // WARNING: overflow potential
        int channel;
        for(channel = 0; channel < sfinfo->channels; channel++) {
            buffer[i + delaySamples + channel] += (float) ((float) buffer[i + channel] * decay);
        }
    }
}

/*

float D[M];     // initialized to zero
long ptr=0;     // read-write offset

float delayline(float x)
{
    float y = D[ptr]; // read operation
    D[ptr++] = x;      // write operation
    if (ptr >= M) {
        ptr -= M;
    } // wrap ptr
    return y;
}
*/
void fake_reverb(float *samples, SF_INFO *sfinfo)
{
    delay_line(samples, sfinfo, 0.9f);


}
