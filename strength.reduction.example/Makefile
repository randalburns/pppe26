CXX      := clang++
CXXFLAGS := -std=c++14

SRC  := strength_reduction.cpp
BINS := sr_O0 sr_O1 sr_O2 sr_O3

.PHONY: all run clean

all: $(BINS)

sr_O0: $(SRC) ; $(CXX) $(CXXFLAGS) -O0 -o $@ $<
sr_O1: $(SRC) ; $(CXX) $(CXXFLAGS) -O1 -o $@ $<
sr_O2: $(SRC) ; $(CXX) $(CXXFLAGS) -O2 -o $@ $<
sr_O3: $(SRC) ; $(CXX) $(CXXFLAGS) -O3 -o $@ $<

run: $(BINS)
	@for lvl in O0 O1 O2 O3; do \
		echo ""; \
		echo "======== -$$lvl ========"; \
		./sr_$$lvl; \
	done

clean:
	rm -f $(BINS)
