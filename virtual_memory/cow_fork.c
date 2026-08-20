/* cow_fork.c -- fork copies an address space without copying memory.
 *
 * fork() hands the child a complete, private copy of the parent's address
 * space.  Doing that literally would mean copying every resident page.  The
 * kernel does not: it marks every writable page read-only in both processes
 * and lets them share the physical frames until somebody writes.  The write
 * traps, the kernel copies that one page, and both sides continue.
 *
 * This measures the three states directly out of /proc/self/smaps_rollup,
 * which separates pages by whether they are shared with another process and
 * whether they have been written:
 *
 *   Shared_Clean    still the parent's frames, untouched since fork
 *   Private_Dirty   copied, and now this process's own
 *
 * The child reads the whole region first (which copies nothing) and then
 * writes it (which copies everything), so the two effects are separated.
 *
 * Output: report on stdout, plus cow_fork.csv
 *
 * Build: gcc -O2 -o cow_fork cow_fork.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/resource.h>

#define PAGE  4096
#define BYTES (256UL << 20)

static double now_s(void) {
    struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}
static long rollup(const char *key) {
    FILE *f = fopen("/proc/self/smaps_rollup", "r");
    char line[256]; long v = -1;
    while (f && fgets(line, sizeof line, f))
        if (!strncmp(line, key, strlen(key))) { sscanf(line + strlen(key), " %ld", &v); break; }
    if (f) fclose(f);
    return v;
}
static long minflt(void) {
    struct rusage r; getrusage(RUSAGE_SELF, &r); return r.ru_minflt;
}
/* The parent wrote every page before forking, so after the fork those pages
 * are *dirty and shared*: two processes map the same frames.  Watching
 * Shared_Dirty fall as Private_Dirty rises is the copy-on-write happening. */
static void show(const char *when) {
    printf("  %-30s shared_dirty %7.1f MB   private_dirty %7.1f MB\n",
           when, rollup("Shared_Dirty:") / 1024.0, rollup("Private_Dirty:") / 1024.0);
}

int main(void) {
    char *p = mmap(NULL, BYTES, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    if (p == MAP_FAILED) { perror("mmap"); return 1; }
    madvise(p, BYTES, MADV_NOHUGEPAGE);        /* count in 4 KB pages */
    memset(p, 0xA5, BYTES);                    /* make every page resident and dirty */

    printf("parent, %zu MB written and resident:\n", BYTES >> 20);
    show("before fork");

    FILE *csv = fopen("cow_fork.csv", "w");
    fprintf(csv, "phase,shared_dirty_mb,private_dirty_mb,ns_per_page,faults\n");
    fprintf(csv, "parent before fork,%.1f,%.1f,,\n",
            rollup("Shared_Dirty:")/1024.0, rollup("Private_Dirty:")/1024.0);

    fflush(stdout); fflush(csv);   /* do not fork a dirty stdio buffer */
    double t0 = now_s();
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {
        double fork_ms = (now_s() - t0) * 1e3;
        /* sample at each phase; writing the csv at the end would record
         * post-write numbers under pre-write labels */
        double sd_fork = rollup("Shared_Dirty:")/1024.0, pd_fork = rollup("Private_Dirty:")/1024.0;
        printf("\nchild, immediately after fork (%.1f ms for the fork itself):\n", fork_ms);
        show("after fork");

        /* reading touches nothing: the pages are already present, just shared */
        long f0 = minflt();
        double r0 = now_s();
        volatile char sink = 0;
        for (size_t off = 0; off < BYTES; off += PAGE) sink ^= p[off];
        double rd = now_s() - r0;
        long rf = minflt() - f0;
        (void)sink;
        printf("\nchild reads all %zu MB:\n", BYTES >> 20);
        printf("  %-34s %8.1f ms  %6.0f ns/page  %ld faults\n",
               "read pass", rd * 1e3, rd * 1e9 / (BYTES / PAGE), rf);
        show("after reading");
        double sd_read = rollup("Shared_Dirty:")/1024.0, pd_read = rollup("Private_Dirty:")/1024.0;

        /* writing copies: one copy-on-write fault per page */
        f0 = minflt();
        double w0 = now_s();
        for (size_t off = 0; off < BYTES; off += PAGE) p[off] = 1;
        double wr = now_s() - w0;
        long wf = minflt() - f0;
        printf("\nchild writes one byte in every page:\n");
        printf("  %-34s %8.1f ms  %6.0f ns/page  %ld faults\n",
               "write pass", wr * 1e3, wr * 1e9 / (BYTES / PAGE), wf);
        show("after writing");

        FILE *c = fopen("cow_fork.csv", "a");
        fprintf(c, "child after fork,%.1f,%.1f,,\n", sd_fork, pd_fork);
        fprintf(c, "child read pass,%.1f,%.1f,%.1f,%ld\n",
                sd_read, pd_read, rd * 1e9 / (BYTES/PAGE), rf);
        fprintf(c, "child write pass,%.1f,%.1f,%.1f,%ld\n",
                rollup("Shared_Dirty:")/1024.0, rollup("Private_Dirty:")/1024.0,
                wr * 1e9 / (BYTES/PAGE), wf);
        fclose(c);
        fflush(stdout);   /* _exit does not flush stdio */
        _exit(0);
    }

    wait(NULL);
    printf("\nparent, after the child copied everything:\n");
    show("after child exits");
    fclose(csv);
    munmap(p, BYTES);
    return 0;
}
