#
# Students' Makefile for the Malloc Lab
#
VERSION = 1

CC = gcc

# for benchmarking - this turns asserts off.
CFLAGS = -Wall -O3 -Werror -pthread -DNDEBUG=1
# for debugging, with asserts on
#CFLAGS = -Wall -g -Werror -pthread

SHARED_OBJS = mdriver.o memlib.o fsecs.o fcyc.o clock.o ftimer.o list.o
OBJS = $(SHARED_OBJS) mm.o
MTOBJS = $(SHARED_OBJS) mmts.o
GBACK_IMPL_OBJS = $(SHARED_OBJS) mm-gback-implicit.o

# Variables used for instrumentation purposes
INST_BUILD_LOC = /home/courses/cs3214/bin/heap-visualizer/processing/build
CLANGFLAGS = -O3 -ggdb3 -emit-llvm -c -fPIC -Wno-address-of-packed-member
LLCFLAGS = --relocation-model=pic -O3 -filetype=obj

all: mdriver libMallocInstrumented.so

mdriver: $(OBJS)
	$(CC) $(CFLAGS) -o mdriver $(OBJS)

libMallocInstrumented.so: mm.c list.c memlib.c mallocanalysis.c
	# This compiles and instruments mm.c
	clang $(CLANGFLAGS) mm.c -o mm.bc
	LD_LIBRARY_PATH=$(INST_BUILD_LOC)/lib opt -load libLLVMMallocInjectorPass.so -mallocinjector mm.bc -o mm-instrumented.bc
	llc $(LLCFLAGS) mm-instrumented.bc -o mm-instrumented.o

	# This compiles and instruments list.c
	clang $(CLANGFLAGS) list.c -o list.bc
	LD_LIBRARY_PATH=$(INST_BUILD_LOC)/lib opt -load libLLVMMallocInjectorPass.so -mallocinjector list.bc -o list-instrumented.bc
	llc $(LLCFLAGS) list-instrumented.bc -o list-instrumented.o

	# Now, simply build memlib and mallocanalysis
	clang -ggdb3 -c -fPIC memlib.c -o memlib-instrumented.o
	clang -ggdb3 -c -fPIC mallocanalysis.c -o mallocanalysis.o
	
	# Link it all together
	clang -shared mm-instrumented.o list-instrumented.o memlib-instrumented.o mallocanalysis.o -o libMallocInstrumented.so

# if multi-threaded implementation is attempted
mdriver-ts: $(MTOBJS)
	$(CC) $(CFLAGS) -o mdriver-ts $(MTOBJS)

# build an executable for implicit list example
mdriver-implicit-example: $(GBACK_IMPL_OBJS)
	$(CC) $(CFLAGS) -o $@ $(GBACK_IMPL_OBJS)

mdriver.o: mdriver.c fsecs.h fcyc.h clock.h memlib.h config.h mm.h
memlib.o: memlib.c memlib.h config.h
mm.o: mm.c mm.h memlib.h

mmts.o: mm.c mm.h memlib.h
	$(CC) $(CFLAGS) -DTHREAD_SAFE=1 -c mm.c -o mmts.o

fsecs.o: fsecs.c fsecs.h config.h
fcyc.o: fcyc.c fcyc.h
ftimer.o: ftimer.c ftimer.h config.h
clock.o: clock.c clock.h
list.o: list.c list.h

handin:
	/home/courses/cs3214/bin/submit.py p3 mm.c

clean:
	rm -f *~ *.o *.so *.bc debug.txt mdriver libMallocInstrumented.so


