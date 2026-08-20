# OpenCilk toolchain (https://www.opencilk.org)
CC     = $(HOME)/opencilk/bin/clang
CFLAGS = -fopencilk -O3 -Wall

# the head-to-head benchmarks also link libomp so they can run the OpenMP versions
OMP_INC = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -Wno-deprecated-declarations
OMP_LIB = -L/opt/homebrew/opt/libomp/lib -lomp

BINS = fib vector_add sum_reducer qsort_steal mergesort_balanced sparse_col_sum

all: $(BINS)

qsort_steal mergesort_balanced sparse_col_sum: %: %.c
	$(CC) $(CFLAGS) $(OMP_INC) $(OMP_LIB) $< -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BINS)

.PHONY: all clean
