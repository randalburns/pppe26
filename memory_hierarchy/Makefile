# Benchmarks behind cache_hierarchy.ipynb.
#
#   make            build everything
#   make data       run every benchmark and refresh the .csv files
#
# Each program pins itself to a CPU and re-measures the core clock while it
# runs; see the comments at the top of each .c file for why that is necessary
# on a mobile part whose clock moves between 0.6 and 5.1 GHz.

CC      = gcc
CFLAGS  = -O2 -Wall
FAST    = -O3 -march=native -Wall
SCRATCH = /tmp/cache_hierarchy_scratch.bin

BINS = cache_latency stride_bandwidth core_to_core row_column bandwidth_scaling \
       storage_latency spin_cost set_conflict

all: $(BINS)

cache_latency: cache_latency.c
	$(CC) $(CFLAGS) -o $@ $<

# -fno-tree-vectorize: GCC vectorizes the row-order loop at -O2 and the
# row-vs-column comparison stops being about access order alone.
row_column: row_column.c
	$(CC) $(CFLAGS) -fno-tree-vectorize -o $@ $<

storage_latency: storage_latency.c
	$(CC) $(CFLAGS) -o $@ $<

# Supporting measurements for the two HTML pages rather than the notebook.
spin_cost: spin_cost.c
	$(CC) $(CFLAGS) -o $@ $<

set_conflict: set_conflict.c
	$(CC) $(CFLAGS) -fno-tree-vectorize -o $@ $<

stride_bandwidth: stride_bandwidth.c
	$(CC) $(FAST) -o $@ $<

core_to_core: core_to_core.c
	$(CC) $(CFLAGS) -pthread -o $@ $<

bandwidth_scaling: bandwidth_scaling.c
	$(CC) $(FAST) -pthread -o $@ $<

# Roughly 12 minutes total.  Run on an otherwise idle machine: every one of
# these measures a shared resource.
data: all
	./cache_latency 0     > latency.csv
	./stride_bandwidth 0  > stride_bandwidth.csv
	./row_column 0        > row_column.csv
	./bandwidth_scaling   > bandwidth_scaling.csv
	./core_to_core 12     > core_to_core.csv
	./storage_latency $(SCRATCH) 2048 > storage.csv
	{ ./spin_cost 0; echo; ./spin_cost 5 | head -6; echo; ./set_conflict; } > spin_and_sets.txt

clean:
	rm -f $(BINS)

.PHONY: all data clean
