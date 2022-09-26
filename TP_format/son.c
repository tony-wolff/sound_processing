#include "gnuplot_i.h"
#include <math.h>
#include <complex.h>
#include <fftw3.h>

#define SAMPLING_RATE 44100.0
#define CHANNELS_NUMBER 1
#define N 1024
#define NB_BITS 16

char *RAW_FILE = "tmp-in.raw";

void fft(double s[], fftw_complex data_in[], fftw_plan plan)
{
	for(int i=0; i<N; i++){
		data_in[i] = s[i];
	}
	fftw_execute(plan);
}

FILE *
sound_file_open_read (char *sound_file_name)
{
  char cmd[256];

  snprintf (cmd, 256,
	    "sox %s -b %d -c %d -r %d %s",
	    sound_file_name,
      NB_BITS,
	    CHANNELS_NUMBER, (int)SAMPLING_RATE, RAW_FILE);

  system (cmd);

  return fopen (RAW_FILE, "rb");
}

void
sound_file_close_read (FILE *fp)
{
  fclose (fp);
}

int
sound_file_read (FILE *fp, double *s)
{
    short tmp[N];
    // En octet d'où le nb_bits/8. On mets dans s n valeur qui font 2 octets
    int nb_read = fread(tmp, (NB_BITS/8), N, fp);
    for(int i=0; i<N; i++){
          //On normalise la représentation informatique.
          s[i]=(double) tmp[i]/pow(2, NB_BITS-1);
      }
    return (nb_read == N);//Retourne 1 si true
}

void cartesian_to_polar (complex s[], double amp[], double phs[])
{
  for(int i=0; i<N/2; i++){
    double norm = cabs(s[i]);
    double phase = carg(s[i]);
    amp[i] = norm*2;
    phs[i] = phase*2;
  }
}

int
main (int argc, char *argv[])
{
  FILE *input;
  double s[N];
  static fftw_plan plan;
  fftw_complex data_in[N];
  fftw_complex data_out[N];
  double freq[N/2];
  double phs[N/2];
  plan = fftw_plan_dft_1d(N, data_in, data_out, FFTW_FORWARD, FFTW_ESTIMATE);
  //on utilise tmp pour store des entiers signés sur 2 octets.
  
  gnuplot_ctrl *h = gnuplot_init();
  gnuplot_setstyle(h, "lines");
  gnuplot_cmd(h, "set yr [-1:1]");

  input = sound_file_open_read(argv[1]);
  double t[N];
  for(int i=0; i<N; i++){
    t[i] = i/SAMPLING_RATE;
  }
  double resolution[N/2];
  for(int i=0; i<N/2; i++){
    resolution[i] = i*SAMPLING_RATE/N;
  }

  while(sound_file_read (input, s))
    {
      fft(s, data_in, plan);
      cartesian_to_polar(data_out, freq, phs);
      // affichage
      gnuplot_resetplot(h);
      //gnuplot_plot_x(h, s, N, "machin");
      gnuplot_plot_xy(h, resolution, freq, N, "représentation spectrale");
      //gnuplot_plot_xy(h, t, s, N, "représentation des amplitudes temporelles");
      usleep(100000);
    }
  
  sound_file_close_read (input);
  exit (EXIT_SUCCESS);
}