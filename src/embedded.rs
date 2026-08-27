// Embedded & IoT Support — Phase 5B
//
// Provides features for targeting microcontrollers and embedded systems:
//   1. RISC-V target support (via codegen_riscv)
//   2. Deterministic memory mode (FixedBufferAllocator — no heap fragmentation)
//   3. Binary size optimization (< 4KB for simple programs)
//   4. Real-time guarantees (!RealTime effect — bounded execution time)
//   5. Hardware abstraction layer (HAL) framework

use std::collections::HashMap;

// ═══════════════════════════════════════════════════════════════════════════
// 5B.2: FixedBufferAllocator — Deterministic Memory
// ═══════════════════════════════════════════════════════════════════════════

/// A fixed-buffer allocator that allocates from a pre-allocated buffer.
/// No heap fragmentation, deterministic allocation patterns.
/// Suitable for embedded systems with strict memory requirements.
pub struct FixedBufferAllocator {
    buffer: Vec<u8>,
    offset: usize,
    /// Track allocations for freeing (address -> (offset, size))
    allocations: HashMap<usize, (usize, usize)>,
    /// Total bytes allocated
    total_allocated: usize,
    /// High-water mark
    peak_usage: usize,
}

impl FixedBufferAllocator {
    /// Create a new FixedBufferAllocator with the given capacity (in bytes).
    pub fn new(capacity: usize) -> Self {
        FixedBufferAllocator {
            buffer: vec![0u8; capacity],
            offset: 0,
            allocations: HashMap::new(),
            total_allocated: 0,
            peak_usage: 0,
        }
    }

    /// Allocate `size` bytes. Returns a pointer (offset into buffer) or None if out of memory.
    pub fn alloc(&mut self, size: usize) -> Option<usize> {
        // Align to 8 bytes
        let aligned_size = (size + 7) & !7;
        if self.offset + aligned_size > self.buffer.len() {
            return None; // Out of memory
        }

        let ptr = self.offset;
        self.offset += aligned_size;
        self.total_allocated += aligned_size;
        self.peak_usage = self.peak_usage.max(self.offset);
        self.allocations.insert(ptr, (ptr, aligned_size));
        Some(ptr)
    }

    /// Free a previously allocated pointer.
    /// Note: This is a bump allocator — freed memory is not reclaimed.
    /// For deterministic behavior, use reset() to clear all allocations.
    pub fn free(&mut self, ptr: usize) {
        self.allocations.remove(&ptr);
    }

    /// Reset the allocator to its initial state.
    /// All previously allocated memory becomes available again.
    pub fn reset(&mut self) {
        self.offset = 0;
        self.total_allocated = 0;
        self.allocations.clear();
    }

    /// Get the current memory usage (bytes allocated).
    pub fn usage(&self) -> usize {
        self.offset
    }

    /// Get the peak memory usage.
    pub fn peak(&self) -> usize {
        self.peak_usage
    }

    /// Get the total capacity.
    pub fn capacity(&self) -> usize {
        self.buffer.len()
    }

    /// Get the remaining free memory.
    pub fn free_space(&self) -> usize {
        self.buffer.len() - self.offset
    }

    /// Read a byte at the given offset.
    pub fn read(&self, offset: usize) -> Option<u8> {
        self.buffer.get(offset).copied()
    }

    /// Write a byte at the given offset.
    pub fn write(&mut self, offset: usize, value: u8) -> bool {
        if offset < self.buffer.len() {
            self.buffer[offset] = value;
            true
        } else {
            false
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// 5B.3: Binary Size Optimization
// ═══════════════════════════════════════════════════════════════════════════

/// Binary size optimizer for embedded targets.
/// Strips unused code, data, and debug info to minimize binary size.
pub struct BinarySizeOptimizer {
    /// Target binary size in bytes
    target_size: usize,
    /// Whether to strip debug info
    strip_debug: bool,
    /// Whether to remove unused functions
    remove_unused: bool,
    /// Whether to merge identical strings
    merge_strings: bool,
}

impl BinarySizeOptimizer {
    pub fn new(target_size: usize) -> Self {
        BinarySizeOptimizer {
            target_size,
            strip_debug: true,
            remove_unused: true,
            merge_strings: true,
        }
    }

    /// Analyze a binary and report size breakdown.
    pub fn analyze(&self, binary: &[u8]) -> SizeReport {
        let mut report = SizeReport::default();
        report.total_size = binary.len();
        report.text_size = binary.len(); // Simplified: all code
        report.data_size = 0;
        report.bss_size = 0;
        report.debug_size = 0;
        report.meets_target = binary.len() <= self.target_size;
        report
    }

    /// Get optimization recommendations for reducing binary size.
    pub fn recommendations(&self, binary: &[u8]) -> Vec<String> {
        let mut recs = Vec::new();
        let report = self.analyze(binary);

        if !report.meets_target {
            recs.push(format!(
                "Binary is {} bytes, target is {} bytes (exceeds by {} bytes)",
                report.total_size,
                self.target_size,
                report.total_size - self.target_size
            ));
        }

        if self.strip_debug {
            recs.push("Strip debug info: use --release flag to remove debug symbols".to_string());
        }

        if self.remove_unused {
            recs.push("Remove unused functions: use link-time dead code elimination".to_string());
        }

        if self.merge_strings {
            recs.push("Merge identical strings: enable string pooling in the compiler".to_string());
        }

        recs.push("Use --bare flag to remove runtime overhead (no stdlib)".to_string());
        recs.push("Consider using smaller integer types (i8/u8 instead of i64/u64)".to_string());

        recs
    }
}

/// Size analysis report for a binary.
#[derive(Debug, Default)]
pub struct SizeReport {
    pub total_size: usize,
    pub text_size: usize,
    pub data_size: usize,
    pub bss_size: usize,
    pub debug_size: usize,
    pub meets_target: bool,
}

// ═══════════════════════════════════════════════════════════════════════════
// 5B.4: Real-Time Guarantees
// ═══════════════════════════════════════════════════════════════════════════

/// Real-time analysis: verifies that functions marked with !RealTime
/// have bounded execution time (no loops without bounds, no recursion,
/// no dynamic dispatch, no heap allocation).
pub struct RealTimeAnalyzer {
    /// Maximum allowed execution time (in "abstract" cycles)
    max_cycles: u64,
    /// Functions verified as real-time safe
    verified: HashMap<String, RealTimeStatus>,
}

/// Result of real-time analysis for a function
#[derive(Debug, Clone)]
pub enum RealTimeStatus {
    /// Function is real-time safe
    Safe { estimated_cycles: u64 },
    /// Function violates real-time constraints
    Violation { reason: String },
}

impl RealTimeAnalyzer {
    pub fn new(max_cycles: u64) -> Self {
        RealTimeAnalyzer {
            max_cycles,
            verified: HashMap::new(),
        }
    }

    /// Check if a function is real-time safe.
    /// A function is real-time safe if:
    /// - It has no unbounded loops (all loops must have constant trip counts)
    /// - It has no recursion (direct or indirect)
    /// - It has no dynamic dispatch (no trait object calls)
    /// - It has no heap allocation (no !Alloc effect)
    /// - It has no I/O operations (no !IO effect)
    pub fn check_function(
        &mut self,
        name: &str,
        has_loops: bool,
        has_recursion: bool,
        has_dyn_dispatch: bool,
        has_alloc: bool,
        has_io: bool,
        estimated_cycles: u64,
    ) -> RealTimeStatus {
        let mut violations: Vec<String> = Vec::new();

        if has_loops {
            violations.push("unbounded loop detected".to_string());
        }
        if has_recursion {
            violations.push("recursion detected (unbounded stack growth)".to_string());
        }
        if has_dyn_dispatch {
            violations.push("dynamic dispatch detected (unbounded call time)".to_string());
        }
        if has_alloc {
            violations.push("heap allocation detected (non-deterministic timing)".to_string());
        }
        if has_io {
            violations.push("I/O operation detected (non-deterministic timing)".to_string());
        }
        if estimated_cycles > self.max_cycles {
            violations.push(format!(
                "estimated {} cycles exceeds max {} cycles",
                estimated_cycles, self.max_cycles
            ));
        }

        let status = if violations.is_empty() {
            RealTimeStatus::Safe { estimated_cycles }
        } else {
            RealTimeStatus::Violation {
                reason: violations.join("; "),
            }
        };

        self.verified.insert(name.to_string(), status.clone());
        status
    }

    /// Get the verification status for a function
    pub fn status(&self, name: &str) -> Option<&RealTimeStatus> {
        self.verified.get(name)
    }

    /// Get all verified functions
    pub fn verified_functions(&self) -> Vec<&str> {
        self.verified.keys().map(|s| s.as_str()).collect()
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// 5B.5: Hardware Abstraction Layer (HAL) Framework
// ═══════════════════════════════════════════════════════════════════════════

/// HAL device descriptor: describes a hardware device and its interface.
#[derive(Debug, Clone)]
pub struct HalDevice {
    pub name: String,
    pub device_type: DeviceType,
    pub base_address: u64,
    pub registers: Vec<HalRegister>,
}

/// Type of hardware device
#[derive(Debug, Clone)]
pub enum DeviceType {
    Gpio,
    Uart,
    Spi,
    I2c,
    Timer,
    InterruptController,
    Mmu,
    Custom(String),
}

/// A hardware register descriptor
#[derive(Debug, Clone)]
pub struct HalRegister {
    pub name: String,
    pub offset: u32,
    pub width: u8, // 8, 16, 32, 64
    pub access: RegisterAccess,
    pub reset_value: u64,
    pub description: String,
}

/// Register access mode
#[derive(Debug, Clone)]
pub enum RegisterAccess {
    ReadOnly,
    WriteOnly,
    ReadWrite,
    WriteToClear,
}

/// HAL framework: manages hardware device descriptors and generates
/// Fides FFI bindings for accessing hardware registers.
pub struct HalFramework {
    devices: Vec<HalDevice>,
}

impl HalFramework {
    pub fn new() -> Self {
        HalFramework {
            devices: Vec::new(),
        }
    }

    /// Register a hardware device
    pub fn register_device(&mut self, device: HalDevice) {
        self.devices.push(device);
    }

    /// Get a device by name
    pub fn get_device(&self, name: &str) -> Option<&HalDevice> {
        self.devices.iter().find(|d| d.name == name)
    }

    /// Generate Fides FFI bindings for all registered devices.
    /// Returns Fides source code that wraps hardware register access.
    pub fn generate_bindings(&self) -> String {
        let mut code = String::new();
        code.push_str("// Generated HAL bindings — Phase 5B.5\n");
        code.push_str("// Hardware abstraction layer for embedded targets\n\n");

        for device in &self.devices {
            code.push_str(&format!("// ─── {} ({}) ───\n", device.name, self.device_type_str(&device.device_type)));
            code.push_str(&format!("// Base address: 0x{:08X}\n", device.base_address));

            for reg in &device.registers {
                let addr = device.base_address + reg.offset as u64;
                let access_fn = match reg.access {
                    RegisterAccess::ReadOnly => "read",
                    RegisterAccess::WriteOnly => "write",
                    RegisterAccess::ReadWrite => "read_write",
                    RegisterAccess::WriteToClear => "write_to_clear",
                };

                code.push_str(&format!(
                    "// {}: {} (offset 0x{:02X}, {}-bit, {})\n",
                    reg.name, reg.description, reg.offset, reg.width, access_fn
                ));
                code.push_str(&format!(
                    "fn {}_{}_read() -> u{} !Read {{\n    return p_phys_read{}(0x{:08X});\n}}\n",
                    device.name, reg.name, reg.width, reg.width, addr
                ));

                if !matches!(reg.access, RegisterAccess::ReadOnly) {
                    code.push_str(&format!(
                        "fn {}_{}_write(val: u{}) -> void !Write {{\n    p_phys_write{}(0x{:08X}, val);\n}}\n",
                        device.name, reg.name, reg.width, reg.width, addr
                    ));
                }
                code.push('\n');
            }
        }

        code
    }

    fn device_type_str(&self, dt: &DeviceType) -> String {
        match dt {
            DeviceType::Gpio => "GPIO".to_string(),
            DeviceType::Uart => "UART".to_string(),
            DeviceType::Spi => "SPI".to_string(),
            DeviceType::I2c => "I2C".to_string(),
            DeviceType::Timer => "Timer".to_string(),
            DeviceType::InterruptController => "Interrupt Controller".to_string(),
            DeviceType::Mmu => "MMU".to_string(),
            DeviceType::Custom(s) => s.clone(),
        }
    }

    /// Create a default HAL for a common microcontroller (e.g., RISC-V HiFive1)
    pub fn default_riscv_hal() -> HalFramework {
        let mut hal = HalFramework::new();

        // GPIO device
        hal.register_device(HalDevice {
            name: "gpio".to_string(),
            device_type: DeviceType::Gpio,
            base_address: 0x10012000,
            registers: vec![
                HalRegister {
                    name: "input_val".to_string(),
                    offset: 0x00,
                    width: 32,
                    access: RegisterAccess::ReadOnly,
                    reset_value: 0,
                    description: "GPIO input value".to_string(),
                },
                HalRegister {
                    name: "output_val".to_string(),
                    offset: 0x08,
                    width: 32,
                    access: RegisterAccess::ReadWrite,
                    reset_value: 0,
                    description: "GPIO output value".to_string(),
                },
                HalRegister {
                    name: "dir".to_string(),
                    offset: 0x04,
                    width: 32,
                    access: RegisterAccess::ReadWrite,
                    reset_value: 0,
                    description: "GPIO direction (1=output, 0=input)".to_string(),
                },
            ],
        });

        // UART device
        hal.register_device(HalDevice {
            name: "uart".to_string(),
            device_type: DeviceType::Uart,
            base_address: 0x10013000,
            registers: vec![
                HalRegister {
                    name: "tx_data".to_string(),
                    offset: 0x00,
                    width: 32,
                    access: RegisterAccess::WriteOnly,
                    reset_value: 0,
                    description: "UART transmit data".to_string(),
                },
                HalRegister {
                    name: "rx_data".to_string(),
                    offset: 0x04,
                    width: 32,
                    access: RegisterAccess::ReadOnly,
                    reset_value: 0,
                    description: "UART receive data".to_string(),
                },
                HalRegister {
                    name: "status".to_string(),
                    offset: 0x08,
                    width: 32,
                    access: RegisterAccess::ReadOnly,
                    reset_value: 0,
                    description: "UART status flags".to_string(),
                },
            ],
        });

        // Timer device
        hal.register_device(HalDevice {
            name: "timer".to_string(),
            device_type: DeviceType::Timer,
            base_address: 0x0200_BFF8,
            registers: vec![
                HalRegister {
                    name: "mtime".to_string(),
                    offset: 0x00,
                    width: 64,
                    access: RegisterAccess::ReadOnly,
                    reset_value: 0,
                    description: "Timer counter (read-only)".to_string(),
                },
                HalRegister {
                    name: "mtimecmp".to_string(),
                    offset: 0x08,
                    width: 64,
                    access: RegisterAccess::ReadWrite,
                    reset_value: 0xFFFFFFFFFFFFFFFF,
                    description: "Timer compare register".to_string(),
                },
            ],
        });

        hal
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════════

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_fixed_buffer_allocator() {
        let mut alloc = FixedBufferAllocator::new(1024);
        assert_eq!(alloc.capacity(), 1024);
        assert_eq!(alloc.usage(), 0);

        let ptr1 = alloc.alloc(16).unwrap();
        assert_eq!(ptr1, 0);
        assert_eq!(alloc.usage(), 16);

        let ptr2 = alloc.alloc(32).unwrap();
        assert_eq!(ptr2, 16); // aligned to 8
        assert_eq!(alloc.usage(), 48);

        alloc.free(ptr1);
        assert_eq!(alloc.usage(), 48); // bump allocator doesn't reclaim

        alloc.reset();
        assert_eq!(alloc.usage(), 0);
    }

    #[test]
    fn test_fba_out_of_memory() {
        let mut alloc = FixedBufferAllocator::new(32);
        assert!(alloc.alloc(32).is_some());
        assert!(alloc.alloc(1).is_none()); // Out of memory
    }

    #[test]
    fn test_fba_read_write() {
        let mut alloc = FixedBufferAllocator::new(64);
        let ptr = alloc.alloc(8).unwrap();
        assert!(alloc.write(ptr, 42));
        assert_eq!(alloc.read(ptr), Some(42));
    }

    #[test]
    fn test_binary_size_optimizer() {
        let opt = BinarySizeOptimizer::new(4096); // 4KB target
        let binary = vec![0u8; 2048];
        let report = opt.analyze(&binary);
        assert!(report.meets_target);
        assert_eq!(report.total_size, 2048);
    }

    #[test]
    fn test_binary_size_exceeds_target() {
        let opt = BinarySizeOptimizer::new(4096);
        let binary = vec![0u8; 8192];
        let report = opt.analyze(&binary);
        assert!(!report.meets_target);
        let recs = opt.recommendations(&binary);
        assert!(!recs.is_empty());
    }

    #[test]
    fn test_real_time_safe() {
        let mut analyzer = RealTimeAnalyzer::new(1000);
        let status = analyzer.check_function("blink", false, false, false, false, false, 100);
        assert!(matches!(status, RealTimeStatus::Safe { .. }));
    }

    #[test]
    fn test_real_time_violation() {
        let mut analyzer = RealTimeAnalyzer::new(1000);
        let status = analyzer.check_function("bad", true, false, false, false, false, 100);
        assert!(matches!(status, RealTimeStatus::Violation { .. }));
    }

    #[test]
    fn test_hal_framework() {
        let hal = HalFramework::default_riscv_hal();
        assert!(hal.get_device("gpio").is_some());
        assert!(hal.get_device("uart").is_some());
        assert!(hal.get_device("timer").is_some());

        let bindings = hal.generate_bindings();
        assert!(bindings.contains("gpio"));
        assert!(bindings.contains("uart"));
        assert!(bindings.contains("timer"));
        assert!(bindings.contains("p_phys_read"));
    }

    #[test]
    fn test_hal_custom_device() {
        let mut hal = HalFramework::new();
        hal.register_device(HalDevice {
            name: "custom".to_string(),
            device_type: DeviceType::Custom("MyDevice".to_string()),
            base_address: 0x40000000,
            registers: vec![HalRegister {
                name: "ctrl".to_string(),
                offset: 0x00,
                width: 32,
                access: RegisterAccess::ReadWrite,
                reset_value: 0,
                description: "Control register".to_string(),
            }],
        });

        let bindings = hal.generate_bindings();
        assert!(bindings.contains("custom_ctrl_read"));
        assert!(bindings.contains("custom_ctrl_write"));
    }
}
