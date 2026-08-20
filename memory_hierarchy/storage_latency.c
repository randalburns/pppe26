/* storage_latency.c -- the bottom two rows of the hierarchy, measured.
 *
 * The classic hierarchy table ends with "10^5 cycles to NVRAM, 10^7 cycles to
 * magnetic disk".  Neither line describes a 2026 laptop: Optane NVRAM was
 * discontinued in 2022 and there is no spinning disk in this machine.  What is
 * actually below DRAM here is one NVMe SSD, and it is worth measuring rather
 * than quoting, because it is roughly three orders of magnitude faster than
 * the "disk" row every textbook figure still shows.
 *
 * Two numbers, both with O_DIRECT so the kernel page cache is bypassed and we
 * see the device rather than DRAM:
 *   - random 4 KB read latency at queue depth 1 (the analogue of the pointer
 *     chase: one dependent access at a time)
 *   - sequential 1 MB read bandwidth (the analogue of the streaming test)
 *
 * Usage: storage_latency <scratch-file-path> [size_mb]
 * The scratch file is created, used, and removed.
 *
 * Build: gcc -O2 -o storage_latency storage_latency.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>

static double now_s(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + 1e-9 * ts.tv_nsec;
}

static uint64_t rng_state = 987654321987654321ULL;
static uint64_t rng(void) {
    rng_state ^= rng_state << 13; rng_state ^= rng_state >> 7; rng_state ^= rng_state << 17;
    return rng_state;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: %s <path> [size_mb]\n", argv[0]); return 1; }
    const char *path = argv[1];
    size_t size_mb = (argc > 2) ? strtoul(argv[2], NULL, 10) : 2048;
    size_t bytes = size_mb << 20;

    /* lay down a file larger than free DRAM would let the page cache hold,
     * then reopen it O_DIRECT so every read really goes to the device */
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    void *buf;
    if (posix_memalign(&buf, 4096, 1 << 20)) { perror("memalign"); return 1; }
    memset(buf, 0xa5, 1 << 20);
    for (size_t off = 0; off < bytes; off += (1 << 20))
        if (write(fd, buf, 1 << 20) != (1 << 20)) { perror("write"); return 1; }
    fsync(fd);
    close(fd);

    fd = open(path, O_RDONLY | O_DIRECT);
    if (fd < 0) { perror("open O_DIRECT"); return 1; }

    printf("test,value,unit\n");

    /* random 4 KB reads, queue depth 1 */
    const int NREAD = 5000;
    size_t nblocks = bytes / 4096;
    double t0 = now_s();
    for (int i = 0; i < NREAD; i++) {
        off_t off = (off_t)(rng() % nblocks) * 4096;
        if (pread(fd, buf, 4096, off) != 4096) { perror("pread"); return 1; }
    }
    double dt = now_s() - t0;
    printf("random_4k_read_latency,%.1f,us\n", dt * 1e6 / NREAD);

    /* sequential 1 MB reads */
    const int NSEQ = 512;
    t0 = now_s();
    for (int i = 0; i < NSEQ; i++)
        if (pread(fd, buf, 1 << 20, (off_t)i * (1 << 20)) != (1 << 20)) { perror("pread"); return 1; }
    dt = now_s() - t0;
    printf("sequential_read_bandwidth,%.2f,GB/s\n", (double)NSEQ * (1 << 20) / dt / 1e9);

    close(fd);
    unlink(path);
    free(buf);
    return 0;
}
