CC     = clang
CFLAGS = -Xpreprocessor -fopenmp -O3 \
         -I/opt/homebrew/opt/libomp/include
LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp

all: stencil hello_omp dotproduct omp_prefix_sum

stencil: stencil/stencil.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o stencil/stencil

hello_omp: hello_omp.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

dotproduct: dotproduct.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

omp_prefix_sum: omp_prefix_sum.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f stencil/stencil hello_omp dotproduct omp_prefix_sum
