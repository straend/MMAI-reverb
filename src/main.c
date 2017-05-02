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
    float *buffer;
} paTestData;

float *samples;
uint32_t  processed=0;
#define FRAMES_PER_BUFFER (1024)
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

    float *d = data->buffer;
    d+=data->c_sample;
    uint32_t  to_process = framesPerBuffer*data->channels;
    if(to_process+data->c_sample > data->samples)
        to_process = data->samples - data->c_sample;

    process_moorer(to_process, d);
    processed += to_process;
    for( i=0; i<to_process; i++ ) {
        *out++ = data->buffer[data->c_sample++];
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
reverb_settings_s rs;
void print_usage(char *cmd_name)
{
    const char *help="\
     __  __ __  __   _   ___   ___ _____   _____ ___ ___     \n\
    |  \\/  |  \\/  | /_\\ |_ _| | _ \\ __\\ \\ / / __| _ \\ _ )    \n\
    | |\\/| | |\\/| |/ _ \\ | |  |   / _| \\ V /| _||   / _ \\    \n\
    |_|  |_|_|  |_/_/ \\_\\___| |_|_\\___| \\_/ |___|_|_\\___/    \n\
   ┌───────────────────────────────────────────────────────┐ \n\
   │Required parameter: audio-file to play, first argument │ \n\
   │                                                       │ \n\
   │Optional parameters:     Default                       │ \n\
   |                                                       | \n\
   │--wet       [0.0-1.0]    0.7                           │ \n\
   |                                                       | \n\
   │--rt60      seconds      3.5s                          │ \n\
   |                                                       | \n\
   │--reflect   [0.0-1.0]    0.7                           │ \n\
   |                                                       | \n\
   |--damping   [0.0-1.0]    1                             | \n\
   │                                                       │ \n\
   │--area      [0.0-100.0]   20 m^2                       │ \n\
   |                                                       | \n\
   |--volume    [0.0-200.0]   40 m^3                       | \n\
   │                                                       │ \n\
   │--out       filename to write Reverbed audio (wav)     │ \n\
   │                                                       │ \n\
   │Example:                                               │ \n\
   │%s ../audio/saxGandalf.wav --dry 0.6 --rt60 5.3    │ \n\
   │                                                       │ \n\
   │                                                       │ \n\
   └───────────────────────────────────────────────────────┘ \n\
    \n\n";
    printf(help, cmd_name);
}

void parse_settings(reverb_settings_s *rs, int argc, char *argv[])
{
    enum ARG_STATE {
      ARG_WAIT,
      ARG_WET_SIGNAL,
      ARG_SIZE,
      ARG_OUTFILE,
      ARG_AREA,
      ARG_VOLUME,
      ARG_RT60,
      ARG_DAMPING /* The dampening will make the "wet" sound of reverb less apparent */
      /* High cut: Emulates the effect of high frequencies being absorbed */
    };
    enum ARG_STATE c_state=ARG_WAIT;
    float wet=0.7,reflect=0.7,damping=1.0,area=20, volume=40, rt60=3.5;

    bool has_rt60 = false;
    bool has_volume_or_area = false;
    for(uint8_t i=2;i<argc; i++){
    if(strncmp("--",argv[i], 2)==0){
      char *c = argv[i];
      c++;c++;
      if(strncmp("wet", c, 3)==0){
        c_state =ARG_WET_SIGNAL;

      } else if (strncmp("reflect", c, 7)==0){
        c_state = ARG_SIZE;

      } else if (strncmp("rt60", c, 4)==0){
        c_state = ARG_RT60;

      } else if (strncmp("out", c, 3)==0){
        c_state = ARG_OUTFILE;

      } else if (strncmp("area", c, 4)==0){
        c_state = ARG_AREA;

      }  else if (strncmp("volume", c, 6)==0){
        c_state = ARG_VOLUME;

      } else if (strncmp("damping", c, 7)==0){
        c_state = ARG_DAMPING;
      }

    } else {
      switch (c_state) {
      case ARG_WET_SIGNAL:
          rs->wetmix = atof(argv[i]);
          break;
      case ARG_SIZE:
        rs->reflect = atof(argv[i]);
        break;
      case ARG_AREA:
        rs->area = atof(argv[i]);
        has_volume_or_area = true;
        break;
      case ARG_VOLUME:
        rs->volume = atof(argv[i]);
        has_volume_or_area = true;
        break;
      case ARG_DAMPING:
        rs->damping = 1/atof(argv[i]);
        break;
      case ARG_RT60:
        rs->rt60 = atof(argv[i]);
        has_rt60 = true;
        break;

      case ARG_OUTFILE:
        outfilename = argv[i];
        break;
      default:
      break;
      }
    }
  }

  if(rs->reflect==0.0)
    rs->reflect = 0.01;

  if(!has_rt60 && has_volume_or_area){
    rs->rt60 = get_rt60_from_volume_area(volume, area);
  }
  rs->earlyRD = rs->reflect;
  rs->lateRD = rs->reflect;
}

int main (int argc, char *argv[])
{
    char    *infilename=NULL, *outfilename=NULL;
    float mix=0, earlyRD=0, lateRD=0;/* max mix is 1, max earlyRD is 1, init values of delay are maximum */
    SNDFILE   *outfile  = NULL;
    SNDFILE   *infile   = NULL ;
    SF_INFO   sfinfo, sfinfo_out;

    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data = {0};
    uint32_t i;


    parse_settings(&rs, argc, argv);
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

    if (argc<2){
        print_usage(argv[0]);
        Pa_Terminate();
        return 1;
    }
    printf( "Device: %s\n", deviceInfo->name );

    enum ARG_STATE {
        ARG_WAIT,
        ARG_WET_SIGNAL,
        ARG_SIZE,
        ARG_OUTFILE,
        ARG_AREA,
        ARG_VOLUME,
        ARG_RT60,
        ARG_DAMPING /* The dampening will make the "wet" sound of reverb less apparent */
        /* High cut: Emulates the effect of high frequencies being absorbed */
    };
    enum ARG_STATE c_state=ARG_WAIT;
    float wet=0.7,reflect=0.7,damping=1.0,area=20, volume=40, rt60=3.5;
    infilename=argv[1];

    bool has_rt60 = false;
    bool has_volume_or_area = false;
    for(uint8_t i=2;i<argc; i++){
        if(strncmp("--",argv[i], 2)==0){
            char *c = argv[i];
            c++;c++;
            if(strncmp("wet", c, 3)==0){
                c_state =ARG_WET_SIGNAL;

            } else if (strncmp("reflect", c, 7)==0){
                c_state = ARG_SIZE;

            } else if (strncmp("rt60", c, 4)==0){
                c_state = ARG_RT60;

            } else if (strncmp("out", c, 3)==0){
                c_state = ARG_OUTFILE;

            } else if (strncmp("area", c, 4)==0){
                c_state = ARG_AREA;

            }  else if (strncmp("volume", c, 6)==0){
                c_state = ARG_VOLUME;

            } else if (strncmp("rt", c, 2)==0){
                //real_time = true;

            } else if (strncmp("damping", c, 7)==0){
                c_state = ARG_DAMPING;
            }

        } else {
            switch (c_state) {
                case ARG_WET_SIGNAL:
                  wet = atof(argv[i]);
                  break;
              case ARG_SIZE:
                  reflect = atof(argv[i]);
                  break;
              case ARG_AREA:
                  area = atof(argv[i]);
                  has_volume_or_area = true;
                  break;
              case ARG_VOLUME:
                  volume = atof(argv[i]);
                  has_volume_or_area = true;
                  break;
              case ARG_DAMPING:
                  damping = 1/atof(argv[i]);
                  break;
              case ARG_RT60:
                  rt60 = atof(argv[i]);
                  has_rt60 = true;
                  break;

              case ARG_OUTFILE:
                outfilename = argv[i];
                break;
            default:
                break;
            }
        }
    }

    if(reflect==0.0)
      reflect = 0.01;

    if(!has_rt60 && has_volume_or_area){
        rt60 = get_rt60_from_volume_area(volume, area);
    }
    earlyRD = reflect;
    lateRD = reflect;

    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL) {
        printf("Could not open '%s' for reading: %s\n", infilename, sf_strerror(NULL));
        return 1 ;
    };


    // Read samples to array
    samples = malloc(sizeof(float) *  sfinfo.frames * sfinfo.channels);
    sf_readf_float(infile, samples, sfinfo.frames);
    sf_close (infile);
    data.samples = sfinfo.frames*sfinfo.channels;
    data.channels = sfinfo.channels;
    data.buffer = samples;
    //try_moorer(samples, &sfinfo, wet, earlyRD, lateRD, rt60, damping);

    init_moorer(samples, &sfinfo, FRAMES_PER_BUFFER, &rs);
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
    bool changed = false;
    while (keep_playing){
        if (!changed && processed > data.samples/2){
            printf("CJANGE\n");
            changed = true;
            //set_rt60(0.1);
            //set_earlyRD(0.1);
            set_cutoff(0.5);



        }
    }
    printf("Processed: %d\n", processed);

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
    sf_writef_float(outfile, samples, sfinfo_out.frames);
    sf_close(outfile);
  }


  err = Pa_StopStream( stream );
  err = Pa_CloseStream( stream );
  Pa_Terminate();
  finnish_moorer();

  free(samples);
  return 0 ;
}
