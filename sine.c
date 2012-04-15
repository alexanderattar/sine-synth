/*
 * =====================================================================================
 *
 *       Filename:  sine.c
 *
 *    Description:  Support File for sinePlayer.c (based on Mary's code)
 *
 *        Version:  1.0
 *        Created:  11/21/2011 01:52:12
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Alexander Attar
 *     University:  New York University
 *    
 *
 * =====================================================================================
 */

#define PI 3.14159265358979323846264338327950288
#define AMPLITUDE 1.0
#define FREQUENCY 440
#define SAMPLE_RATE 44100
#define TABLE_SIZE SAMPLE_RATE
#define NUM_SECONDS 30
#define FRAMES_PER_BUFFER 1024
#define MONO 1
#define STEREO 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for sleep() */
#include <portaudio.h>
#include <math.h>
#include <sndfile.h>
#include <string.h> /* for memset */
#include <ncurses.h> /* This library is for getting input without hitting return */


typedef struct {
    float sampleRate;
    SNDFILE *infile;
    SF_INFO sfinfo;
} paData;

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  paCallback
 *  Description:  Callback for Port Audio
 * =====================================================================================
 */
static int paCallback( const void *inputBuffer,
			 void *outputBuffer, unsigned long framesPerBuffer,
			 const PaStreamCallbackTimeInfo* timeInfo,
			 PaStreamCallbackFlags statusFlags, void *userData )  {

    float *out = (float*) outputBuffer;
    float buffer[framesPerBuffer * STEREO];
    static int phase = 0;
    float sample;
    int i, readcount;
    paData *data = (paData*)userData;

    readcount = sf_readf_float( data->infile, buffer, framesPerBuffer );

    if ( readcount < framesPerBuffer ) {
        sf_seek( data->infile, 0, SEEK_SET );
        readcount = sf_readf_float( data->infile, buffer + (readcount*STEREO), framesPerBuffer - readcount );
    }

    for ( i = 0; i < framesPerBuffer; i++ ) {
        sample = AMPLITUDE * sin( 2 * PI * FREQUENCY * phase / (float) SAMPLE_RATE );
        *out++ = sample + buffer[2*i];    // left
        *out++ = sample + buffer[2*i+1];    // right
        phase++;
        phase %= SAMPLE_RATE;
    }

    return paContinue;
}
/* -----  end of function paCallback  ----- */

int main( int argc, char **argv ) {

    PaStream *stream;
    PaStreamParameters outputParameters;
    PaError err;
    paData data;

    /* Initialize PortAudio */
    Pa_Initialize();

    /* Set output stream parameters */
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = STEREO;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = 
        Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* Open the audio file */
    if (( data.infile = sf_open( argv[1], SFM_READ, &data.sfinfo )) == NULL ) {
        printf("Error, couldn't open file\n");
        exit(1);
    }

    /* Print audio file info */
    printf("Audio File: \nFrames: %d\nChannels: %d\nSamplerate: %d\n",
            (int)data.sfinfo.frames, data.sfinfo.channels, data.sfinfo.samplerate);

    /* Open audio stream */
    err = Pa_OpenStream( &stream, NULL /* no input */,
		       &outputParameters,
		       SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag, 
		       paCallback, &data );

    if (err != paNoError) {
        printf("PortAudio error: open stream: %s\n", Pa_GetErrorText(err));
    }
  
    /* Start audio stream */
    err = Pa_StartStream( stream );
    if (err != paNoError) {
        printf(  "PortAudio error: start stream: %s\n", Pa_GetErrorText(err));
    }

    /* Play sine wav */
    printf("Play for %d seconds.\n", NUM_SECONDS );
    sleep(NUM_SECONDS);
    err = Pa_StopStream( stream );

    /* Close audio file */
    sf_close( data.infile );

    /* Stop audio stream */
    if (err != paNoError) {
        printf(  "PortAudio error: stop stream: %s\n", Pa_GetErrorText(err));
    }
    /* Close audio stream */
    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        printf("PortAudio error: close stream: %s\n", Pa_GetErrorText(err));
    }
    /* Terminate audio stream */
    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
    }
  
    return 0;
}



