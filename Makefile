CC      = gcc
CFLAGS  = -O2 -Wall -Wextra -std=c11
OMPFLAGS= -fopenmp
PTHFLAGS= -pthread
LDLIBS  =               

all: count3_omp count3_pthreads

count3_omp: count3_openmp.c timer.c common.h timer.h
	$(CC) $(CFLAGS) $(OMPFLAGS) count3_openmp.c timer.c -o $@ $(LDLIBS)

count3_pthreads: count3_pthreads_padding.c timer.c common.h timer.h
	$(CC) $(CFLAGS) $(PTHFLAGS) count3_pthreads_padding.c timer.c -o $@ $(LDLIBS)

clean:
	rm -f count3_omp count3_pthreads *.o results.csv summary.csv plot_*.png
