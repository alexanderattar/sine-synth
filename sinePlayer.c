/*****************************************************************************
 * Alexander Attar - Fall 2011
 *
 * sinePlayer.c
 * 
 * 
 * Allows the user to "play" two differen audio channels by hitting
 * keys corresponding to fixed-pitch frequencies that generate sine waves.
 * Each channel is mapped to the A major scale; channel 1 has pitches exactly
 * an octave below channel 2.
 *
 * Channel 1 mappings:
 * Keys [asdf] correspond to A3, B3, C#3, D4
 * Keys [qwer] correspond to E4, F#4, G#4, A4
 * [v] silences the channel
 *
 * Channel 2 mappings:
 * Keys [jkl;] correspond to A4, B4, C#4, D5
 * Keys [uiop] correspond to E5, F#5, G#5, A5
 * [n] silences the channel
 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with this program. 
 * If not, see <http://www.gnu.org/licenses/>
 *
 *****************************************************************************/
#define PI 3.14159265358979323846264338327950288
#define INITIAL_AMPLITUDE 1 
#define SAMPLE_RATE 44100
#define TABLE_SIZE SAMPLE_RATE
#define FRAMES_PER_BUFFER 256
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

///////////////////////////////////////////////////////////////////////////////////
                          /* Global Data Variables */

typedef struct {
    float amplitude1;
    float amplitude2;
    char *pitch1;
    char *pitch2;
    float frequency1;
    float frequency2;
    char *volume1;
    char *volume2;
    float sampleRate;
} paData;

static int paCallback(const void *inputBuffer,
            void *outputBuffer, unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags, void *userData);


///////////////////////////////////////////////////////////////////////////////

/*  MAIN FUNCTION */
int main(int argc, char **argv)
{
  PaStream *stream;
  PaStreamParameters outputParameters;
  PaError err;
  paData data; 

  /*  INITIALIZE PORTAUDIO */ 
  err = Pa_Initialize();
  if (err != paNoError) {
    printf("PortAudio error: %s\n", Pa_GetErrorText(err));
    printf("\nExiting.\n");
    exit(1);
  }

  //set output stream parameters
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputParameters.channelCount = STEREO;
  outputParameters.sampleFormat = paFloat32;
  outputParameters.suggestedLatency =
  Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  //Open audio stream
  err = Pa_OpenStream(&stream,
        NULL /* no input */,
        &outputParameters,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paNoFlag,
        paCallback,
        &data);
  
  if (err != paNoError) {
    printf("PortAudio error: open stream: %s\n", Pa_GetErrorText(err));
    printf("\n Exiting. \n");
    exit(1);
  }

  //start audio stream
  err = Pa_StartStream(stream);
  if (err != paNoError) {
    printf("PortAudio error: start stream: %s\n", Pa_GetErrorText(err));
  }

  /*  MAIN LOOP  */
  //Initialize interactive character input
  initscr(); //start cures mode
  cbreak(); //line buffering disabled 
  noecho(); //prevents characters from diplaying on the screen

  char ch;
  ch = '\0'; //init ch to null character
  
  //Initialize Audio Parameters
   data.sampleRate = SAMPLE_RATE;
   data.amplitude1 = INITIAL_AMPLITUDE;
   data.amplitude2 = INITIAL_AMPLITUDE;

  //Initialize Pitch Names
  data.pitch1 = "";
  data.pitch2 = "";

  //Initialize Volume Level
  data.volume1 = "ON";
  data.volume2 = "ON";

  data.amplitude1 = 1;
  data.amplitude2 = 1;

  data.frequency1 = 0;
  data.frequency2 = 0;

  /*  Print Display  */
  mvprintw(0,0, " Channel 1 Pitch: %s Channel 2 Pitch: %s\n"
 "Volume Left: %s Volume Right: %s \n\n"
 "Channel 1 mappings:\n"
 "Keys [asdf] correspond to A3, B3, C#3, D4\n"
 "Keys [qwer] correspond to E4, F#4, G#4, A4\n"
 "[v] silences the channel\n\n"
 
 "Channel 2 mappings:\n"
 "Keys [jkl;] correspond to A4, B4, C#4, D5\n"
 "Keys [uiop] correspond to E5, F#5, G#5, A5\n"
 "[n] silences the channel\n\n"
 "[spacebar] to quit\n", data.pitch1, data.pitch2, data.volume1, data.volume2);

  /*  Create switch cases for the different tones */
  while (ch != ' ') {
    ch = getch(); 

    switch (ch) {
      case 'a':
        data.frequency1 = 220.0; //A3
        data.pitch1 = "A3";
        break;
      case 's':
        data.frequency1 = 246.9; //B3
        data.pitch1 = "B3";
        break;
      case 'd':
        data.frequency1 = 277.2; //C#4
        data.pitch1 = "C#4";
        break;
      case 'f':
        data.frequency1 = 293.7; //D4
        data.pitch1 = "D4";
        break;
      case 'q':
        data.frequency1 = 329.6; //E4 
        data.pitch1 = "E4";
        break;
      case 'w':
        data.frequency1 = 370.0; //F#4
        data.pitch1 = "F#4";
        break;
      case 'e':
        data.frequency1 = 415.3; //G#4
        data.pitch1 = "G#4";
        break;
      case 'r':
        data.frequency1 = 440.0; //A4
        data.pitch1 = "A4";
        break;
      case 'j':
        data.frequency2 = 440.0; //A4
        data.pitch2 = "A4";
        break;
      case 'k':
        data.frequency2 = 493.9; //B4
        data.pitch2 = "B4";
        break;
      case 'l':
        data.frequency2 = 554.4; //C#5
        data.pitch2 = "C#5";
        break;
      case ';':
        data.frequency2 = 587.3; //D5
        data.pitch2 = "D5";
        break;
      case 'u':
        data.frequency2 = 659.3; //E5
        data.pitch2 = "E5";
        break;
      case 'i':
        data.frequency2 = 740.0; //F#5
        data.pitch2 = "F#5";
        break;
      case 'o':
        data.frequency2 = 830.6; //G#5
        data.pitch2 = "G#5";
        break;
      case 'p':
        data.frequency2 = 880.0; //A5
        data.pitch2 = "A5";
        break;

      /* user can turn volume of each channel On and Off */
      case 'v':
          if (data.amplitude1 == 1) {
          data.amplitude1 = 0;
          data.volume1 = "OFF";
          }
          else {
          data.amplitude1 = 1;
          data.volume1 = "ON";
          }
          break;
      case 'n':
          if (data.amplitude2 == 1) {
          data.amplitude2 = 0;
          data.volume2 = "OFF";
          }
          else {
          data.amplitude2 = 1;
          data.volume2 = "ON";
          }
          break;
    }

    mvprintw(0,0, " Channel 1 Pitch: %s Channel 2 Pitch: %s\n"
   "Volume Left: %s Volume Right: %s \n\n"
   "Channel 1 mappings:\n"
   "Keys [asdf] correspond to A3, B3, C#3, D4\n"
   "Keys [qwer] correspond to E4, F#4, G#4, A4\n"
   "[v] silences the channel\n\n"
   
   "Channel 2 mappings:\n"
   "Keys [jkl;] correspond to A4, B4, C#4, D5\n"
   "Keys [uiop] correspond to E5, F#5, G#5, A5\n"
   "[n] silences the channel\n\n"
   "[spacebar] to quit\n", data.pitch1, data.pitch2, data.volume1, data.volume2);

  }

  //end curses mode
  endwin();

  //Stop stream
  err = Pa_StopStream(stream);
  if (err != paNoError) {
    printf("PortAudio error: stop stream: %s\n", Pa_GetErrorText(err));
  }

  //Close stream
  err = Pa_CloseStream(stream);
  if (err != paNoError) {
    printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
  }

  //Terminate PortAudio
  err = Pa_Terminate();
  if (err != paNoError) {
    printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
  }
  return 0;
  
} //END MAIN

static int paCallback(const void *inputBuffer,
            void *outputBuffer, unsigned long framesPerBuffer,
            const PaStreamCallbackTimeInfo* timeInfo,
            PaStreamCallbackFlags statusFlags, void *userData) 
{
  paData *data = (paData*)userData;
  float *out = (float*) outputBuffer;
  static int phase1 = 0;
  static int phase2 = 0;
  int i;

  for (i = 0; i < framesPerBuffer; i++) {
    *out++ = data->amplitude1 * sin(2 * PI * data->frequency1 * phase1/(float) SAMPLE_RATE); 
    *out++ = data->amplitude2 * sin(2 * PI * data->frequency2 * phase2/(float) SAMPLE_RATE); 
    phase1++;
    phase2++;
  }
    return paContinue;
}
