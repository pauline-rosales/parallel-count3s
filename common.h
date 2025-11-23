#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>

/* Fill arr[0..N-1] with integers in [0,3] (uniform-ish via rand_r) */
static inline void fill_random_0_to_3(int *arr, size_t N, unsigned int seed) {
    for (size_t i = 0; i < N; ++i) {
        arr[i] = (int)(rand_r(&seed) % 4);   // 0..3 inclusive
    }
}

/* Serial reference count of 3s */
static inline long long count3_serial(const int *arr, size_t N) {
    long long c = 0;
    for (size_t i = 0; i < N; ++i) c += (arr[i] == 3);
    return c;
}

/* Partition [0,N) into T roughly equal chunks, return [lo,hi) for tid */
static inline void chunk_bounds(size_t N, int T, int tid, size_t *lo, size_t *hi) {
    size_t base = N / (size_t)T;
    size_t rem  = N % (size_t)T;
    size_t tz   = (size_t)tid;

    *lo = tz * base + (tz < rem ? tz : rem);
    *hi = *lo + base + (tz < rem ? 1 : 0);
}
#endif
