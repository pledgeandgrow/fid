// Test: MMU/page table management — Phase 5A.4
// Tests control register access, TLB invalidation, and page mapping builtins.

pf main() {
    // Read CR0 (control register 0)
    pv cr0 = p_cr0_read();

    // Read CR3 (page table base)
    pv cr3 = p_cr3_read();

    // Read CR4 (control register 4)
    pv cr4 = p_cr4_read();

    // Write CR3 (set page table base)
    p_cr3_write(cr3);

    // Invalidate TLB entry for a virtual address
    p_invlpg(0x1000);

    // Map a virtual page to a physical page
    // args: page_table, virt_addr, phys_addr, flags
    p_page_map(cr3, 0x2000, 0x8000, 0x3);

    // Unmap a virtual page
    p_page_unmap(cr3, 0x2000);

    // Read from physical address (identity mapped in bare mode)
    pv val = p_phys_read64(0x10000);

    // Write to physical address
    p_phys_write64(0x10000, 42);

    ret 0;
}
