# n=16 sorting comparison on x86-64 (AMD Ryzen / Zen 5)
#
# Highway's *static* target is chosen from the -m flags, not from the CPU.
# HWY_BASELINE_SSE4 requires AES+PCLMUL as well as SSE4.1, and AVX2 builds on
# top of it, so a bare -mavx2 silently falls back to the SSSE3 target and
# makes Highway look ~2x slower than it is.  AVX2FLAGS below is the minimum
# set that actually selects HWY_AVX2.

AVX2FLAGS := -mavx2 -mbmi -mbmi2 -mfma -mf16c -maes -mpclmul

SRC  := sort_compare_n16_x86.cpp
LIBS := -lhwy
BINS := n16_gcc_avx2 n16_gcc_native n16_clang_avx2 n16_clang_native

.PHONY: all run clean

all: $(BINS)

n16_gcc_avx2:     $(SRC) ; g++ -O2 -std=c++17 $(AVX2FLAGS) -o $@ $< $(LIBS)
n16_gcc_native:   $(SRC) ; g++ -O2 -std=c++17 -march=native -o $@ $< $(LIBS)
n16_clang_avx2:   $(SRC) ; clang++ -O2 -std=c++17 -stdlib=libc++ $(AVX2FLAGS) -o $@ $< $(LIBS)
n16_clang_native: $(SRC) ; clang++ -O2 -std=c++17 -stdlib=libc++ -march=native -o $@ $< $(LIBS)

# Pin to one core: at 14-60 ns per sort, frequency ramp and core migration
# move the numbers by 2x if the process is left to float.
run: $(BINS)
	@for b in $(BINS); do \
		echo ""; \
		echo "======== $$b ========"; \
		taskset -c 4 ./$$b; \
	done

clean:
	rm -f $(BINS)
