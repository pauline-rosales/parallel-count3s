# Parallel Count3s (OpenMP + Pthreads)

Implements a parallel Count3s benchmark using OpenMP and Pthreads to compare shared-memory parallel performance. The program generates a large array of random integers, counts how many are equal to 3, and reports serial vs parallel runtimes and speedup.

On Linux with GCC + OpenMP:
- `make`
- `./count3_omp N T`
- `./count3_pthreads N T`

On macOS (no OpenMP by default):
- `gcc -O2 -Wall -Wextra -std=c11 count3_pthreads_padding.c timer.c -o count3_pthreads`
- `./count3_pthreads N T`
