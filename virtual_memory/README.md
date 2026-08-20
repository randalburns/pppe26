# Virtual memory

A measured description of the virtual memory system on one machine: an **AMD Ryzen AI 9 HX 370**,
Linux, gcc 13.3, 48-bit addresses, 4 KB pages, 15.3 GB of RAM and 15.3 GB of swap.

This directory is about the **operating system's** memory abstraction — address spaces,
translation, page faults, copy-on-write, sharing and protection. What happens to an address
*after* it has been translated — cache levels, line sizes, the TLB — is
[../memory_hierarchy/](../memory_hierarchy/README.md).

Five programs, none of which needs privileges. They read `/proc/self/{maps,status,smaps_rollup,
pagemap}` and take fault counts from `getrusage`.

---

## Start here

Two pages, both standalone — double-click either.

| | |
|---|---|
| [vm_intro.html](vm_intro.html) | **Three Illusions** — a gentle introduction. What virtual memory, `malloc` and `fork` actually do, from first principles, with no measurements. Start here if the ideas are new. |
| [vm_reality.html](vm_reality.html) | **The Address Is Not The Memory** — the same mechanisms measured on this machine, with the numbers. |

Or run the programs yourself:

```
make            # build all five
make data       # run them all and refresh the .txt / .csv outputs (~30 s)
```

---

## What this machine turns out to do

| | Measured | Program |
|---|---|---|
| Address space a process may use | **128 TiB** (2<sup>47</sup>) | `address_space` |
| Fraction of it actually mapped | **0.000050 %** | `address_space` |
| Mapped memory that is resident | **4.1 %** — 66.6 MB virtual, 2.7 MB real | `address_space` |
| Largest reservation accepted | **512 GB** with `MAP_NORESERVE`, **16 GB** without | `lazy_alloc` |
| Cost of reserving 64 GB | +65,536 MB virtual, **+0.1 MB resident** | `lazy_alloc` |
| Page tables per 1 GB touched | 44 KB → **2,100 KB** (8 B per 4 KB page) | `lazy_alloc` |
| First touch of a 4 KB page | **853 ns**, one minor fault each | `fault_cost` |
| First touch, 2 MB pages | **277 ns** per 4 KB — 512× fewer faults | `fault_cost` |
| A page already present | **12 ns**, no fault | `fault_cost` |
| A major fault (page on the SSD) | **55.2 µs** | `fault_cost` |
| `fork` of a 256 MB process | **2.3 ms**, 0 bytes copied | `cow_fork` |
| Each page the child then writes | **1,308 ns**, one copy-on-write fault | `cow_fork` |
| Same virtual address in two processes | different memory, or the same — one `mmap` flag | `same_address` |

---

## The programs

| Program | What it shows | Emits |
|---|---|---|
| [address_space.c](address_space.c) | what a process's address space really contains: 25 regions, not 4; virtual vs resident; how little of 128 TiB is used; ASLR | `address_space.txt`, `regions.csv` |
| [lazy_alloc.c](lazy_alloc.c) | allocation hands out addresses, not memory; where the overcommit ceiling is; resident size tracking touched bytes page for page | `lazy_alloc.txt`, `lazy_alloc.csv` |
| [fault_cost.c](fault_cost.c) | what a page fault costs — anonymous, huge-page, prefaulted, and file-backed with a cold page cache | `fault_cost.txt`, `fault_cost.csv` |
| [cow_fork.c](cow_fork.c) | `fork` shares frames until a write; the `Shared_Dirty` → `Private_Dirty` transition, page by page | `cow_fork.txt`, `cow_fork.csv` |
| [same_address.c](same_address.c) | two processes at the identical virtual address, seeing their own memory or each other's depending on one flag | `same_address.txt` |

---

## Measurement notes

**`_exit` does not flush stdio.** Both fork-based programs lost all of their child output the first
time. Flush before forking too, or the child inherits the parent's buffer and reprints it.

**Argument evaluation order is unspecified.** `report(name, touch(...), dmin, dmaj)` reads the fault
counters *before* `touch` writes them, on this compiler. Call it first, into a variable.

**Sequence with pipes, not sleeps.** `same_address` has to guarantee that B stores its own value
before A writes, or the shared case races and always looks like isolation.

**Cold means cold.** The scratch file must live on the NVMe, not tmpfs, and its pages are evicted
with `posix_fadvise(POSIX_FADV_DONTNEED)` — no root and no dropping the system's caches.

**Readahead hides major faults.** Reading a cold 256 MB file sequentially costs 2,052 faults and
*one* blocking read, not 65,536. To see what a page actually costs to fetch, touch it in random
order and turn readahead off with `POSIX_FADV_RANDOM`.

**Physical frame numbers are not readable.** `/proc/self/pagemap` zeroes the PFN without
`CAP_SYS_ADMIN`, so `same_address` demonstrates frame identity behaviourally. The present and
swapped bits are still readable and `lazy_alloc` uses them.

---

## Where this connects

| Topic | Directory |
|---|---|
| Caches, and the TLB's capacity cliff | [../memory_hierarchy/](../memory_hierarchy/README.md) |
| Threads, which share all of this | [../openmp/](../openmp/) &middot; [../cilk/](../cilk/README.md) |
| False sharing: two threads, one frame, no logical sharing | [../false_sharing/](../false_sharing/false_sharing_ryzen.md) |
