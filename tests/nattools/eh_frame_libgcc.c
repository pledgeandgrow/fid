/* DB6 (docs/roadmap/llvm-roadmap.md): libgcc's unwinder reads the
 * .eh_frame of a machine-layer executable (compiler/nat_enc.fd
 * nat_elf_eh_frame).
 *
 *   gcc -O1 -o eh_frame_libgcc tests/nattools/eh_frame_libgcc.c
 *   ./eh_frame_libgcc <x86-64 or any ELF64 LE executable written by fid emit>
 *
 * The PT_LOAD holding .eh_frame is mapped at its own address (the
 * pc-relative addresses in it then mean what they say), the frame
 * section is registered with libgcc (__register_frame) and, for every
 * entry of the .eh_frame_hdr search table, _Unwind_Find_FDE is asked for
 * the FDE of an address inside the function (its start, and the last
 * byte the FDE covers). Each answer must be the FDE the table names, and
 * libgcc's decoding of its pc_begin (`bases.func`) the table's start.
 * Prints "ok: N functions found by libgcc" and exits 0, or names the
 * first wrong one and exits 1. Needs a host whose address space has the
 * executable's addresses free (a PIE host program, as gcc builds by
 * default).
 */
#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

struct dwarf_eh_bases {
    void *tbase;
    void *dbase;
    void *func;
};

extern void __register_frame(void *begin);
extern void __deregister_frame(void *begin);
extern const void *_Unwind_Find_FDE(void *pc, struct dwarf_eh_bases *bases);

static uint64_t rd(const unsigned char *b, size_t at, int n) {
    uint64_t v = 0;
    for (int i = n - 1; i >= 0; i--) v = (v << 8) | b[at + i];
    return v;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <elf>\n", argv[0]);
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        perror(argv[1]);
        return 2;
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    unsigned char *d = malloc((size_t)size);
    if (fread(d, 1, (size_t)size, f) != (size_t)size) {
        fprintf(stderr, "short read\n");
        return 2;
    }
    fclose(f);
    uint64_t phoff = rd(d, 0x20, 8);
    int phnum = (int)rd(d, 0x38, 2);
    uint64_t hdr_off = 0, hdr_addr = 0;
    int have = 0;
    for (int i = 0; i < phnum; i++) {
        size_t at = phoff + 56 * (size_t)i;
        if (rd(d, at, 4) == 0x6474E550) {
            hdr_off = rd(d, at + 8, 8);
            hdr_addr = rd(d, at + 16, 8);
            have = 1;
        }
    }
    if (!have) {
        printf("FAIL no PT_GNU_EH_FRAME\n");
        return 1;
    }
    /* map every PT_LOAD at its address (read/write, the file's bytes) */
    for (int i = 0; i < phnum; i++) {
        size_t at = phoff + 56 * (size_t)i;
        if (rd(d, at, 4) != 1) continue;
        uint64_t off = rd(d, at + 8, 8), va = rd(d, at + 16, 8);
        uint64_t fs = rd(d, at + 32, 8), ms = rd(d, at + 40, 8);
        uint64_t page = va & ~(uint64_t)0xFFF;
        uint64_t len = ((va + ms + 0xFFF) & ~(uint64_t)0xFFF) - page;
        void *m = mmap((void *)page, len, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (m == MAP_FAILED || m != (void *)page) {
            printf("FAIL cannot map %#llx+%#llx here\n", (unsigned long long)page, (unsigned long long)len);
            return 1;
        }
        memcpy((void *)va, d + off, fs);
    }
    const unsigned char *h = (const unsigned char *)hdr_addr;
    if (h[0] != 1 || h[1] != 0x1B || h[2] != 0x03 || h[3] != 0x3B) {
        printf("FAIL header encodings %02x %02x %02x %02x\n", h[0], h[1], h[2], h[3]);
        return 1;
    }
    unsigned char *eh = (unsigned char *)(hdr_addr + 4 + (int64_t)(int32_t)rd(h, 4, 4));
    int n = (int)rd(h, 8, 4);
    __register_frame(eh);
    int found = 0;
    for (int i = 0; i < n; i++) {
        uint64_t loc = hdr_addr + (int64_t)(int32_t)rd(h, 12 + 8 * (size_t)i, 4);
        uint64_t fde = hdr_addr + (int64_t)(int32_t)rd(h, 16 + 8 * (size_t)i, 4);
        uint64_t range = rd((const unsigned char *)fde, 12, 4);
        if (range == 0) continue;
        uint64_t pcs[2] = { loc, loc + range - 1 };
        for (int k = 0; k < 2; k++) {
            struct dwarf_eh_bases bases;
            memset(&bases, 0, sizeof bases);
            const void *got = _Unwind_Find_FDE((void *)pcs[k], &bases);
            if ((uint64_t)got != fde || (uint64_t)bases.func != loc) {
                printf("FAIL pc %#llx: libgcc found FDE %p (function %p), the table says %#llx (function %#llx)\n",
                       (unsigned long long)pcs[k], got, bases.func, (unsigned long long)fde, (unsigned long long)loc);
                return 1;
            }
        }
        found++;
    }
    __deregister_frame(eh);
    (void)hdr_off;
    printf("ok: %d functions found by libgcc\n", found);
    return found > 0 ? 0 : 1;
}
