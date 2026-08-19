"""
prefixsum_chart.py

Runs ./prefixsum at varying thread counts, then plots:
  1. Speedup vs threads (measured + Amdahl fit)
  2. Parallel efficiency vs threads
  3. Amdahl serial fraction annotation
"""

import subprocess
import re
import numpy as np
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt

N = 100_000_000
TRIALS = 4
THREAD_COUNTS = [1, 2, 4, 6, 8, 10]


def run_prefixsum(n_threads):
    result = subprocess.run(
        ["./prefixsum", str(N), str(n_threads)],
        capture_output=True, text=True
    )
    times = re.findall(r"omp two-pass\s+time=([0-9.]+)", result.stdout)
    times = [float(t) for t in times]
    # drop first (warm-up already done inside binary), take median of rest
    return np.median(times)


print("Running prefixsum benchmarks...")
times = {}
for t in THREAD_COUNTS:
    times[t] = run_prefixsum(t)
    print(f"  {t:2d} threads: {times[t]:.4f} s")

t1 = times[1]
threads = np.array(THREAD_COUNTS)
speedups = np.array([t1 / times[t] for t in THREAD_COUNTS])
efficiency = speedups / threads


# Amdahl's law: S(p) = 1 / (f + (1-f)/p)
def amdahl(p, f):
    return 1.0 / (f + (1.0 - f) / p)

popt, _ = curve_fit(amdahl, threads, speedups, p0=[0.1], bounds=(0, 1))
serial_fraction = popt[0]
print(f"\nAmdahl serial fraction f = {serial_fraction:.4f}  ({serial_fraction*100:.2f}%)")
print(f"Theoretical max speedup  = {1/serial_fraction:.1f}x")

p_fine = np.linspace(1, max(THREAD_COUNTS), 200)
amdahl_curve = amdahl(p_fine, serial_fraction)

# --- Plot ---
fig, axes = plt.subplots(1, 3, figsize=(14, 5))
fig.suptitle(f"Prefix Sum OpenMP Scaling  (N={N:,})", fontsize=13)

# 1. Speedup
ax = axes[0]
ax.plot(threads, speedups, "o-", color="steelblue", label="Measured")
ax.plot(p_fine, amdahl_curve, "--", color="tomato",
        label=f"Amdahl fit (f={serial_fraction:.3f})")
ax.plot(p_fine, p_fine, ":", color="gray", label="Ideal linear")
ax.set_xlabel("Threads")
ax.set_ylabel("Speedup")
ax.set_title("Speedup")
ax.legend()
ax.set_xticks(THREAD_COUNTS)
ax.set_xlim(0, max(THREAD_COUNTS) + 0.5)
ax.set_ylim(0)

# 2. Parallel efficiency
ax = axes[1]
ax.plot(threads, efficiency * 100, "s-", color="seagreen")
ax.axhline(100, color="gray", linestyle=":", label="Ideal (100%)")
ax.set_xlabel("Threads")
ax.set_ylabel("Efficiency (%)")
ax.set_title("Parallel Efficiency")
ax.set_xticks(THREAD_COUNTS)
ax.set_xlim(0, max(THREAD_COUNTS) + 0.5)
ax.set_ylim(0, 110)
ax.legend()

# 3. Amdahl summary — bar showing serial vs parallel fraction
ax = axes[2]
bars = ax.barh(
    ["Parallel\nfraction", "Serial\nfraction"],
    [(1 - serial_fraction) * 100, serial_fraction * 100],
    color=["steelblue", "tomato"]
)
ax.set_xlabel("Fraction of runtime (%)")
ax.set_title(f"Amdahl Analysis\nMax speedup = {1/serial_fraction:.1f}x")
ax.set_xlim(0, 105)
for bar, val in zip(bars, [(1-serial_fraction)*100, serial_fraction*100]):
    ax.text(bar.get_width() + 1, bar.get_y() + bar.get_height()/2,
            f"{val:.1f}%", va="center")

plt.tight_layout()
out = "prefixsum_scaling.png"
plt.savefig(out, dpi=150)
print(f"\nSaved {out}")
plt.show()
