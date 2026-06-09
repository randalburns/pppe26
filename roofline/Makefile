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
	@echo "=== Peak FLOPS (SIMD + Scalar) ==="
	@./peak_flops | tee /tmp/flops_out.txt
	@BW=$$(grep PEAK_BANDWIDTH       /tmp/stream_out.txt | cut -d= -f2); \
	 GF_SIMD=$$(grep PEAK_GFLOPS_SIMD   /tmp/flops_out.txt  | cut -d= -f2); \
	 GF_SC=$$(grep   PEAK_GFLOPS_SCALAR /tmp/flops_out.txt  | cut -d= -f2); \
	 echo ""; echo "Plotting: BW=$${BW} GB/s  SIMD=$${GF_SIMD} GFLOPS/s  Scalar=$${GF_SC} GFLOPS/s"; \
	 python3 roofline.py "$${BW}" "$${GF_SIMD}" "$${GF_SC}"

clean:
	rm -f stream peak_flops *.o roofline.png /tmp/stream_out.txt /tmp/flops_out.txt
