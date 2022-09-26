#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <complex.h>
#include <fftw3.h>
#include <sndfile.h>

#include <math.h>

#include "../includes/gnuplot_i.h"

#define	FRAME_SIZE 1024
#define HOP_SIZE 1024
#define Fe 44100
#define MAX_FRAME 500

static gnuplot_ctrl *h;
static fftw_plan plan;

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

double freq_to_bark(double freq)
{
  if(freq<=500)
    return freq/100;
  else return 9+4*log2(freq/1000);
}

double audibilite(double freq)
{
    return  3.64 * pow( freq/1000, -0.8) -6.5*exp(-0.6*pow((freq/1000-3.3),2)) + pow(10,-3) * pow(freq/1000,4);
}

double amplitude_to_db(double amp)
{
    return 20*log10(amp/pow(10, -6));
}

double db_to_amplitude(double db)
{
    return pow(10, -6)*pow(10, db/20);
}

double energy_for_frame(double frame[])
{
    double e=0;
    for(int i=0; i<FRAME_SIZE; i++)
    {
        e+= frame[i]*frame[i];
    }
    return (e/FRAME_SIZE);
}

double filter_pinnacles(double h[], int threshold)
{
    for(int i=0; i<MAX_FRAME; i++){
        if(h[i] > h[i+1] && h[i] > h[i-1] && h[i] > threshold)
            h[i] = h[i]
        else h[i]=0;
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
    double decibel[FRAME_SIZE];
	complex spectrum[FRAME_SIZE];
	double x_freq[FRAME_SIZE];
    double audi[FRAME_SIZE];
    double energy_per_frame[MAX_FRAME];
	
	/* FFT init */
	fft_init(samples, spectrum);
		
	while (read_samples (infile, new_buffer, sfinfo.channels)==1)
	  {
	    /* Process Samples */
	    printf("Processing frame %d\n",nb_frames);

	    /* hop size */
	    fill_buffer(buffer, new_buffer);

	    // fft process
	    for (i=0; i < FRAME_SIZE; i++)
	      {
		// Fenetre Hann
		//samples[i] = buffer[i]*(0.5-0.5*cos(2.0*M_PI*(double)i/FRAME_SIZE));
		// Fenetre rect
		samples[i] = buffer[i];
	      }
		    
	    fft_process();

	    for (i=0; i < FRAME_SIZE; i++)
	      {
		amplitude[i] = cabs(spectrum[i]);
	      }
        energy_per_frame[nb_frames] = energy_for_frame(amplitude);
	    /* frequence */
        for(int i=0; i < FRAME_SIZE; i++)
        {
            decibel[i] = amplitude_to_db(amplitude[i]);
        }
	    /* DB */

	    /* Barks */
		for(int i=0; i < FRAME_SIZE; i++)
        {
            //Transformation de l'indice en Hz.
            double freq = i*Fe/FRAME_SIZE;
            x_freq[i] = freq;
        }  
	    /* Seuil audibilite */
		for(int i=0; i<FRAME_SIZE; i++)
        {
            audi[i] = audibilite(x_freq[i]);
        }
        for(int i=0; i<FRAME_SIZE; i++)
        {
            x_freq[i] = freq_to_bark(x_freq[i]);
        }

	    /* plot amplitude */
	    gnuplot_resetplot(h);
	    //gnuplot_plot_x(h,decibel,FRAME_SIZE/2,"amplitude");
	    //gnuplot_plot_xy(h,x_freq,audi,FRAME_SIZE/2,"amplitude");
        //gnuplot_plot_xy(h,x_freq,decibel,FRAME_SIZE/2,"amplitude");
	    //sleep(1); 
	    nb_frames++;
	  }

	sf_close (infile) ;
    gnuplot_plot_x(h, energy_per_frame, nb_frames, "energy");
    sleep(10);
	/* FFT exit */
	fft_exit();
	
	return 0 ;
} /* main */
