CFLAGS=-I. -Wall -Werror

OBJ=spawnattr_setflags.o  spawnattr_tcsetpgrp.o  spawn.o  spawni.o

all:	libspawn.a

libspawn.a: $(OBJ)	
	ar cr $@ $(OBJ)


clean:
	/bin/rm -f $(OBJ) libspawn.a

