# Reads summary.csv and writes plot_N{size}.png (one per array size)
import csv, collections
import matplotlib
matplotlib.use("Agg")  # headless
import matplotlib.pyplot as plt

rows = []
with open("summary.csv") as f:
    r = csv.DictReader(f)
    for row in r:
        rows.append({
            "impl": row["impl"],
            "N": int(row["N"]),
            "T": int(row["T"]),
            "avg_s": float(row["avg_serial_ms"]),
            "std_s": float(row["std_serial_ms"]),
            "avg_p": float(row["avg_parallel_ms"]),
            "std_p": float(row["std_parallel_ms"]),
        })

byN = collections.defaultdict(lambda: collections.defaultdict(list))
for x in rows:
    byN[x["N"]][x["impl"]].append(x)

for N, implmap in byN.items():
    plt.figure()

    # parallel time curves for each impl
    for impl, grp in implmap.items():
        grp.sort(key=lambda g: g["T"])
        T = [g["T"] for g in grp]
        avg_p = [g["avg_p"] for g in grp]
        std_p = [g["std_p"] for g in grp]
        plt.errorbar(T, avg_p, yerr=std_p, fmt='o-', capsize=3, label="%s parallel" % impl)

    # serial reference (OpenMP serial line)
    if "openmp" in implmap:
        grp = sorted(implmap["openmp"], key=lambda g: g["T"])
        T = [g["T"] for g in grp]
        avg_s = [g["avg_s"] for g in grp]
        std_s = [g["std_s"] for g in grp]
        plt.errorbar(T, avg_s, yerr=std_s, fmt='s--', capsize=3, label="serial (ref)")

    # Older Matplotlib API: use 'basex'
    plt.xscale('log', basex=2)   # log2 threads
    plt.yscale('log')            # log time
    plt.title("Count-3 Timing (N=%d)" % N)
    plt.xlabel("# threads (log2 scale)")
    plt.ylabel("Time (ms, log scale)")
    plt.grid(True, which='both', ls='--', alpha=0.4)
    plt.legend()
    plt.tight_layout()
    plt.savefig("plot_N%d.png" % N, dpi=180)
    plt.close()

print("Wrote plots.")
