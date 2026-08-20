/* lazy_alloc.c -- allocation does not allocate.
 *
 * A successful malloc on Linux buys you *addresses*, not memory.  Physical
 * pages are attached one at a time, on the first touch of each page, by the
 * page-fault handler.  Until then the mapping is a promise the kernel has not
 * had to keep.
 *
 * Two things get measured here:
 *
 *   1. How large a reservation this machine will accept.  With MAP_NORESERVE
 *      the answer is "as much address space as you like".  Without it, the
 *      kernel's heuristic (vm.overcommit_memory = 0) refuses anything it could
 *      not possibly honour, which puts the ceiling near RAM + swap.
 *
 *   2. What actually arrives when you touch it.  Resident size is sampled
 *      against bytes touched, alongside a direct count of present pages read
 *      out of /proc/self/pagemap -- so the claim "one page per fault" is
 *      checked at page granularity rather than inferred.
 *
 * Output: report on stdout, plus lazy_alloc.csv.
 *
 * Build: gcc -O2 -o lazy_alloc lazy_alloc.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define PAGE 4096

static long status_kb(const char *key) {
    FILE *f = fopen("/proc/self/status", "r");
    char line[256]; long v = -1;
    while (f && fgets(line, sizeof line, f))
        if (!strncmp(line, key, strlen(key))) { sscanf(line + strlen(key), " %ld", &v); break; }
    if (f) fclose(f);
    return v;
}

/* How many pages of [base, base+len) does the kernel say are actually present?
 * Bit 63 of a pagemap entry is the present bit; the PFN itself is zeroed for
 * unprivileged readers, but the flag is all we need. */
static long present_pages(void *base, size_t len) {
    int fd = open("/proc/self/pagemap", O_RDONLY);
    if (fd < 0) return -1;
    long n = len / PAGE, present = 0;
    uint64_t *buf = malloc(n * 8);
    uint64_t vpn = (uint64_t)base / PAGE;
    if (pread(fd, buf, n * 8, vpn * 8) == (ssize_t)(n * 8))
        for (long i = 0; i < n; i++) present += (buf[i] >> 63) & 1;
    else present = -1;
    free(buf); close(fd);
    return present;
}

int main(void) {
    printf("=== 1. how much can be reserved ===\n");
    printf("%10s  %-12s  %-12s\n", "size", "MAP_NORESERVE", "default");
    for (double gb = 8; gb <= 512; gb *= 2) {
        size_t n = (size_t)(gb * (1UL << 30));
        void *a = mmap(NULL, n, PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
        void *b = mmap(NULL, n, PROT_READ|PROT_WRITE,
                       MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        printf("%8.0f GB  %-12s  %-12s\n", gb,
               a == MAP_FAILED ? "refused" : "reserved",
               b == MAP_FAILED ? "refused" : "reserved");
        if (a != MAP_FAILED) munmap(a, n);
        if (b != MAP_FAILED) munmap(b, n);
    }

    /* ---- 2. reserve far more than this machine has, then touch a little ---- */
    const size_t RESERVE = 64UL << 30;          /* 64 GB on a 15 GB machine */
    const size_t TOUCH   = 1UL  << 30;          /* but only ever touch 1 GB */

    long rss_before = status_kb("VmRSS:"), vsz_before = status_kb("VmSize:");
    char *p = mmap(NULL, RESERVE, PROT_READ|PROT_WRITE,
                   MAP_PRIVATE|MAP_ANONYMOUS|MAP_NORESERVE, -1, 0);
    if (p == MAP_FAILED) { perror("mmap"); return 1; }
    long rss_after = status_kb("VmRSS:"), vsz_after = status_kb("VmSize:");

    printf("\n=== 2. reserving %zu GB on a machine with 15 GB of RAM ===\n", RESERVE >> 30);
    printf("VmSize  %8.1f MB -> %8.1f MB   (+%.0f MB)\n",
           vsz_before/1024.0, vsz_after/1024.0, (vsz_after-vsz_before)/1024.0);
    printf("VmRSS   %8.1f MB -> %8.1f MB   (+%.1f MB)  <- nothing arrived\n",
           rss_before/1024.0, rss_after/1024.0, (rss_after-rss_before)/1024.0);

    printf("\n=== 3. what arrives as it is touched ===\n");
    printf("%12s %12s %12s %10s %13s %9s\n",
           "touched MB", "VmSize MB", "VmRSS MB", "VmPTE KB", "present pages", "expected");
    FILE *csv = fopen("lazy_alloc.csv", "w");
    fprintf(csv, "touched_mb,vmsize_mb,vmrss_mb,vmpte_kb,present_pages,expected_pages\n");

    size_t steps[] = {0, 1, 4, 16, 64, 256, 512, 1024};
    for (unsigned s = 0; s < sizeof steps / sizeof steps[0]; s++) {
        size_t upto = steps[s] << 20;
        for (size_t off = (s ? steps[s-1] << 20 : 0); off < upto; off += PAGE)
            p[off] = 1;                          /* one byte per page */
        long rss = status_kb("VmRSS:"), vsz = status_kb("VmSize:");
        long pte = status_kb("VmPTE:");   /* the page tables are memory too */
        /* sample presence over the first 64 MB only -- reading pagemap for the
         * whole 64 GB reservation would itself take longer than the experiment */
        size_t window = TOUCH < (64UL<<20) ? TOUCH : (64UL<<20);
        long present = present_pages(p, window);
        long expect  = (upto < window ? upto : window) / PAGE;
        printf("%12zu %12.1f %12.1f %10ld %13ld %9ld\n",
               steps[s], vsz/1024.0, rss/1024.0, pte, present, expect);
        fprintf(csv, "%zu,%.1f,%.1f,%ld,%ld,%ld\n",
                steps[s], vsz/1024.0, rss/1024.0, pte, present, expect);
    }
    fclose(csv);
    printf("\n(present pages counted over the first 64 MB of the mapping)\n");
    munmap(p, RESERVE);
    return 0;
}
