#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FE 44100
#define DUREE 10
#define N DUREE*FE

/* output */
static char *RAW_OUT = "tmp-out.raw";
static char *FILE_OUT = "out.wav";
int amplitude =0;
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
  snprintf(cmd, 256, "sox -c 1 -r %d -b16 -e signed-integer %s %s", (int)FE, RAW_OUT, FILE_OUT);
  system (cmd);
}

void
sound_file_write (double *s, FILE *fp)
{
  int i;
  short tmp[N];
  for (i=0; i<N; i++)
    {
      tmp[i] = (short)(s[i]*32765);
    }
  fwrite (tmp, sizeof(short), N, fp);
}

void
silence(double* s, FILE* output)
{
  int i;

  for(i=0;i<N;i++)
    s[i] = 0;

  sound_file_write (s, output);
}

void
note_shepard(double* s, double f, FILE* output)
{
    for(int i=0; i<N; i++)
    {
        for(int a = 0; a<9; a++)
        {
            //Formule de wikipedia
            //Synthèse additive de 9 sinus dont la fréquence se contrôle avec "a"
            //on multiplie par 0.1 pour ne pas dépasser 1 dans la somme.
            s[i] += 0.1*sin(2*M_PI*pow(2, a)*(f/8)*i*1/FE);
        }
    }
  sound_file_write (s, output);
}

void binaural(double *s, double f, double f2, FILE* output)
{
    for(int i=0; i<N; i+=2)
    {
        s[i]= sin(2*M_PI*f*i/FE);
        s[i+1] = sin(2*M_PI*(f2)*i/FE);
    }
    sound_file_write (s, output);
}

void
gamme_shepard_12_up(double* s, FILE* fo)
{
  note_shepard(s, 261.626, fo); // C4
  silence(s, fo);
  
  note_shepard(s, 277.183, fo); // C#4
  silence(s, fo);
  
  note_shepard(s, 293.665, fo); // D4
  silence(s, fo);           
  
  note_shepard(s, 311.127, fo); // D#4
  silence(s, fo);
  
  note_shepard(s, 329.628, fo); // E4
  silence(s, fo);
  
  note_shepard(s, 349.228, fo); // F4
  silence(s, fo);           
  
  note_shepard(s, 369.994, fo); // F#4
  silence(s, fo);
  
  note_shepard(s, 391.995, fo); // G4
  silence(s, fo);           
  
  note_shepard(s, 415.305, fo); // G#4
  silence(s, fo);
  
  note_shepard(s, 440, fo); // A4
  silence(s, fo);       
  
  note_shepard(s, 466.164, fo); // A#4
  silence(s, fo);
  
  note_shepard(s, 493.883, fo); // B4
  silence(s, fo);
}

int
main (int argc, char *argv[])
{
  //int i;
  double s[N];
  FILE* fo = sound_file_open_write ();
  
/*   for(i=0;i<4;i++)
    {
      gamme_shepard_12_up(s, fo);
    } */

    for(int i=0; i<4; i++)
    {
        //Deuxième fréquence avec accélération
        binaural(s, 100, 100+i, fo);
    }
  sound_file_write (s, fo);
  sound_file_close_write (fo);
  exit (EXIT_SUCCESS);
}