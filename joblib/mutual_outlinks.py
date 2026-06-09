"""
Mutual Outlinks: A First Parallel Program using joblib

Derived from Chapter 1 of Mattson, Parallel Computing for Data Science.
"""

import os
import time
import tempfile
import numpy as np
import networkx as nx
from joblib import Parallel, delayed, dump, load
from random import random
from threading import Thread
from multiprocessing import Process


def serial_outlinks(gmat):
    outmat = np.zeros(gmat.shape)
    for i in range(gmat.shape[0]):
        for j in range(i + 1, gmat.shape[1]):
            outmat[i, j] = np.dot(gmat[i, :].reshape(-1), gmat[j, :].reshape(-1))
    return outmat


def inner_loop(gmat, i):
    partial_out = np.zeros(gmat.shape[1])
    for j in range(i + 1, gmat.shape[1]):
        partial_out[j] = np.dot(gmat[i, :].reshape(-1), gmat[j, :].reshape(-1))
    return partial_out


def gen_random_01(count):
    for i in range(count):
        random()


if __name__ == "__main__":

    # --- Small graph: visualize the structure ---

    G_small = nx.erdos_renyi_graph(5, 0.8, directed=True)
    gmat_small = nx.to_numpy_array(G_small)
    print("Small graph adjacency matrix:")
    print(gmat_small)

    outmat_small = np.zeros(gmat_small.shape)
    for i in range(gmat_small.shape[0]):
        for j in range(i + 1, gmat_small.shape[0]):
            outmat_small[i, j] = np.dot(gmat_small[i, :], gmat_small[j, :])
    print("\nMutual outlinks (small graph):")
    print(outmat_small)

    # --- Large graph: 1000 nodes, ~5 outbound edges per node ---

    G = nx.erdos_renyi_graph(1000, 0.01, directed=True)
    print(f"\nLarge graph edges: {G.number_of_edges()}")
    gmat = nx.to_numpy_array(G)

    # --- Serial computation ---

    outmat = serial_outlinks(gmat)
    print(f"Non-zero mutual outlinks (serial): {np.count_nonzero(outmat)}")

    t0 = time.perf_counter()
    for _ in range(3):
        serial_outlinks(gmat)
    serial_time = (time.perf_counter() - t0) / 3
    print(f"Serial time (avg of 3): {serial_time:.3f}s")

    # --- Parallel with pickling (baseline) ---

    partials = Parallel(n_jobs=8)(delayed(inner_loop)(gmat, i) for i in range(gmat.shape[0]))
    outmat_parallel = np.array(partials)
    print(f"Non-zero mutual outlinks (parallel): {np.count_nonzero(outmat_parallel)}")

    t0 = time.perf_counter()
    for _ in range(3):
        Parallel(n_jobs=8)(delayed(inner_loop)(gmat, i) for i in range(gmat.shape[0]))
    parallel_time = (time.perf_counter() - t0) / 3
    print(f"Parallel (pickle) time (avg of 3): {parallel_time:.3f}s  speedup: {serial_time / parallel_time:.2f}x")

    # --- Parallel with memmap: workers share the array without pickling ---
    # joblib detects numpy memmaps and sends only file metadata to workers;
    # each worker opens the same file read-only via OS shared memory.

    with tempfile.NamedTemporaryFile(suffix=".npy", delete=False) as f:
        mmap_path = f.name
    try:
        dump(gmat, mmap_path)
        gmat_mmap = load(mmap_path, mmap_mode="r")

        partials_mmap = Parallel(n_jobs=8)(delayed(inner_loop)(gmat_mmap, i) for i in range(gmat_mmap.shape[0]))
        outmat_mmap = np.array(partials_mmap)
        print(f"Non-zero mutual outlinks (memmap):  {np.count_nonzero(outmat_mmap)}")

        t0 = time.perf_counter()
        for _ in range(3):
            Parallel(n_jobs=8)(delayed(inner_loop)(gmat_mmap, i) for i in range(gmat_mmap.shape[0]))
        mmap_time = (time.perf_counter() - t0) / 3
        print(f"Parallel (memmap) time (avg of 3): {mmap_time:.3f}s  speedup: {serial_time / mmap_time:.2f}x")
    finally:
        os.unlink(mmap_path)

    # --- Vectorized: replace nested Python loop with a single BLAS matrix multiply ---
    # outmat[i,j] = dot(gmat[i,:], gmat[j,:])  is exactly the upper triangle of gmat @ gmat.T

    outmat_vec = np.triu(gmat @ gmat.T, k=1)
    print(f"Non-zero mutual outlinks (vectorized): {np.count_nonzero(outmat_vec)}")

    t0 = time.perf_counter()
    for _ in range(3):
        np.triu(gmat @ gmat.T, k=1)
    vec_time = (time.perf_counter() - t0) / 3
    print(f"Vectorized time (avg of 3):        {vec_time:.3f}s  speedup: {serial_time / vec_time:.0f}x")

    # --- GIL demonstration: threads vs processes ---

    N = 10_000_000

    t0 = time.perf_counter()
    gen_random_01(N)
    serial_rand = time.perf_counter() - t0
    print(f"\nSerial random gen: {serial_rand:.3f}s")

    t0 = time.perf_counter()
    threads = [Thread(target=gen_random_01, args=(N // 4,)) for _ in range(4)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    thread_time = time.perf_counter() - t0
    print(f"Threaded (4 threads, GIL-bound): {thread_time:.3f}s")

    t0 = time.perf_counter()
    procs = [Process(target=gen_random_01, args=(N // 4,)) for _ in range(4)]
    for p in procs:
        p.start()
    for p in procs:
        p.join()
    proc_time = time.perf_counter() - t0
    print(f"Multiprocessing (4 processes): {proc_time:.3f}s")
