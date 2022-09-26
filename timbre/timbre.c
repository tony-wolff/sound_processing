#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <complex.h>
#include <fftw3.h>
#include <sndfile.h>

#include <math.h>

#include "../includes/gnuplot_i.h"

#define	FRAME_SIZE 2048
#define HOP_SIZE 2048
#define ORDER 50

static gnuplot_ctrl *h;
static fftw_plan plan;
static fftw_plan iplan;

static void
print_usage (char *progname)
{	printf ("\nUsage : %s <input file> \n", progname) ;
	puts ("\n"
		) ;

} 
static void
fill_buffer(double *buffer, double *new_buffer)
{
  int i;
  double tmp[FRAME_SIZE-HOP_SIZE];
  
  /* save */
  for (i=0;i<FRAME_SIZE-HOP_SIZE;i++)
    tmp[i] = buffer[i+HOP_SIZE];
  
  /* save offset */
  for (i=0;i<(FRAME_SIZE-HOP_SIZE);i++)
    {
      buffer[i] = tmp[i];
    }
  
  for (i=0;i<HOP_SIZE;i++)
    {
      buffer[FRAME_SIZE-HOP_SIZE+i] = new_buffer[i];
    }
}

void amplitude_to_decibel(double amp[], double dec[])
{
    for(int i=0; i<FRAME_SIZE; i++)
    {
        dec[i] = 20*log10(amp[i]/pow(10, -6));
    }
}

static int
read_n_samples (SNDFILE * infile, double * buffer, int channels, int n)
{

  if (channels == 1)
    {
      /* MONO */
      int readcount ;

      readcount = sf_readf_double (infile, buffer, n);

      return readcount==n;
    }
  else if (channels == 2)
    {
      /* STEREO */
      double buf [2 * n] ;
      int readcount, k ;
      readcount = sf_readf_double (infile, buf, n);
      for (k = 0 ; k < readcount ; k++)
	buffer[k] = (buf [k * 2]+buf [k * 2+1])/2.0 ;

      return readcount==n;
    }
  else
    {
      /* FORMAT ERROR */
      printf ("Channel format error.\n");
    }
  
  return 0;
} 

static int
read_samples (SNDFILE * infile, double * buffer, int channels)
{
  return read_n_samples (infile, buffer, channels, HOP_SIZE);
}

void
fft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE])
{
  plan = fftw_plan_dft_1d(FRAME_SIZE, in, spec, FFTW_FORWARD, FFTW_ESTIMATE);
}

void
fft_exit (void)
{
  fftw_destroy_plan(plan);
}

void
fft_process (void)
{
  fftw_execute(plan);
}

void
ifft_init (complex in[FRAME_SIZE], complex spec[FRAME_SIZE])
{
    iplan = fftw_plan_dft_1d(FRAME_SIZE, in, spec, FFTW_BACKWARD, FFTW_ESTIMATE);
}

void
ifft_exit (void)
{
    fftw_destroy_plan(iplan);
}

void
ifft_process (void)
{
    fftw_execute(iplan);
}

void filtre_passe_bas(complex spectrum[], complex samples[])
{
    int hpb[FRAME_SIZE] = {0};
    for(int i=0; i<FRAME_SIZE; i++){
        if(i == 0 || i == ORDER/2)
            hpb[i]=0;
        else if(i >= 1 && i < ORDER/2)
            hpb[i]=1;
        else hpb[i]=0;
        
    }

    for(int i=0; i<FRAME_SIZE; i++)
    {
      samples[i] = spectrum[i]*hpb[i];
    }
}


int
main (int argc, char * argv [])
{	char 		*progname, *infilename;
	SNDFILE	 	*infile = NULL ;
	SF_INFO	 	sfinfo ;

	progname = strrchr (argv [0], '/') ;
	progname = progname ? progname + 1 : argv [0] ;

	if (argc != 2)
	{	print_usage (progname) ;
		return 1 ;
		} ;

	infilename = argv [1] ;

	if ((infile = sf_open (infilename, SFM_READ, &sfinfo)) == NULL)
	{	printf ("Not able to open input file %s.\n", infilename) ;
		puts (sf_strerror (NULL)) ;
		return 1 ;
		} ;

	/* Read WAV */
	int nb_frames = 0;
	double new_buffer[HOP_SIZE];
	double buffer[FRAME_SIZE];

	/* Plot Init */
	h=gnuplot_init();
	gnuplot_setstyle(h, "lines");

	
	int i;
	for (i=0;i<(FRAME_SIZE/HOP_SIZE-1);i++)
	  {
	    if (read_samples (infile, new_buffer, sfinfo.channels)==1)
	      fill_buffer(buffer, new_buffer);
	    else
	      {
		printf("not enough samples !!\n");
		return 1;
	      }
	  }

	complex samples[FRAME_SIZE];
	double amplitude[FRAME_SIZE];
	complex spectrum[FRAME_SIZE];
    double decibel[FRAME_SIZE];
    complex samples2[FRAME_SIZE];
	/* FFT init */
    fft_init(samples, spectrum);
    ifft_init(samples2, spectrum);
	
	while (read_samples (infile, new_buffer, sfinfo.channels)==1)
	  {
	    /* Process Samples */
	    printf("Processing frame %d\n",nb_frames);

	    /* hop size */
	    fill_buffer(buffer, new_buffer);

	    // fft process
	    for (i=0; i < FRAME_SIZE; i++)
	      {
		// Fenetre rect
		    samples[i] = buffer[i];
	      }
        fft_process();
        for (i=0; i < FRAME_SIZE; i++)
	      {
		    amplitude[i] = cabs(spectrum[i]);
	      }
	    amplitude_to_decibel(amplitude, decibel);

	    /* TODO */
        for(i=0; i< FRAME_SIZE; i++)
        {
            samples2[i]=decibel[i];
        }
        ifft_process();
        filtre_passe_bas(spectrum, samples);
        fft_process();
        for(i=0; i< FRAME_SIZE; i++)
        {
          amplitude[i] = creal(spectrum[i])/FRAME_SIZE;
        }

	    /* plot amplitude */
	    gnuplot_resetplot(h);
	    gnuplot_plot_x(h,decibel,FRAME_SIZE/2,"enveloppe fréquentielle");
      gnuplot_plot_x(h,amplitude,FRAME_SIZE/2,"db");
	    sleep(1);

	    nb_frames++;
	  }

	sf_close (infile) ;

	/* FFT exit */

	
	return 0 ;
} /* main */
