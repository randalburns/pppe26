CXX      = clang++
CXXFLAGS = -std=c++17 -O3 -march=native

OPENBLAS = /opt/homebrew/opt/openblas

all: stream peak_flops sort_bench dot_bench matmul_bench openblas_bench

stream: stream.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

peak_flops: peak_flops.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

sort_bench: sort_bench.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

dot_bench: dot_bench.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

matmul_bench: matmul_bench.cpp
	$(CXX) $(CXXFLAGS) -DACCELERATE_NEW_LAPACK -framework Accelerate -o $@ $<

openblas_bench: matmul_bench.cpp
	$(CXX) $(CXXFLAGS) -DUSE_OPENBLAS \
	    -I$(OPENBLAS)/include -L$(OPENBLAS)/lib -lopenblas \
	    -o $@ $<

# Run all benchmarks, capture results, generate the plot
plot: all
	@echo "=== Memory Bandwidth (STREAM) ==="
	@./stream | tee /tmp/stream_out.txt
	@echo ""
	@echo "=== Peak FLOPS (SIMD FMA, SIMD no-FMA, Scalar FMA, Scalar no-FMA) ==="
	@./peak_flops | tee /tmp/flops_out.txt
	@echo ""
	@echo "=== std::sort 1M floats ==="
	@./sort_bench | tee /tmp/sort_out.txt
	@echo ""
	@echo "=== dot product 32M floats ==="
	@./dot_bench | tee /tmp/dot_out.txt
	@echo ""
	@echo "=== BLAS sgemm 1024x1024 ==="
	@./matmul_bench 1024 | tee /tmp/mm1024_out.txt
	@echo ""
	@echo "=== BLAS sgemm 4096x4096 ==="
	@./matmul_bench 4096 | tee /tmp/mm4096_out.txt
	@BW=$$(grep        "^PEAK_BANDWIDTH="            /tmp/stream_out.txt | cut -d= -f2); \
	 GF_SFMA=$$(grep   "^PEAK_GFLOPS_SIMD_FMA="     /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SMUL=$$(grep   "^PEAK_GFLOPS_SIMD_NOFMA="   /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SCFMA=$$(grep  "^PEAK_GFLOPS_SCALAR_FMA="   /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SCMUL=$$(grep  "^PEAK_GFLOPS_SCALAR_NOFMA=" /tmp/flops_out.txt  | cut -d= -f2); \
	 SORT_AI=$$(grep   "^SORT_AI="                   /tmp/sort_out.txt   | cut -d= -f2); \
	 SORT_GF=$$(grep   "^SORT_GFLOPS="               /tmp/sort_out.txt   | cut -d= -f2); \
	 DOT_AI=$$(grep    "^DOTPROD_AI="                /tmp/dot_out.txt    | cut -d= -f2); \
	 DOT_GF=$$(grep    "^DOTPROD_GFLOPS="            /tmp/dot_out.txt    | cut -d= -f2); \
	 MM1024_AI=$$(grep "^SGEMM_AI="                  /tmp/mm1024_out.txt | cut -d= -f2); \
	 MM1024_GF=$$(grep "^SGEMM_GFLOPS="              /tmp/mm1024_out.txt | cut -d= -f2); \
	 MM4096_AI=$$(grep "^SGEMM_AI="                  /tmp/mm4096_out.txt | cut -d= -f2); \
	 MM4096_GF=$$(grep "^SGEMM_GFLOPS="              /tmp/mm4096_out.txt | cut -d= -f2); \
	 echo ""; echo "=== OpenBLAS sgemm 1024x1024 ==="; \
	 ./openblas_bench 1024 | tee /tmp/ob1024_out.txt; \
	 echo ""; echo "=== OpenBLAS sgemm 4096x4096 ==="; \
	 ./openblas_bench 4096 | tee /tmp/ob4096_out.txt; \
	 OB1024_AI=$$(grep "^SGEMM_AI="     /tmp/ob1024_out.txt | cut -d= -f2); \
	 OB1024_GF=$$(grep "^SGEMM_GFLOPS=" /tmp/ob1024_out.txt | cut -d= -f2); \
	 OB4096_AI=$$(grep "^SGEMM_AI="     /tmp/ob4096_out.txt | cut -d= -f2); \
	 OB4096_GF=$$(grep "^SGEMM_GFLOPS=" /tmp/ob4096_out.txt | cut -d= -f2); \
	 python3 roofline.py "$${BW}" "$${GF_SFMA}" "$${GF_SMUL}" "$${GF_SCFMA}" "$${GF_SCMUL}" \
	     --point "std::sort 1M"       "$${SORT_AI}"    "$${SORT_GF}" \
	     --point "dot product 32M"    "$${DOT_AI}"     "$${DOT_GF}" \
	     --point "Accel sgemm 1024²"  "$${MM1024_AI}"  "$${MM1024_GF}" \
	     --point "Accel sgemm 4096²"  "$${MM4096_AI}"  "$${MM4096_GF}" \
	     --point "OBlas sgemm 1024²"  "$${OB1024_AI}"  "$${OB1024_GF}" \
	     --point "OBlas sgemm 4096²"  "$${OB4096_AI}"  "$${OB4096_GF}"

clean:
	rm -f stream peak_flops sort_bench dot_bench matmul_bench openblas_bench *.o roofline.png \
	      /tmp/stream_out.txt /tmp/flops_out.txt /tmp/sort_out.txt /tmp/dot_out.txt \
	      /tmp/mm1024_out.txt /tmp/mm4096_out.txt /tmp/ob1024_out.txt /tmp/ob4096_out.txt
