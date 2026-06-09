#!/usr/bin/env python3
"""
Roofline model plotter.
Usage: python3 roofline.py <peak_bw_GBs> <peak_simd_gflops> [peak_scalar_gflops]

Example (Apple M4 single-thread, measured values):
    python3 roofline.py 113.1 62.9 15.7
"""

import sys
import numpy as np
import matplotlib.pyplot as plt


def plot_roofline(peak_bw: float, peak_simd: float,
                  peak_scalar: float | None = None, title: str = "") -> None:

    ai = np.logspace(-2, 4, 2000)

    fig, ax = plt.subplots(figsize=(10, 6))

    # --- memory bandwidth slope (shared by both ceilings) ---
    slope_mid_x = 10 ** (np.log10(ai[0]) / 2 + np.log10(peak_simd / peak_bw) / 2)
    slope_mid_y = peak_bw * slope_mid_x
    ax.text(slope_mid_x * 0.7, slope_mid_y * 4,
            f"Peak BW\n{peak_bw:.1f} GB/s",
            fontsize=9, ha="right", va="bottom", color="#555")

    # --- scalar roofline (draw first so SIMD sits on top) ---
    if peak_scalar is not None:
        ridge_s   = peak_scalar / peak_bw
        perf_s    = np.minimum(peak_bw * ai, peak_scalar)
        ax.loglog(ai, perf_s, color="#ff7f0e", lw=2.0, ls="--", zorder=3,
                  label=f"Scalar FP32  ({peak_scalar:.1f} GFLOPS/s)")

        ax.scatter([ridge_s], [peak_scalar], color="#ff7f0e", zorder=5, s=60)
        ax.annotate(
            f"Ridge: {ridge_s:.2f} FLOP/B",
            xy=(ridge_s, peak_scalar),
            xytext=(ridge_s * 1.6, peak_scalar * 0.45),
            fontsize=8, color="#ff7f0e",
            arrowprops=dict(arrowstyle="->", color="#ff7f0e", lw=0.9),
        )

    # --- SIMD roofline ---
    ridge_v  = peak_simd / peak_bw
    perf_v   = np.minimum(peak_bw * ai, peak_simd)
    ax.loglog(ai, perf_v, color="#1f77b4", lw=2.5, zorder=4,
              label=f"SIMD FP32x4  ({peak_simd:.1f} GFLOPS/s)")

    ax.scatter([ridge_v], [peak_simd], color="red", zorder=6, s=60)
    ax.annotate(
        f"Ridge: {ridge_v:.2f} FLOP/B",
        xy=(ridge_v, peak_simd),
        xytext=(ridge_v * 1.6, peak_simd * 0.55),
        fontsize=8, color="red",
        arrowprops=dict(arrowstyle="->", color="red", lw=0.9),
    )

    # --- SIMD speedup callout ---
    if peak_scalar is not None:
        speedup = peak_simd / peak_scalar
        ax.text(ridge_v * 80, peak_simd * 0.6,
                f"SIMD peak\n{peak_simd:.1f} GFLOPS/s\n({speedup:.1f}× scalar)",
                fontsize=9, ha="left", va="center", color="#1f77b4")
        # mark the "SIMD gains nothing" zone between scalar ridge and SIMD ridge
        ax.axvspan(peak_scalar / peak_bw, ridge_v,
                   alpha=0.05, color="green",
                   label="Scalar compute-bound,\nSIMD memory-bound")
    else:
        ax.text(ridge_v * 80, peak_simd * 0.55,
                f"Peak compute\n{peak_simd:.1f} GFLOPS/s",
                fontsize=9, ha="left", va="center", color="#1f77b4")

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
    scalar = float(sys.argv[3]) if len(sys.argv) > 3 else None
    title  = sys.argv[4] if len(sys.argv) > 4 else ""
    plot_roofline(float(sys.argv[1]), float(sys.argv[2]), scalar, title)
