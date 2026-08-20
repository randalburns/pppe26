# Virtual memory measurements.
#
#   make            build everything
#   make data       run every program and refresh the .csv / .txt outputs
#
# Nothing here needs privileges. The programs read /proc/self/{maps,status,
# smaps_rollup,pagemap} and use getrusage for fault counts, all of which are
# available to an ordinary user. Physical frame numbers in pagemap are zeroed
# without CAP_SYS_ADMIN, so same_address.c demonstrates frame identity
# behaviourally instead of by printing PFNs.

CC      = gcc
CFLAGS  = -O2 -Wall
SCRATCH = /tmp/vm_fault_scratch.bin

BINS = address_space lazy_alloc fault_cost cow_fork same_address

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

# Roughly 30 seconds total. fault_cost writes and then evicts a 256 MB scratch
# file, so give it a path on a real filesystem rather than tmpfs, or the
# "cold" cases are served from RAM and measure nothing.
data: all
	./address_space          > address_space.txt
	./lazy_alloc             > lazy_alloc.txt
	./fault_cost $(SCRATCH)  > fault_cost.txt
	./cow_fork               > cow_fork.txt
	./same_address           > same_address.txt

clean:
	rm -f $(BINS)

.PHONY: all data clean
