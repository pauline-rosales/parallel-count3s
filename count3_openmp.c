// OpenMP Count-3 with reduction; uses common.h filler (0..3)
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <omp.h>
#include "common.h"
#include "timer.h"

static const int TARGET = 3;

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "usage: %s N T\n", argv[0]);
        return 1;
    }
    size_t N = strtoull(argv[1], NULL, 10);
    int     T = atoi(argv[2]);

    unsigned int seed = 12345u;
    int *a = (int *)malloc(sizeof(int) * N);
    if (!a) { perror("malloc"); return 2; }
    fill_random_0_to_3(a, N, seed);

    /* serial */
    uint64_t t0 = ns_now();
    long long scount = count3_serial(a, N);
    uint64_t t1 = ns_now();

    /* parallel (OpenMP) — reduction */
    long long pcount = 0;
    uint64_t p0 = ns_now();
    #pragma omp parallel for num_threads(T) reduction(+:pcount) schedule(static)
    for (size_t i = 0; i < N; ++i) {
        pcount += (a[i] == TARGET);
    }
    uint64_t p1 = ns_now();

    double serial_ms   = (t1 - t0) / 1e6;
    double parallel_ms = (p1 - p0) / 1e6;
    int ok = (scount == pcount);

    /* CSV line for scripts */
    printf("impl=openmp,N=%zu,T_arg=%d,omp_max_threads=%d,serial_ms=%.3f,parallel_ms=%.3f,scount=%lld,pcount=%lld,ok=%d\n",
           N, T, omp_get_max_threads(), serial_ms, parallel_ms, scount, pcount, ok);

    /* human line */
    fprintf(stderr,"[OpenMP] N=%zu OMP_MAX=%d (arg T=%d) | serial=%.3f ms | parallel=%.3f ms | %s\n",
            N, omp_get_max_threads(), T, serial_ms, parallel_ms, ok ? "MATCH" : "MISMATCH");

    free(a);
    return ok ? 0 : 3;
}
