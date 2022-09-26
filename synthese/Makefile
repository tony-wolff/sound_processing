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
EXECS = synthese synthese-2

all: $(EXECS)

synthese : $(LIB_OBJ) $(LIB_SRC) synthese.c
	$(CC) $(CFLAGS) $(LIB_OBJ) synthese.c -o synthese -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lfftw3 -lm 

synthse-2 : $(LIB_OBJ) $(LIB_SRC) synthese-2.c
	$(CC) $(CFLAGS) $(LIB_OBJ) synthese-2.c -o synthese-2 -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lfftw3 -lm 

clean : 
	rm -f *.o 

cleanall :
	rm -f *.o *~ 
