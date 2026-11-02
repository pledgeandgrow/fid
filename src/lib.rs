// lib.rs — Library target for fidesc, exposing modules to other binaries
// (scribe, veil, ark). The main.rs binary also uses these modules.

// The Fides compiler is a multi-backend compiler (interpreter, PE, ELF, C,
// WASM, ARM64, RISC-V). Not all code paths are exercised in every build
// configuration, but all backends must remain available. Suppress dead-code
// warnings at the crate level rather than deleting unused code.
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_imports)]
#![allow(unused_mut)]
#![allow(unused_assignments)]
#![allow(unreachable_patterns)]
#![allow(non_snake_case)]

pub mod ast;
pub mod borrowck;
pub mod escape;
pub mod secrecy;
pub mod codegen;
pub mod codegen_c;
pub mod compiler;
pub mod flat_ast;
pub mod comptime;
pub mod cparser;
pub mod diag;
pub mod interp;
pub mod ir;
pub mod json;
pub mod lower;
pub mod mono;
pub mod lexer;
pub mod parser;
pub mod token;
pub mod typechecker;
pub mod pe;
pub mod dwarf;
pub mod pir;
pub mod opt;
pub mod regalloc;
pub mod codegen_x64;
pub mod codegen_arm64;
pub mod arm64_enc;
pub mod codegen_riscv;
pub mod riscv_enc;
pub mod peephole;
pub mod elf64;
pub mod coff;
pub mod macho;
pub mod hotreload;
pub mod inc;
pub mod linker;
pub mod runtime_linux;
pub mod runtime_arm64;
pub mod runtime_riscv;
pub mod autograd;
pub mod embedded;
pub mod gpu;
pub mod tensor;
pub mod wasm;
pub mod x86;
pub mod lsp;
pub mod fmt;
pub mod debugger;
pub mod native_debugger;
pub mod ark;
pub mod scribe;
pub mod veil;
