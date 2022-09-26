#--------------------------------
# GENERAL VARIABLES
#--------------------------------

CC = gcc 
CFLAGS = -O3 -I. -Wall -lm #-pg -g

#--------------------------------
# LIBRARY SOURCES
#--------------------------------

LIB_SRC =  ../includes/gnuplot_i.c 
LIB_OBJ = $(LIB_SRC:.c=.o)
EXECS = shepard

all: $(EXECS)

synthese : $(LIB_OBJ) $(LIB_SRC) shepard.c
	$(CC) $(CFLAGS) $(LIB_OBJ) shepard.c -o shepard -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lfftw3 -lm 

clean : 
	rm -f *.o 

cleanall :
	rm -f *.o *~ 
