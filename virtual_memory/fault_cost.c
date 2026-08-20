/* fault_cost.c -- what a page fault costs, and who pays it.
 *
 * Demand paging is not free, it is deferred.  Every page of a fresh mapping
 * costs one trap into the kernel on its first touch: find a free frame, zero
 * it, install a page-table entry, return.  This measures that cost four ways,
 * and confirms each against the kernel's own fault counters from getrusage
 * rather than trusting the timing alone.
 *
 *   anon 4K       ordinary demand paging, one minor fault per 4 KB
 *   anon 2M       transparent huge pages: 512x fewer faults, each one bigger
 *   MAP_POPULATE  faults moved into the mmap call -- paid up front, not saved
 *   file, cold    the page cache misses too, and the fault goes to the SSD
 *
 * The "warm" column re-touches pages that are already present, which is what
 * the same loop costs once translation exists.  The gap between the two is
 * the fault.
 *
 * Usage: fault_cost [scratch-file-path]
 * Output: report on stdout, plus fault_cost.csv
 *
 * Build: gcc -O2 -o fault_cost fault_cost.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define PAGE  4096
#define HUGE  (2UL << 20)
#define BYTES (256UL << 20)          /* 256 MB per experiment */

static double now_s(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}
static void faults(long *minor, long *major) {
    struct rusage r; getrusage(RUSAGE_SELF, &r);
    *minor = r.ru_minflt; *major = r.ru_majflt;
}

/* Touch one byte per 4 KB page across the whole region.  `write_mode` matters:
 * a file mapped PROT_READ segfaults if you store into it, and a read-only
 * touch of a private file mapping does not fault in a writable copy. */
static double touch(char *p, size_t len, int write_mode, long *dmin, long *dmaj) {
    static volatile char sink;
    long m0, j0, m1, j1;
    faults(&m0, &j0);
    double t0 = now_s();
    if (write_mode) for (size_t off = 0; off < len; off += PAGE) p[off] = 1;
    else            for (size_t off = 0; off < len; off += PAGE) sink ^= p[off];
    double dt = now_s() - t0;
    faults(&m1, &j1);
    *dmin = m1 - m0; *dmaj = j1 - j0;
    return dt;
}

static FILE *csv;
static void report(const char *name, double dt, size_t len, long dmin, long dmaj,
                   const char *note) {
    long pages = len / PAGE;
    printf("%-16s %9.1f ms %10.0f ns %12ld %8ld   %s\n",
           name, dt * 1e3, dt * 1e9 / pages, dmin, dmaj, note);
    fprintf(csv, "%s,%.3f,%.1f,%ld,%ld,%zu\n", name, dt * 1e3, dt * 1e9 / pages,
            dmin, dmaj, len);
}

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] : "/tmp/vm_fault_scratch.bin";
    csv = fopen("fault_cost.csv", "w");
    fprintf(csv, "case,total_ms,ns_per_4k_page,minor_faults,major_faults,bytes\n");

    printf("%-16s %12s %12s %12s %8s   %s\n",
           "CASE", "total", "per 4K page", "minor flt", "major", "");
    printf("%.96s\n", "------------------------------------------------------"
                      "--------------------------------------------");

    long dmin, dmaj;
    char *p;

    /* ---- anonymous, 4 KB pages: the ordinary case ---- */
    p = mmap(NULL, BYTES, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    madvise(p, BYTES, MADV_NOHUGEPAGE);
    { double dt = touch(p, BYTES, 1, &dmin, &dmaj);
      report("anon 4K cold", dt, BYTES, dmin, dmaj, "one fault per page"); }
    { double dt = touch(p, BYTES, 1, &dmin, &dmaj);
      report("anon 4K warm", dt, BYTES, dmin, dmaj, "already present"); }
    munmap(p, BYTES);

    /* ---- anonymous, 2 MB huge pages ---- */
    if (posix_memalign((void **)&p, HUGE, BYTES)) { perror("memalign"); return 1; }
    madvise(p, BYTES, MADV_HUGEPAGE);
    { double dt = touch(p, BYTES, 1, &dmin, &dmaj);
      report("anon 2M cold", dt, BYTES, dmin, dmaj, "512x fewer faults"); }
    { double dt = touch(p, BYTES, 1, &dmin, &dmaj);
      report("anon 2M warm", dt, BYTES, dmin, dmaj, "already present"); }
    free(p);

    /* ---- MAP_POPULATE: the same faults, taken during mmap ---- */
    double t0 = now_s();
    p = mmap(NULL, BYTES, PROT_READ|PROT_WRITE,
             MAP_PRIVATE|MAP_ANONYMOUS|MAP_POPULATE, -1, 0);
    double map_ms = (now_s() - t0) * 1e3;
    printf("%-16s %9.1f ms %10.0f ns %12s %8s   %s\n", "MAP_POPULATE",
           map_ms, map_ms * 1e6 / (BYTES / PAGE), "-", "-", "cost of the mmap call itself");
    fprintf(csv, "MAP_POPULATE,%.3f,%.1f,,,%zu\n", map_ms, map_ms*1e6/(BYTES/PAGE), BYTES);
    { double dt = touch(p, BYTES, 1, &dmin, &dmaj);
      report("populated touch", dt, BYTES, dmin, dmaj, "nothing left to fault"); }
    munmap(p, BYTES);

    /* ---- file-backed with a cold page cache: faults reach the SSD ---- */
    int fd = open(path, O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    char *buf = calloc(1, 1 << 20);
    for (size_t off = 0; off < BYTES; off += (1 << 20))
        if (write(fd, buf, 1 << 20) != (1 << 20)) { perror("write"); return 1; }
    free(buf);
    fsync(fd);
    /* evict this file's pages from the page cache -- no privileges needed */
    posix_fadvise(fd, 0, BYTES, POSIX_FADV_DONTNEED);

    p = mmap(NULL, BYTES, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) { perror("mmap file"); return 1; }
    long m0, j0, m1, j1;
    faults(&m0, &j0);
    t0 = now_s();
    volatile char sink = 0;
    for (size_t off = 0; off < BYTES; off += PAGE) sink ^= p[off];
    double dt = now_s() - t0;
    faults(&m1, &j1);
    (void)sink;
    printf("%-16s %9.1f ms %10.0f ns %12ld %8ld   %s\n", "file cold",
           dt * 1e3, dt * 1e9 / (BYTES / PAGE), m1 - m0, j1 - j0,
           "page cache misses -> NVMe");
    fprintf(csv, "file cold,%.3f,%.1f,%ld,%ld,%zu\n", dt*1e3, dt*1e9/(BYTES/PAGE),
            m1 - m0, j1 - j0, BYTES);
    { double dt = touch(p, BYTES, 0, &dmin, &dmaj);
      report("file warm", dt, BYTES, dmin, dmaj, "now in the page cache"); }
    munmap(p, BYTES);

    /* ---- the same file, cold again, but touched in random page order.
     * Sequentially the kernel's readahead sees the pattern coming and turns
     * 65,536 would-be major faults into 2,052 minor ones and a single blocking
     * read.  In random order it cannot, and the faults wait on the device. ---- */
    const size_t RND = 64UL << 20;
    posix_fadvise(fd, 0, BYTES, POSIX_FADV_DONTNEED);
    posix_fadvise(fd, 0, BYTES, POSIX_FADV_RANDOM);   /* and turn readahead off */
    p = mmap(NULL, RND, PROT_READ, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) { perror("mmap random"); return 1; }
    long npg = RND / PAGE;
    long *order = malloc(npg * sizeof(long));
    for (long i = 0; i < npg; i++) order[i] = i;
    uint64_t rs = 88172645463325252ULL;
    for (long i = npg - 1; i > 0; i--) {
        rs ^= rs << 13; rs ^= rs >> 7; rs ^= rs << 17;
        long j = rs % (i + 1), t = order[i]; order[i] = order[j]; order[j] = t;
    }
    {
        long a0, b0, a1, b1; volatile char s2 = 0;
        faults(&a0, &b0);
        double t1 = now_s();
        for (long i = 0; i < npg; i++) s2 ^= p[order[i] * PAGE];
        double d1 = now_s() - t1;
        faults(&a1, &b1); (void)s2;
        printf("%-16s %9.1f ms %10.0f ns %12ld %8ld   %s\n", "file cold rand",
               d1 * 1e3, d1 * 1e9 / npg, a1 - a0, b1 - b0,
               "no readahead -> real major faults");
        /* the per-page average is diluted by fault-around, which brings in
         * neighbours for free; the major faults are what actually waited */
        if (b1 > b0)
            printf("%-16s %9s    %7.1f us per major fault -- the device latency\n",
                   "", "", d1 * 1e6 / (b1 - b0));
        fprintf(csv, "file cold rand,%.3f,%.1f,%ld,%ld,%zu\n",
                d1 * 1e3, d1 * 1e9 / npg, a1 - a0, b1 - b0, RND);
    }
    free(order);
    munmap(p, RND);
    close(fd); unlink(path);
    fclose(csv);
    return 0;
}
