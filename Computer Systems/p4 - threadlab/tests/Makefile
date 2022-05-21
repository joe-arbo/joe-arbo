##
# Temporary Makefile
#
# Treat warnings as errors. This seems to be the only way to
# convince some students of the importance of ensuring that
# their code compiles without warnings before starting to debug.
#
# Do not change this line.  We will not use your copy of the Makefile
# we will use *this* Makefile when grading.
#
# for benchmarking we'll use this.
# Note that this defines NDEBUG, which removes all assert()s from your code
CFLAGS=-pthread -Wall -O3 -Werror -Wmissing-prototypes -fopenmp -DNDEBUG
LDFLAGS=-pthread
# for debugging, you may use these 2
#LDFLAGS=-pthread -fsanitize=undefined
#CFLAGS=-Wall -O0 -ggdb3 -Werror -Wmissing-prototypes -fopenmp -fsanitize=undefined

OBJ=threadpool.o list.o threadpool_lib.o

ALL=quicksort psum_test fib_test mergesort nqueens \
	threadpool_test threadpool_test2 threadpool_test3 threadpool_test4 threadpool_test5 \
	threadpool_test6

all: $(ALL)

threadpool_test6: threadpool_test6.o $(OBJ)

threadpool_test5: threadpool_test5.o $(OBJ)

threadpool_test4: threadpool_test4.o $(OBJ)

threadpool_test3: threadpool_test3.o $(OBJ)

threadpool_test2: threadpool_test2.o $(OBJ)

threadpool_test: threadpool_test.o $(OBJ)

quicksort: quicksort.o $(OBJ)

nqueens: nqueens.o $(OBJ)

mergesort: mergesort.o $(OBJ)

psum_test: psum_test.o $(OBJ)

fib_test: fib_test.o $(OBJ)

clean:
	rm -f *.o $(ALL)

