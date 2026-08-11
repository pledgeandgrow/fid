// Raspberry Pi 3 LED Blinker — ARM64 bare-metal
// Phase 5A success metric: Write a bare-metal LED blinker for a Raspberry Pi (ARM64).
//
// This program blinks the ACT (Activity) LED on a Raspberry Pi 3.
// The ACT LED is controlled via GPIO pin 47, which is mapped to
// the BCM2837 SoC's GPIO peripheral at physical address 0x3F200000.
//
// Build: pledgec emit --bare examples/rpi_led_blink.p -o kernel8.img
//        (or) pledgec codegen --target arm64 examples/rpi_led_blink.p
// Deploy: Copy kernel8.img to the Pi's boot partition (alongside start.elf).
//
// The program uses:
//   - asm!() for memory-mapped I/O (GPIO register access)
//   - --bare mode (no stdlib, no OS, flat binary)
//   - A busy-wait delay loop for timing
//
// NOTE: The bare-metal emitter currently produces x86-64 machine code.
// For ARM64 deployment, use `pledgec codegen --target arm64` to get
// AArch64 assembly text, then assemble with an external assembler.

/// Write a 32-bit value to a memory-mapped register
pf mmio_write(addr: u64, val: u32) {
    asm!("str {0:w}, [{1}]", in("w0") val, in("x1") addr);
}

/// Read a 32-bit value from a memory-mapped register
pf mmio_read(addr: u64) -> u32 {
    pv result: u32 = 0;
    asm!("ldr {0:w}, [{1}]", out("w0") result, in("x1") addr);
    ret result;
}

/// Configure GPIO pin 47 as output
pf led_init() {
    // GPFSEL4: bits 21-23 control pin 47 (001 = output)
    // GPIO_BASE = 0x3F200000, GPFSEL4 offset = 0x10
    pv sel = mmio_read(0x3F200010);
    // Clear bits 21-23: AND with inverted mask (use XOR to compute ~mask)
    // 0x7 << 21 = 0xE00000, ~0xE00000 = 0xFF1FFFFF
    sel = sel & 0xFF1FFFFF;
    sel = sel | (0x1 << 21);    // Set bit 21 (output mode)
    mmio_write(0x3F200010, sel);
}

/// Turn the LED on (set pin 47 high)
pf led_on() {
    // GPSET1 offset = 0x20, pin 47 is bit (47-32) = bit 15
    mmio_write(0x3F200020, 1 << 15);
}

/// Turn the LED off (clear pin 47)
pf led_off() {
    // GPCLR1 offset = 0x2C, pin 47 is bit (47-32) = bit 15
    mmio_write(0x3F20002C, 1 << 15);
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
        delay(500000);
        led_off();
        delay(500000);
    }
    
    ret 0;
}
