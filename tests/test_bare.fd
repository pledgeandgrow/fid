// Test: Bare-metal mode — Phase 5A.2
// Tests --bare flag: no stdlib, inline asm, raw binary output.
// This program uses only built-in language features (no stdlib functions).

pf main() {
    // Inline assembly: halt the CPU (bare-metal, no OS calls)
    asm!("hlt");
    ret 0;
}
