/* same_address.c -- the same address is not the same memory.
 *
 * "The addresses don't correspond to hardware addresses" is the usual one-line
 * summary of virtual memory, and it is easy to nod at without believing.  This
 * makes it concrete: two processes are forced to use the *identical* virtual
 * address -- not similar, not analogous, the same 64-bit number -- and then
 * asked what they find there.
 *
 * Twice, with one difference:
 *
 *   MAP_PRIVATE | MAP_ANONYMOUS   each process gets its own physical frame.
 *                                 Same address, different memory, no visibility.
 *   MAP_SHARED  | file            both processes get the same physical frame.
 *                                 Same address, same memory, writes visible.
 *
 * The address is fixed with MAP_FIXED_NOREPLACE so the kernel refuses rather
 * than silently unmapping something already there.
 *
 * Children are sequenced with pipes rather than sleeps, so the ordering of the
 * write and the read is guaranteed, not merely likely.
 *
 * Output: report on stdout.
 *
 * Build: gcc -O2 -o same_address same_address.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/mman.h>

/* an address chosen to be well clear of anything the loader placed */
#define ADDR ((void *)0x0000600000000000UL)
#define LEN  4096

static void wait_byte(int fd) { char c; if (read(fd, &c, 1) != 1) _exit(9); }
static void post_byte(int fd) { if (write(fd, "x", 1) != 1) _exit(9); }

static void run(const char *label, int shared, int fd_file) {
    int a2b[2], b2a[2];
    if (pipe(a2b) || pipe(b2a)) { perror("pipe"); exit(1); }

    printf("\n%s\n", label);
    fflush(stdout);              /* or every child inherits and reprints it */
    for (int who = 0; who < 2; who++) {
        pid_t pid = fork();
        if (pid) continue;

        int flags = MAP_FIXED_NOREPLACE |
                    (shared ? MAP_SHARED : (MAP_PRIVATE | MAP_ANONYMOUS));
        char *p = mmap(ADDR, LEN, PROT_READ|PROT_WRITE, flags, shared ? fd_file : -1, 0);
        if (p == MAP_FAILED) { perror("  mmap"); _exit(1); }
        if (p != ADDR)       { fprintf(stderr, "  kernel moved the mapping\n"); _exit(1); }

        /* Ordering matters: B must record its own idea of this address FIRST,
         * then A writes, then B looks again.  If A wrote first, B's own store
         * would clobber it in the shared case and the test would always claim
         * isolation. */
        if (who == 0) {                       /* A: writes second */
            wait_byte(b2a[0]);                /* B has stored its value */
            strcpy(p, "written by process A");
            printf("  A writes  \"%s\"  at %p\n", p, (void *)p);
            fflush(stdout);
            post_byte(a2b[1]);
        } else {                              /* B: writes first, reads last */
            memset(p, 0, LEN);
            strcpy(p, "written by process B");
            post_byte(b2a[1]);                /* A may now write */
            wait_byte(a2b[0]);                /* A has written */
            printf("  B reads   \"%s\"  at %p\n", p, (void *)p);
            fflush(stdout);
        }
        _exit(0);
    }
    close(a2b[0]); close(a2b[1]); close(b2a[0]); close(b2a[1]);
    wait(NULL); wait(NULL);
}

int main(void) {
    printf("both processes map the very same virtual address: %p\n", ADDR);

    fflush(stdout);
    run("private + anonymous  ->  each process gets its own frame", 0, -1);

    /* a small file, so both can map the same physical pages */
    char path[] = "/tmp/vm_same_addr_XXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) { perror("mkstemp"); return 1; }
    if (ftruncate(fd, LEN)) { perror("ftruncate"); return 1; }

    fflush(stdout);
    run("shared + file-backed  ->  both processes get the same frame", 1, fd);

    close(fd); unlink(path);
    printf("\nSame number, opposite outcomes. The address is not the memory;\n");
    printf("the page table entry behind it is.\n");
    return 0;
}
