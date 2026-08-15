// Test: Interrupt handler support — Phase 5A.3
// Tests `interrupt pf` syntax and interrupt control builtins.
// This program defines an interrupt handler and uses cli/sti/hlt builtins.

// Interrupt handler: called by the CPU when an interrupt occurs.
// The codegen saves/restores all registers and uses `iret` instead of `ret`.
interrupt pf timer_handler() {
    // In a real OS, this would handle a timer interrupt.
    // For testing, we just return.
    ret 0;
}

pf main() {
    // Disable interrupts
    p_cli();

    // Enable interrupts
    p_sti();

    // Halt the CPU
    p_hlt();

    ret 0;
}
