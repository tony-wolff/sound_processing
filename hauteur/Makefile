#--------------------------------
# GENERAL VARIABLES
#--------------------------------

CC = gcc 
CFLAGS = -O3 -I. -Wall -lm #-pg -g

#--------------------------------
# LIBRARY SOURCES
#--------------------------------

LIB_SRC =  gnuplot_i.c 
LIB_OBJ = $(LIB_SRC:.c=.o)
EXECS = hauteur

all: $(EXECS)

hauteur : $(LIB_OBJ) $(LIB_SRC) hauteur.c
	$(CC) $(CFLAGS) $(LIB_OBJ) hauteur.c -o $(EXECS) -lsndfile  -lvorbis -lvorbisenc -logg -lFLAC -lfftw3 -lm 

clean : 
	rm -f *.o 

cleanall :
	rm -f *.o *~ 
