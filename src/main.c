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
    double *buffer;
} paTestData;

double *samples;

#define FRAMES_PER_BUFFER (64)
volatile bool keep_playing = false;


    static int paCallback(
    const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData
)
{
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo;
    (void) statusFlags;
    (void) inputBuffer;
    //ou init_moorer
    //process_moorer(framesPerBuffer, data->buffer);

    for( i=0; i<framesPerBuffer; i++ ) {
        for(uint8_t c=0;c<data->channels;c++)
	    //out init_moorer
            //*out++ = data->buffer[data->c_sample++];
	    //out try_moorer
	    *out++ = samples[data->c_sample++];
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
    double mix=0, earlyRD=0;/* max mix is 1, max earlyRD is 1, init values of delay are maximum */
    SNDFILE   *outfile  = NULL;
    SNDFILE   *infile   = NULL ;
    SF_INFO   sfinfo, sfinfo_out;

    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data = {0};
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
    enum ARG_STATE {
        ARG_WAIT,
        ARG_DRY_SIGNAL,
        ARG_EARLY,
        ARG_LATE,
        ARG_OUTFILE,
        ARG_RT60
    };
    enum ARG_STATE c_state=ARG_WAIT;
    float dry=0.7,early=0.7,rt60=3.0;
    infilename=argv[1];

    for(uint8_t i=2;i<argc; i++){
        if(strncmp("--",argv[i], 2)==0){
            char *c = argv[i];
            c++;c++;
            if(strncmp("dry", c, 3)==0){
                c_state =ARG_DRY_SIGNAL;
            } else if (strncmp("early", c, 4)==0){
                c_state = ARG_EARLY;
            } else if (strncmp("out", c, 3)==0){
                c_state = ARG_OUTFILE;
            } else if (strncmp("rt60", c, 4)==0){
                c_state = ARG_RT60;
            } else if (strncmp("rt", c, 2)==0){
                //real_time = true;
            }

        } else {
            switch (c_state) {
                case ARG_DRY_SIGNAL:
                  dry = atof(argv[i]);
                  break;
              case ARG_EARLY:
                  early = atof(argv[i]);
                  break;
              case ARG_RT60:
                  rt60 = atof(argv[i]);
                  break;
              case ARG_OUTFILE:
                outfilename = argv[i];
                break;
            }
        }
    }

    infilename  = argv [1];
    mix = dry;
    earlyRD = early;

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
    data.buffer = samples;

    try_moorer(samples, &sfinfo, mix, earlyRD);
    //init_moorer(samples, &sfinfo, FRAMES_PER_BUFFER);

    // Play audio
    outputParameters.channelCount = sfinfo.channels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.hostApiSpecificStreamInfo = NULL;


    printf("SampleRate: %d\n", sfinfo.samplerate);
    printf("Samples   : %lld\n", sfinfo.frames);

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


  err = Pa_StopStream( stream );
  err = Pa_CloseStream( stream );
  Pa_Terminate();
  finnish_moorer();

  free(samples);
  return 0 ;
}
