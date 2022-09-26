#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FE 44100
#define DUREE 60
#define N DUREE*FE
#define FRAME_SIZE 1024

/* output */
static char *RAW_OUT = "tmp-out.raw";
static char *FILE_OUT = "out.wav";

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
  snprintf(cmd, 256, "sox -c1 -r %d -e signed-integer -b 16 %s %s", (int)FE, RAW_OUT, FILE_OUT);
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

int
main (int argc, char *argv[])
{
  int i;
  FILE *output;
  double s[N];

  if (argc != 1)
    {
      printf ("usage: %s\n", argv[0]);
      exit (EXIT_FAILURE);
    }
    output = sound_file_open_write ();
    float freq = 300;
    float freq2 = 600;
    float amplitude = 10;
    int phi = 0;
  /*  Stéréo WIP, ca marche pas avec la commande sox
  for (i=0; i<N; i+=2)
    {
        s[i] = amplitude * sin(2*M_PI*freq*i*1/FE+phi);
        s[i+1] = amplitude * sin(2*M_PI*freq2*i*1/FE+phi);
    }*/
    
    //altération au fil du temps;
    /*
    for(i=0; i<N; i++)
    {
        freq+=0.1;
        s[i] = amplitude * sin(2*M_PI*freq*i*1/FE+phi);
    }*/
    //Synthèse additive
    /*float freq_max = 0;
    for(i=0; i<N; i++)
    {
        s[i] = amplitude *sin(2*M_PI*freq*i*1/FE+phi) + amplitude*sin(2*M_PI*freq2*1/FE+phi);
        if(s[i]>freq_max)
            freq_max = s[i];
    }
    for(i =0;i<N; i++)
    {
        s[i]/=freq_max;
    }*/
    int cpt=0;
    while(cpt*FRAME_SIZE*1.0/FE < DUREE)
    {
        //Incrément de phase à effectuer à chaque trame
        float delta_phi = M_PI*freq*N/FE;
        for(i = 0; i<FRAME_SIZE; i++)
        {
            s[i] = amplitude*sin(2*M_PI*freq*i*1/FE+delta_phi*cpt);
        }
        sound_file_write (s, output);
        cpt++;
    }
  sound_file_close_write (output);
  exit (EXIT_SUCCESS);
}