#!/usr/bin/env bash
set -euo pipefail
OUT="results.csv"
echo "impl,N,T,serial_ms,parallel_ms,scount,pcount,ok" > "$OUT"
ARRAYS=(100000 1000000 10000000)   # 100K, 1M, 10M
THREADS=(2 4 8 16 32 64)
ITERS=6

run_omp(){ N=$1 T=$2
  export OMP_NUM_THREADS=$T
  line=$(./count3_omp "$N" "$T")
  IFS=',' read -ra kv <<< "$line"
  # extract fields
  declare -A m; for p in "${kv[@]}"; do k="${p%%=*}"; v="${p#*=}"; m[$k]="$v"; done
  echo "openmp,$N,$T,${m[serial_ms]},${m[parallel_ms]},${m[scount]},${m[pcount]},${m[ok]}" >> "$OUT"
}

run_pth(){ N=$1 T=$2
  line=$(./count3_pthreads "$N" "$T")
  IFS=',' read -ra kv <<< "$line"
  declare -A m; for p in "${kv[@]}"; do k="${p%%=*}"; v="${p#*=}"; m[$k]="$v"; done
  echo "pthreads,$N,$T,${m[serial_ms]},${m[parallel_ms]},${m[scount]},${m[pcount]},${m[ok]}" >> "$OUT"
}

for N in "${ARRAYS[@]}"; do
  for T in "${THREADS[@]}"; do
    for ((i=1;i<=ITERS;i++)); do
      echo "[OpenMP] N=$N T=$T ($i/$ITERS)"
      run_omp "$N" "$T"
      echo "[Pthreads] N=$N T=$T ($i/$ITERS)"
      run_pth "$N" "$T"
    done
  done
done
echo "Wrote $OUT"
