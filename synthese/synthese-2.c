#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FE 44100
#define DUREE 5
#define N DUREE*FE

/* output */
static char *RAW_OUT = "tmp-out.raw";
static char *FILE_OUT = "out2.wav";

FILE *
sound_file_open_write (void)
{
 return fopen (RAW_OUT, "wb");
}

void
sound_file_close_write (FILE *fp)
{
  char cmd[256];
  fclose (fp);
  snprintf(cmd, 256, "sox -c 1 -r %d -e signed-integer -b 16 %s %s", (int)FE, RAW_OUT, FILE_OUT);
  system (cmd);
}

void
sound_file_write (double *s, FILE *fp)
{
  int i;
  short tmp[N];
  for (i=0; i<N; i++)
    {
      tmp[i] = (short)(s[i]*32768);
    }
  fwrite (tmp, sizeof(short), N, fp);
}

void additive(double *s, float f1, float f2, float amp1, float amp2, int phi)
{
    float freqmax=0;
    for (int i=0; i<N; i++){
        s[i] = amp1 * sin(2*M_PI*f1*i*1/FE+phi) + amp2 * sin(2*M_PI*f2*i*1/FE+phi);
        if(s[i]>freqmax)
            freqmax=s[i];
    }
    for(int i=0; i<N; i++)
        s[i]/=freqmax;
}

//C contrôle l'amplitude de la porteuse
void combinationAM(double *s, float c, float amp2, float f1, float f2)
{
    for(int i=0; i<N; i++)
    {
        s[i] = (amp2*sin(2*M_PI*f1*i*1/FE)+c) * sin(2*M_PI*f2*i*1/FE);
    }
}

void combinationFM(double *s, float c, float amp1, float amp2, float f2)
{
    for(int i=0; i<N; i++)
    {
        float t = i*1/FE;
        //s[i] = amp1 * sin(2*M_PI*(c+amp2*sin(2*M_PI*f2*i*1/FE))*i*1/FE);
        s[i] = amp1*t*sin(2*M_PI*f1*t+I+I*t*sin(2*M_PI*f2*t*t));
    }
}

void osc(float amp, float f, double *signal)
{   
    for(int i=0; i<N; i++)
    {
        signal[i]= amp * sin(2*M_PI*f*i*1/FE);
    }
}

int
main (int argc, char *argv[])
{
    FILE *output;
    double s[N];
    float f1 = 200;
    float amp = 0.2;
    float amp2 = 0.3;
    float f2 = 400;
    int phi = 0;
    if (argc != 1)
    {
        printf ("usage: %s\n", argv[0]);
        exit (EXIT_FAILURE);
    }
    //additive(s, f1, f2, amp, amp2, phi);
    //combinationAM(s, 0.3, amp2, f1, f2);
    combinationFM(s, 0.7, amp, amp2, f2);
    output = sound_file_open_write ();

    sound_file_write (s, output);
    sound_file_close_write (output);
    exit (EXIT_SUCCESS);
}