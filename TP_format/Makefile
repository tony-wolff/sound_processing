CFLAGS = -O3 -I. -Wall -lm

all : son
son : son.o gnuplot_i.o
	gcc -c son.c gnuplot_i.c
	gcc $(CFLAGS) son.o gnuplot_i.o -o son -g -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lfftw3 -lm 

clean : 
	rm -f son son.o gnuplot_i.o