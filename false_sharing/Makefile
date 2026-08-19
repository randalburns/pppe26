CXX      := clang++
CXXFLAGS := -std=c++17 -stdlib=libc++ -pthread

SRC  := false_sharing.cpp
BINS := fs_O0 fs_O1 fs_O2 fs_O3

.PHONY: all run clean

all: $(BINS)

fs_O0: $(SRC) ; $(CXX) $(CXXFLAGS) -O0 -o $@ $<
fs_O1: $(SRC) ; $(CXX) $(CXXFLAGS) -O1 -o $@ $<
fs_O2: $(SRC) ; $(CXX) $(CXXFLAGS) -O2 -o $@ $<
fs_O3: $(SRC) ; $(CXX) $(CXXFLAGS) -O3 -o $@ $<

run: $(BINS)
	@for lvl in O0 O1 O2 O3; do \
		echo ""; \
		echo "======== -$$lvl ========"; \
		./fs_$$lvl; \
	done

clean:
	rm -f $(BINS) false_sharing
