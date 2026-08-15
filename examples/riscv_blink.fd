// RISC-V Microcontroller Example — Bare-metal LED blink for SiFive HiFive1
// Phase 5B success metric: Run Pledge on a RISC-V microcontroller with 16KB RAM.
//
// This program blinks the onboard LED of a SiFive HiFive1 Rev B board.
// The HiFive1 uses a FE310 SoC with GPIO at base address 0x10012000.
//
// Build: pledgec codegen --target riscv examples/riscv_blink.p
//        Then assemble and flash with external tools.
//
// The program uses:
//   - asm!() for memory-mapped I/O (GPIO register access)
//   - RISC-V codegen (RV64IMAFDC)
//   - A busy-wait delay loop for timing
//   - No stdlib, no OS — pure bare-metal

/// Write a 32-bit value to a memory-mapped register
pf mmio_write(addr: u64, val: u32) {
    asm!("sw {0}, 0({1})", in("a0") val, in("a1") addr);
}

/// Read a 32-bit value from a memory-mapped register
pf mmio_read(addr: u64) -> u32 {
    pv result: u32 = 0;
    asm!("lw {0}, 0({1})", out("a0") result, in("a1") addr);
    ret result;
}

/// Configure GPIO pin 5 (HiFive1 onboard LED) as output
pf led_init() {
    // GPIO base = 0x10012000
    // GPIO_OUTPUT_EN offset = 0x08 (output enable)
    // Set bit 5 to enable output on pin 5
    pv en = mmio_read(0x10012008);
    en = en | (1 << 5);
    mmio_write(0x10012008, en);
}

/// Turn the LED on (set pin 5 high)
pf led_on() {
    // GPIO_OUTPUT_VAL offset = 0x0C (output value)
    // Set bit 5 to drive pin 5 high
    pv val = mmio_read(0x1001200C);
    val = val | (1 << 5);
    mmio_write(0x1001200C, val);
}

/// Turn the LED off (clear pin 5)
pf led_off() {
    // GPIO_OUTPUT_VAL offset = 0x0C (output value)
    // Clear bit 5 to drive pin 5 low
    pv val = mmio_read(0x1001200C);
    val = val & 0xFFFFFFDF;  // Clear bit 5 (~0x20 = 0xFFFFFFDF)
    mmio_write(0x1001200C, val);
}

/// Busy-wait delay (approximate, depends on CPU clock)
pf delay(cycles: u64) {
    pv i: u64 = 0;
    while i < cycles {
        asm!("nop");
        i = i + 1;
    }
}

/// Main entry point — blinks the LED forever
pf main() -> i64 {
    led_init();
    
    // Blink loop
    while true {
        led_on();
        delay(100000);
        led_off();
        delay(100000);
    }
    
    ret 0;
}
