/* address_space.c -- what a process's virtual address space actually contains.
 *
 * The textbook picture of virtual memory is a tidy stack of four boxes: code,
 * data, heap, stack.  A real process on this machine has dozens of regions,
 * almost all of its address space is empty, and almost all of what it *has*
 * mapped has no physical memory behind it.  This reads /proc/self/maps and
 * /proc/self/status and reports what is really there.
 *
 * Three numbers matter and they are all different:
 *   VmSize  virtual bytes mapped        -- what the process can address
 *   VmRSS   resident bytes              -- what it actually costs in DRAM
 *   the address space itself            -- 128 TiB, essentially all unused
 *
 * Output: a human-readable report on stdout, plus regions.csv.
 *
 * Build: gcc -O2 -o address_space address_space.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

/* x86-64 gives userspace the low half of a 48-bit address space: 47 bits. */
#define USER_VA_BITS 47

static long status_kb(const char *key) {
    FILE *f = fopen("/proc/self/status", "r");
    char line[256]; long v = -1;
    while (f && fgets(line, sizeof line, f))
        if (!strncmp(line, key, strlen(key))) { sscanf(line + strlen(key), " %ld", &v); break; }
    if (f) fclose(f);
    return v;
}

/* Classify a mapping by its permissions and backing path -- the categories the
 * textbook diagram draws, plus the ones it leaves out. */
static const char *classify(const char *perms, const char *path) {
    if (!strcmp(path, "[stack]"))    return "stack";
    if (!strcmp(path, "[heap]"))     return "heap";
    if (!strcmp(path, "[vdso]") || !strcmp(path, "[vvar]") ||
        !strcmp(path, "[vsyscall]")) return "kernel-provided";
    if (path[0] == '\0')             return "anonymous";
    if (perms[2] == 'x')             return "code (file-backed)";
    if (perms[1] == 'w')             return "data (file-backed)";
    return "read-only (file-backed)";
}

int main(void) {
    /* give the process something in each category to look at */
    void *heap = malloc(64 << 20);
    if (heap) memset(heap, 1, 1 << 20);          /* touch only the first MB */
    int stack_var = 0;

    FILE *maps = fopen("/proc/self/maps", "r");
    FILE *csv  = fopen("regions.csv", "w");
    if (!maps || !csv) { perror("open"); return 1; }
    fprintf(csv, "kind,perms,size_kb,path\n");

    char line[512];
    unsigned long lo, hi, total = 0, exec = 0, writ = 0, nregions = 0;
    unsigned long by_kind[8] = {0};
    const char *kinds[8] = {"code (file-backed)", "read-only (file-backed)",
                            "data (file-backed)", "heap", "stack", "anonymous",
                            "kernel-provided", "other"};

    printf("%-26s %-6s %10s  %s\n", "KIND", "PERMS", "SIZE", "BACKING");
    printf("%.78s\n", "----------------------------------------"
                      "----------------------------------------");
    while (fgets(line, sizeof line, maps)) {
        char perms[8] = {0}, path[256] = {0};
        if (sscanf(line, "%lx-%lx %7s %*s %*s %*s %255[^\n]", &lo, &hi, perms, path) < 3) continue;
        char *p = path; while (*p == ' ') p++;
        unsigned long kb = (hi - lo) >> 10;
        const char *kind = classify(perms, p);

        total += hi - lo;
        if (perms[2] == 'x') exec += hi - lo;
        if (perms[1] == 'w') writ += hi - lo;
        nregions++;
        for (int i = 0; i < 8; i++) if (!strcmp(kind, kinds[i])) { by_kind[i] += hi - lo; break; }

        /* print only the notable ones; the full list goes to the csv */
        if (kb >= 1024 || p[0] == '[')
            printf("%-26s %-6s %8lu K  %s\n", kind, perms, kb, p[0] ? p : "-");
        fprintf(csv, "%s,%s,%lu,%s\n", kind, perms, kb, p[0] ? p : "-");
    }
    fclose(maps); fclose(csv);

    long vmsize = status_kb("VmSize:"), vmrss = status_kb("VmRSS:"), vmpte = status_kb("VmPTE:");
    double user_va_gib = (double)(1UL << USER_VA_BITS) / (1UL << 30);

    printf("\n%.78s\n", "========================================"
                        "========================================");
    printf("regions mapped            %lu\n", nregions);
    printf("  executable              %8.1f MB\n", exec / 1048576.0);
    printf("  writable                %8.1f MB\n", writ / 1048576.0);
    for (int i = 0; i < 8; i++)
        if (by_kind[i]) printf("  %-22s %8.1f MB\n", kinds[i], by_kind[i] / 1048576.0);

    printf("\nvirtual size  (VmSize)    %8.1f MB   what the process can address\n", vmsize / 1024.0);
    printf("resident      (VmRSS)     %8.1f MB   what it costs in DRAM\n", vmrss / 1024.0);
    printf("page tables   (VmPTE)     %8.1f MB   the cost of the translation itself\n", vmpte / 1024.0);
    printf("resident / virtual        %8.1f %%\n", 100.0 * vmrss / vmsize);

    printf("\nuser address space        %8.0f GB   (2^%d bytes)\n", user_va_gib, USER_VA_BITS);
    printf("fraction mapped           %8.6f %%\n", 100.0 * total / (double)(1UL << USER_VA_BITS));

    printf("\nthis run's addresses (they move every run -- ASLR):\n");
    printf("  code   (main)           %#018lx\n", (unsigned long)(uintptr_t)main);
    printf("  heap   (malloc)         %#018lx\n", (unsigned long)(uintptr_t)heap);
    printf("  stack  (local)          %#018lx\n", (unsigned long)(uintptr_t)&stack_var);
    printf("  libc   (printf)         %#018lx\n", (unsigned long)(uintptr_t)printf);
    return 0;
}
