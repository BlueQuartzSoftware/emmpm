# For Linux or any machines with gcc compiler
CC = gcc
CFLAGS = -ansi -Wall -pedantic

# For SunOS
#CFLAGS = -Aa

OBJ = em.o mpm.o random.o allocate.o tiff.o entropy.o

emmpm: $(OBJ) 
	cc $(CFLAGS) -o emmpm $(OBJ) -lm

em.o: em.c
	cc -c em.c

mpm.o: mpm.c
	cc -c mpm.c

random.o: random.c
	cc -c random.c

allocate.o: allocate.c
	cc -c allocate.c

tiff.o: tiff.c
	cc -c tiff.c
	
entropy.o: entropy.c
	cc -c entropy.c

clean:
	/bin/rm emmpm *.o
