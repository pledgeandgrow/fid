// Test: Inline assembly — Phase 5A.1
// Tests asm!() expression syntax and interpreter behavior

pf main() {
    // Test 1: Simple no-op asm (void return)
    asm!("nop");
    print("asm nop OK\n");

    // Test 2: Multiple instructions
    asm!("nop; hlt; cli; sti");
    print("asm multi OK\n");

    // Test 3: asm with input operands
    pv x = 42;
    asm!("nop", in("rax") x);
    print("asm input OK\n");

    // Test 4: asm with output operands (returns i64 in interpreter)
    pv result = asm!("nop", out("rax") x);
    print("asm output OK\n");
    print(result);
    print("\n");

    // Test 5: asm with both input and output
    pv a = 10;
    pv b = 20;
    pv c = asm!("nop", in("rax") a, in("rcx") b, out("rdx") b);
    print("asm in+out OK\n");
    print(c);
    print("\n");

    // Test 6: Memory fence instructions
    asm!("mfence");
    asm!("sfence");
    asm!("lfence");
    print("asm fences OK\n");

    // Test 7: Pause instruction (spin-wait hint)
    asm!("pause");
    print("asm pause OK\n");

    ret 0;
}
