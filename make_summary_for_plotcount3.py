# Aggregates results.csv -> summary.csv per (impl,N,T) with avg/std + speedup/efficiency
import sys, csv, statistics as stats
from collections import defaultdict

if len(sys.argv) < 2:
    print("usage: python3 make_summary_for_plotcount3.py results.csv"); sys.exit(1)

rows = []
with open(sys.argv[1], newline='') as f:
    rd = csv.DictReader(f)
    for r in rd:
        if not r: continue
        ok = (r.get('ok','1') or '1').strip().lower()
        if ok not in ('1','true','yes'): continue
        try:
            impl = r['impl'].strip()
            N    = int(r['N'])
            T    = int(r['T'])
            sm   = float(r['serial_ms'])
            pm   = float(r['parallel_ms'])
        except Exception:
            continue
        if impl not in ('openmp','pthreads'): continue
        rows.append((impl,N,T,sm,pm))

agg = defaultdict(lambda: {'s': [], 'p': []})
for impl,N,T,sm,pm in rows:
    agg[(impl,N,T)]['s'].append(sm)
    agg[(impl,N,T)]['p'].append(pm)

with open('summary.csv','w',newline='') as f:
    wr = csv.writer(f)
    wr.writerow(['impl','N','T','avg_serial_ms','std_serial_ms','avg_parallel_ms','std_parallel_ms','speedup','efficiency'])
    for (impl,N,T), vals in sorted(agg.items()):
        s, p = vals['s'], vals['p']
        avg_s = stats.mean(s); std_s = stats.stdev(s) if len(s)>1 else 0.0
        avg_p = stats.mean(p); std_p = stats.stdev(p) if len(p)>1 else 0.0
        speedup = (avg_s/avg_p) if avg_p>0 else 0.0
        eff = (speedup/T) if T>0 else 0.0
        wr.writerow([impl,N,T,f"{avg_s:.6f}",f"{std_s:.6f}",f"{avg_p:.6f}",f"{std_p:.6f}",f"{speedup:.6f}",f"{eff:.6f}"])

print("Wrote summary.csv")
