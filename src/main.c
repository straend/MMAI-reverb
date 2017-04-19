#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>

#include "sndfile.h"
#include "portaudio.h"

#include "reverb_fake.h"
#include "functions.h"
#include "delay_stuff.h"

typedef struct {
  uint32_t c_sample;
  sf_count_t samples;
  uint8_t  channels;
} paTestData;

double *samples;

#define FRAMES_PER_BUFFER (64)
volatile bool keep_playing = false;

static int paCallback(
  const void *inputBuffer, void *outputBuffer,
  unsigned long framesPerBuffer,
  const PaStreamCallbackTimeInfo* timeInfo,
  PaStreamCallbackFlags statusFlags,
  void *userData)
{
  paTestData *data = (paTestData*)userData;
  float *out = (float*)outputBuffer;
  unsigned long i;

  (void) timeInfo;
  (void) statusFlags;
  (void) inputBuffer;

  for( i=0; i<framesPerBuffer; i++ ) {
    for(uint8_t c=0;c<data->channels;c++)
      *out++ = samples[data->c_sample++];  // left

    if(data->c_sample >= data->samples) {
      data->c_sample = 0;
      // Stop playing
      return  paAbort;
    }
  }

  return paContinue;
}

static void streamFinished( void* userData )
{
  paTestData *data = (paTestData *) userData;
  (void) data;
  printf( "Stream Completed\n");
  keep_playing = false;
}

int main (int argc, char *argv[])
{ 
  char    *infilename=NULL, *outfilename=NULL;
  SNDFILE   *outfile  = NULL;
  SNDFILE   *infile   = NULL ;
  SF_INFO   sfinfo, sfinfo_out;

  PaStreamParameters outputParameters;
  PaStream *stream;
  PaError err;
  paTestData data;
  memset(&data, 0, sizeof(paTestData));

  uint32_t i;


  // initialize portaudio
  if ((err = Pa_Initialize()) != paNoError){
    printf("portaudio error %d: %s\n", err, Pa_GetErrorText(err));
    Pa_Terminate();
    return 1;
  }
  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
    printf("Error: No default output device.\n");
    Pa_Terminate();
    return 1;
  }

  const PaDeviceInfo *deviceInfo;
  deviceInfo = Pa_GetDeviceInfo(outputParameters.device);
  printf( "Device: %s\n", deviceInfo->name );

  if (argc<2){
    printf("Need infile and optional outfile as parameters\n\t$%s infile [outfile]\n", argv[0]);
    Pa_Terminate();

    return 1;
  }

  if(argc==3){
    // Write to outfile
    outfilename = argv [2];
  }
  infilename  = argv [1];

  memset (&sfinfo, 0, sizeof (sfinfo)) ;

  if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL) { 
    printf("Could not open '%s' for reading: %s\n", infilename, sf_strerror(NULL));
    return 1 ;
  };


  // Read samples to array
  samples = malloc(sizeof(double) *  sfinfo.frames * sfinfo.channels);
  sf_readf_double (infile, samples, sfinfo.frames);
  sf_close (infile);
  data.samples = sfinfo.frames*sfinfo.channels;
  data.channels = sfinfo.channels;

    //fake_reverb(samples, &sfinfo);
  try_moorer(samples, &sfinfo);

  // Save audio
  if( NULL != outfilename) {
    // Copy audioinfo and create outfile
    memcpy(&sfinfo_out, &sfinfo, sizeof(SF_INFO));
    if ((outfile = sf_open(outfilename, SFM_WRITE, &sfinfo_out)) == NULL) {
      printf("Could not open '%s' for writing: %s\n", outfilename, sf_strerror(NULL));
      return 1;
    }

    printf("Writing output to: %s\n", outfilename);
    sfinfo_out.frames = sfinfo.frames;

    sf_writef_double(outfile, samples, sfinfo_out.frames);
    sf_close(outfile);
  }

  // Play audio
  outputParameters.channelCount = sfinfo.channels;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.hostApiSpecificStreamInfo = NULL;


  printf("SampleRate: %d\n", sfinfo.samplerate);
  printf("Samples   : %ld\n", sfinfo.frames);

  err = Pa_OpenStream(
    &stream,
    NULL,
    &outputParameters,
    sfinfo.samplerate,
    FRAMES_PER_BUFFER,
    paClipOff,
    paCallback,
    &data
  );

  if( err != paNoError){
    printf("PortAudio Error %d: %s\n", err, Pa_GetErrorText(err));
    Pa_Terminate();
    free(samples);
    return 1;
  }

  err = Pa_SetStreamFinishedCallback( stream, streamFinished );

  err = Pa_StartStream( stream );

  // Wait until we have played all samples
  keep_playing = true;
  while (keep_playing);

  err = Pa_StopStream( stream );
  err = Pa_CloseStream( stream );
  Pa_Terminate();

  free(samples);
  return 0 ;
} 
