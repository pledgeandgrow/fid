// Test allocator-as-parameter — Phase 2A.8
// Functions that declare !Alloc must take an Allocator parameter (like Zig, verified at compile time)

pf alloc_sum(alloc: &Allocator, a: i64, b: i64) -> i64 !Alloc {
    pv ptr = allocator_alloc(alloc, 8);
    ret a + b;
}

pf main() -> i64 {
    pv alloc = create_allocator("my_pool");
    
    pv result = alloc_sum(&alloc, 10, 20);
    print(result);  // 30
    
    pv bytes = allocator_bytes_used(&alloc);
    print(bytes);  // 0 (simulated)
    
    ret 0;
}
