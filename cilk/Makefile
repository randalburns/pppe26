# OpenCilk toolchain (https://www.opencilk.org)
CC     = $(HOME)/opencilk/bin/clang
CFLAGS = -fopencilk -O3 -Wall

# qsort_steal also links libomp so it can run the OpenMP-task version head to head
OMP_INC = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include
OMP_LIB = -L/opt/homebrew/opt/libomp/lib -lomp

BINS = fib vector_add sum_reducer qsort_steal mergesort_balanced

all: $(BINS)

qsort_steal mergesort_balanced: %: %.c
	$(CC) $(CFLAGS) $(OMP_INC) $(OMP_LIB) $< -o $@

%: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(BINS)

.PHONY: all clean
