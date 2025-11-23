// Pthreads Count-3 with cacheline padding (1.14-style)
#include "common.h"
#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

static const int TARGET = 3;
#ifndef CACHELINE
#define CACHELINE 64
#endif

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
  #include <stdalign.h>
  typedef struct { alignas(CACHELINE) long long value; } padded_ll_t;
#else
  typedef struct { long long value; char _pad[CACHELINE - sizeof(long long)]; } __attribute__((aligned(CACHELINE))) padded_ll_t;
#endif

typedef struct { const int *a; size_t N; int tid, T; padded_ll_t *partials; } args_t;

static void *thr(void *p) {
    args_t *x=(args_t*)p; size_t lo,hi; chunk_bounds(x->N,x->T,x->tid,&lo,&hi);
    long long local=0; for(size_t i=lo;i<hi;++i) local += (x->a[i]==TARGET);
    x->partials[x->tid].value=local; return NULL;
}

int main(int argc,char**argv){
    if(argc<3){fprintf(stderr,"usage: %s N T [seed]\n",argv[0]);return 1;}
    size_t N=strtoull(argv[1],NULL,10); int T=atoi(argv[2]);
    unsigned int seed=(argc>3)?(unsigned int)strtoul(argv[3],NULL,10):12345u;

    int *a=(int*)malloc(sizeof(int)*N); if(!a){perror("malloc");return 2;}
    fill_random_0_to_3(a,N,seed);

    uint64_t t0=ns_now(); long long ref=count3_serial(a,N); uint64_t t1=ns_now();

    /* portable aligned allocation */
    padded_ll_t *partials = NULL;
    void *tmp = NULL;
    if (posix_memalign(&tmp, CACHELINE, sizeof(padded_ll_t)*T) != 0 || !tmp) {
        perror("posix_memalign"); free(a); return 3;
    }
    partials = (padded_ll_t*)tmp;
    for(int i=0;i<T;++i) partials[i].value=0;

    pthread_t *th=(pthread_t*)malloc(sizeof(pthread_t)*T);
    args_t *ar=(args_t*)malloc(sizeof(args_t)*T);
    if(!th||!ar){perror("malloc"); free(partials); free(a); return 4;}

    uint64_t p0=ns_now();
    for(int t=0;t<T;++t){ ar[t]=(args_t){.a=a,.N=N,.tid=t,.T=T,.partials=partials}; pthread_create(&th[t],NULL,thr,&ar[t]); }
    for(int t=0;t<T;++t) pthread_join(th[t],NULL);
    long long par=0; for(int t=0;t<T;++t) par+=partials[t].value;
    uint64_t p1=ns_now();

    double serial_ms=(t1-t0)/1e6, parallel_ms=(p1-p0)/1e6; int ok=(ref==par);
    printf("impl=pthreads,N=%zu,T=%d,serial_ms=%.3f,parallel_ms=%.3f,scount=%lld,pcount=%lld,ok=%d\n",
           N,T,serial_ms,parallel_ms,ref,par,ok);
    fprintf(stderr,"[Pthreads] N=%zu T=%d | serial=%.3f ms | parallel=%.3f ms | %s\n",
            N,T,serial_ms,parallel_ms,ok?"MATCH":"MISMATCH");

    free(ar); free(th); free(partials); free(a); return ok?0:2;
}
