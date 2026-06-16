CXX      = clang++
CXXFLAGS = -std=c++17 -O3 -march=native

all: stream peak_flops

stream: stream.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

peak_flops: peak_flops.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# Run both benchmarks, capture results, generate the plot
plot: all
	@echo "=== Memory Bandwidth (STREAM) ==="
	@./stream | tee /tmp/stream_out.txt
	@echo ""
	@echo "=== Peak FLOPS (SIMD FMA, SIMD no-FMA, Scalar FMA, Scalar no-FMA) ==="
	@./peak_flops | tee /tmp/flops_out.txt
	@BW=$$(grep       "^PEAK_BANDWIDTH="           /tmp/stream_out.txt | cut -d= -f2); \
	 GF_SFMA=$$(grep   "^PEAK_GFLOPS_SIMD_FMA="    /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SMUL=$$(grep   "^PEAK_GFLOPS_SIMD_NOFMA="  /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SCFMA=$$(grep  "^PEAK_GFLOPS_SCALAR_FMA="  /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SCMUL=$$(grep  "^PEAK_GFLOPS_SCALAR_NOFMA=" /tmp/flops_out.txt | cut -d= -f2); \
	 echo ""; echo "BW=$${BW}  SIMD_FMA=$${GF_SFMA}  SIMD_MUL=$${GF_SMUL}  SC_FMA=$${GF_SCFMA}  SC_MUL=$${GF_SCMUL}"; \
	 python3 roofline.py "$${BW}" "$${GF_SFMA}" "$${GF_SMUL}" "$${GF_SCFMA}" "$${GF_SCMUL}"

clean:
	rm -f stream peak_flops *.o roofline.png /tmp/stream_out.txt /tmp/flops_out.txt
