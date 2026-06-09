CC     = clang
CFLAGS = -Xpreprocessor -fopenmp -O3 \
         -I/opt/homebrew/opt/libomp/include
LDFLAGS = -L/opt/homebrew/opt/libomp/lib -lomp

all: stencil hello_omp

stencil: stencil.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

hello_omp: hello_omp.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f stencil hello_omp
