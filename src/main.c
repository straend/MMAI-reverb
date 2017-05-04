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

#include "GUI.h"

typedef struct {
    uint32_t c_sample;
    sf_count_t samples;
    uint8_t  channels;
    float *buffer;
} paTestData;

float *samples;
uint32_t  processed=0;
#define FRAMES_PER_BUFFER (64)
volatile bool keep_playing = false;
void save_outputfile(void);

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

SNDFILE   *outfile  = NULL;
SNDFILE   *infile   = NULL ;
SF_INFO   sfinfo, sfinfo_out;

const PaDeviceInfo *deviceInfo;
PaStreamParameters outputParameters;
PaStream *stream;
PaError err;
paTestData data = {0};
reverb_settings_s rs;
char    *infilename=NULL, *outfilename=NULL;

static void streamFinished( void* userData )
{
  paTestData *data = (paTestData *) userData;
  (void) data;
  printf( "Stream Completed\n");
  keep_playing = false;
  save_outputfile();
  finnish_moorer();
  file_selected(infilename);

}
void outputfile_selected(char *fname)
{
    if (infilename==NULL)
        return;

    outfilename = fname;
}

void save_outputfile(void)
{
    // Save audio
    if( NULL != outfilename) {
        // Copy audioinfo and create outfile
        memcpy(&sfinfo_out, &sfinfo, sizeof(SF_INFO));
        if ((outfile = sf_open(outfilename, SFM_WRITE, &sfinfo_out)) == NULL) {
            printf("Could not open '%s' for writing: %s\n", outfilename, sf_strerror(NULL));
            return ;
        }

        printf("Writing output to: %s\n", outfilename);
        sfinfo_out.frames = sfinfo.frames;
        sf_writef_float(outfile, samples, sfinfo_out.frames);
        sf_close(outfile);
        outfilename = NULL;
    }
}

    void file_selected(char *fname)
{
  if ((infile = sf_open (fname, SFM_READ, &sfinfo)) == NULL) {
    printf("Could not open '%s' for reading: %s\n", fname, sf_strerror(NULL));
    return ;
  };
  infilename = fname;

  outputParameters.channelCount = sfinfo.channels;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency = deviceInfo->defaultLowOutputLatency;
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.hostApiSpecificStreamInfo = NULL;


  printf("SampleRate: %d\n", sfinfo.samplerate);
  printf("Samples   : %lld\n", sfinfo.frames);

  // Read samples to array
  samples = malloc(sizeof(float) *  sfinfo.frames * sfinfo.channels);
  sf_readf_float(infile, samples, sfinfo.frames);
  sf_close (infile);
  data.samples = sfinfo.frames*sfinfo.channels;
  data.channels = sfinfo.channels;
  data.buffer = samples;

  init_moorer(samples, &sfinfo, FRAMES_PER_BUFFER, &rs);
}

void start_playback(void)
{

    Pa_AbortStream(stream);
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
    return;
  }

  err = Pa_SetStreamFinishedCallback( stream, streamFinished );
  err = Pa_StartStream( stream );
}

void pause_playback(bool is_paused)
{
  if(is_paused){
    err = Pa_StartStream( stream );
  } else {
    Pa_StopStream(stream);
  }
}

void stop_playback(void)
{
  err = Pa_StopStream( stream );
  err = Pa_CloseStream( stream );
}
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
  rs->rt60 = 3.5;
  rs->wetmix = 0.7;
  rs->reflect = 0.7;
  rs->damping = 0.7;
  rs->area = 0;
  rs->volume = 0;

    bool has_rt60 = false;
    bool has_volume_or_area = false;
    uint8_t i;
    for(i=2;i<argc; i++){
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
    rs->rt60 = get_rt60_from_volume_area(rs->volume, rs->area);
  }
  rs->earlyRD = rs->reflect;
  rs->lateRD = rs->reflect;
}

int main (int argc, char *argv[])
{

    uint32_t i;

    parse_settings(&rs, argc, argv);
    printf("rt60:\t %f \nearlyRD:\t %f \nlateRD:\t %f \n", rs.rt60, rs.earlyRD, rs.lateRD);
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
    deviceInfo = Pa_GetDeviceInfo(outputParameters.device);


    printf( "Device: %s\n", deviceInfo->name );
    memset (&sfinfo, 0, sizeof (sfinfo)) ;
  /* Initialisation of GTK+ */
    gtk_init(&argc, &argv);
    /* Creation of the window */
    GtkWidget * MainWindow = NULL;
    MainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    init_gui(MainWindow, &rs);
    //void init_gui(GtkWidget *MainWindow, reverb_settings_s *rs);


    gtk_widget_show_all(MainWindow);
    gtk_main();

  /*
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


*/


  Pa_Terminate();
  finnish_moorer();

  free(samples);
  return 0 ;
}
