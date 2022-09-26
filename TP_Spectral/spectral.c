#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <sndfile.h>

#include <math.h>
#include <complex.h>

#include "gnuplot_i.h"
#include <fftw3.h>

/* taille de la fenetre */
#define	FRAME_SIZE 1024
#define FFT_SIZE 1024
/* avancement */
#define HOP_SIZE 1024

static gnuplot_ctrl *h;


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

/* O(n²), quadratique, c'est mauvais*/
void dft(double s[], complex S[])
{
	complex sum=0;
	for (int m=0; m<FRAME_SIZE; m++){
		sum=0;
		for(int n=0; n<FRAME_SIZE; n++){
			sum += s[n]*cexp(-I*(2*M_PI/FRAME_SIZE)*n*m);
		}
		S[m]=sum; 
	}
}

void fft(double s[], fftw_complex data_in[], fftw_plan plan)
{
	for(int i=0; i<FRAME_SIZE; i++){
		data_in[i] = s[i];
	}
	for(int i=FRAME_SIZE; i<FFT_SIZE; i++){
		data_in[i]=0;
	}
	fftw_execute(plan);
}

void cartesian_to_polar (complex S[], double amp[], double phs [])
{
	for(int i=0; i < FRAME_SIZE; i++){
		double norm = cabs(S[i]);
		double phase = carg(S[i]);
		amp[i] = norm;
		phs[i] = phase;
	}
}
int index_for_max_amp(double amp[]){
	int max=-1;
	int id=0;
	for(int i=0; i<FRAME_SIZE/2; i++){
		if(amp[i]>max)
		{
			max=amp[i];
			id=i;
		}
	}
	printf("max amp %f\n", (float)max/(FRAME_SIZE/2));
	return id;
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
	complex spectrum[FRAME_SIZE];
	double buffer[FRAME_SIZE];
	double amp[FRAME_SIZE];
	double phs[FRAME_SIZE];
	double hann[FRAME_SIZE];

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

	/* Info file */
	printf("sample rate %d\n", sfinfo.samplerate);
	printf("channels %d\n", sfinfo.channels);
	printf("size %d\n", (int)sfinfo.frames);
	for(int i=0; i<FRAME_SIZE; i++){
		hann[i] = 0.5-0.5*cos(2*M_PI*(double)i/FRAME_SIZE);
	}

	static fftw_plan plan;
	fftw_complex data_in[FFT_SIZE];
	fftw_complex data_out[FFT_SIZE];
	plan = fftw_plan_dft_1d(FFT_SIZE, data_in, data_out, FFTW_FORWARD, FFTW_ESTIMATE);
	clock_t t1, t2;
	double delta_t;	
	t1 = clock();
	while (read_samples (infile, new_buffer, sfinfo.channels)==1)
	  {
	    /* Process Samples */
	    printf("Processing frame %d\n",nb_frames);
	    /* hop size */
	    fill_buffer(buffer, new_buffer);
/* 		for(int i=0; i<FRAME_SIZE; i++){
			buffer[i] = buffer[i]*hann[i];
		} */
		//dft(buffer, spectrum);
		fft(buffer, data_in, plan);
		cartesian_to_polar(data_out, amp, phs);
		int m = index_for_max_amp(amp);
		double al = 20*log10(amp[m-1]);
		double ac = 20*log10(amp[m]);
		double ar = 20*log10(amp[m+1]);
		double d = 0.5*(al-ar/(al-2*ac+ar));
		printf("fréq %f\n", (m+d)*sfinfo.samplerate/FRAME_SIZE); 
		printf("Hz with max amp: %d\n", index_for_max_amp(amp));
		printf("s[k]: %f + i%f\n", creal(data_out[nb_frames]), cimag(data_out[nb_frames]));
		printf("s[frame_size-k]: %f + i%f\n", creal(data_out[FRAME_SIZE-nb_frames]), cimag(data_out[FRAME_SIZE-nb_frames]));
	    /* PLOT */
	    gnuplot_resetplot(h);
	    //gnuplot_plot_x(h, buffer,FRAME_SIZE,"temporal frame");
		gnuplot_plot_x(h, amp, FRAME_SIZE, "frequencial frame");
		sleep(1);
    
	    nb_frames++;
	  }
	t2 = clock();
	delta_t = t2-t1;
	printf("temps écoulé : %f secondes", delta_t / CLOCKS_PER_SEC);
	fftw_destroy_plan(plan);
	sf_close (infile) ;

	return 0 ;
} /* main */

