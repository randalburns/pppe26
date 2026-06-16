#!/usr/bin/env python3
"""
Roofline model plotter.
Usage: python3 roofline.py <peak_bw_GBs> <simd_fma> [simd_nofma] [scalar_fma] [scalar_nofma]

Example (Apple M4 single-thread, measured values):
    python3 roofline.py 113.1 73.0 47.5 18.2 9.1
"""

import sys
import numpy as np
import matplotlib.pyplot as plt

# (color, linestyle, linewidth)
_STYLES = {
    "simd_fma":    ("#1f77b4", "-",  2.5),   # blue  solid
    "simd_nofma":  ("#9467bd", "--", 1.8),   # purple dashed
    "scalar_fma":  ("#ff7f0e", "--", 1.8),   # orange dashed
    "scalar_nofma":("#2ca02c", ":",  1.8),   # green  dotted
}


def _draw_ceiling(ax, ai, bw, peak, key, label):
    color, ls, lw = _STYLES[key]
    perf  = np.minimum(bw * ai, peak)
    ridge = peak / bw
    ax.loglog(ai, perf, color=color, lw=lw, ls=ls, zorder=3, label=label)
    ax.scatter([ridge], [peak], color=color, zorder=5, s=50)
    return ridge, color


def plot_roofline(peak_bw: float,
                  simd_fma:    float,
                  simd_nofma:  float | None = None,
                  scalar_fma:  float | None = None,
                  scalar_nofma:float | None = None,
                  title: str = "") -> None:

    ai = np.logspace(-2, 4, 2000)
    fig, ax = plt.subplots(figsize=(11, 6))

    # Draw bottom-to-top so higher lines sit on top visually
    ceilings = [
        ("scalar_nofma", scalar_nofma, f"Scalar mul-only  ({scalar_nofma:.1f} GFLOPS/s)" if scalar_nofma else None),
        ("scalar_fma",   scalar_fma,   f"Scalar FMA       ({scalar_fma:.1f} GFLOPS/s)"   if scalar_fma   else None),
        ("simd_nofma",   simd_nofma,   f"SIMD mul-only    ({simd_nofma:.1f} GFLOPS/s)"   if simd_nofma   else None),
        ("simd_fma",     simd_fma,     f"SIMD FMA         ({simd_fma:.1f} GFLOPS/s)"),
    ]

    ridge_annotations = []   # (x, y, label, color)
    for key, peak, label in ceilings:
        if peak is None:
            continue
        ridge, color = _draw_ceiling(ax, ai, peak_bw, peak, key, label)
        ridge_annotations.append((ridge, peak, color))

    # Ridge annotations — offset alternately above/below to reduce overlap
    for i, (r, peak, color) in enumerate(ridge_annotations):
        yfactor = 0.42 if i % 2 == 0 else 0.58
        ax.annotate(
            f"{r:.2f}",
            xy=(r, peak),
            xytext=(r * 2.0, peak * yfactor),
            fontsize=7.5, color=color,
            arrowprops=dict(arrowstyle="->", color=color, lw=0.8),
        )

    # Memory bandwidth label on the slope
    slope_x = 10 ** (np.log10(ai[0]) / 2 + np.log10(simd_fma / peak_bw) / 2)
    ax.text(slope_x * 0.7, peak_bw * slope_x * 4,
            f"Peak BW\n{peak_bw:.1f} GB/s",
            fontsize=9, ha="right", va="bottom", color="#555")

    ax.set_xlabel("Arithmetic Intensity  (FLOP / byte)", fontsize=12)
    ax.set_ylabel("Performance  (GFLOPS/s)", fontsize=12)
    ax.set_title(title or "Roofline Model — Apple M4 (single thread, FP32)",
                 fontsize=13, fontweight="bold")
    ax.legend(loc="lower right", fontsize=9)
    ax.grid(True, which="both", linestyle=":", linewidth=0.5, alpha=0.5)
    ax.set_xlim(ai[0], ai[-1])

    plt.tight_layout()
    outfile = "roofline.png"
    plt.savefig(outfile, dpi=150)
    print(f"Saved {outfile}")
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    args = [float(a) for a in sys.argv[1:6]]
    while len(args) < 5:
        args.append(None)
    title = sys.argv[6] if len(sys.argv) > 6 else ""
    plot_roofline(*args, title=title)
