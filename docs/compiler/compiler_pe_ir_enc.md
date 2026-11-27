[All modules](index.md)

# compiler::pe_ir_enc -- Fides Documentation

## Contents

- [PEIR_RAX:](#PEIR_RAX:) (const)
- [PEIR_RCX:](#PEIR_RCX:) (const)
- [PEIR_RDX:](#PEIR_RDX:) (const)
- [PEIR_RBX:](#PEIR_RBX:) (const)
- [PEIR_RSP:](#PEIR_RSP:) (const)
- [PEIR_RBP:](#PEIR_RBP:) (const)
- [PEIR_RSI:](#PEIR_RSI:) (const)
- [PEIR_RDI:](#PEIR_RDI:) (const)
- [PEIR_R8:](#PEIR_R8:) (const)
- [PEIR_R9:](#PEIR_R9:) (const)
- [PEIR_R10:](#PEIR_R10:) (const)
- [PEIR_R11:](#PEIR_R11:) (const)
- [PEIR_R12:](#PEIR_R12:) (const)
- [PEIR_R13:](#PEIR_R13:) (const)
- [PEIR_R14:](#PEIR_R14:) (const)
- [PEIR_R15:](#PEIR_R15:) (const)
- [XMM0:](#XMM0:) (const)
- [XMM1:](#XMM1:) (const)
- [XMM2:](#XMM2:) (const)
- [XMM3:](#XMM3:) (const)
- [XMM4:](#XMM4:) (const)
- [XMM5:](#XMM5:) (const)
- [XMM6:](#XMM6:) (const)
- [XMM7:](#XMM7:) (const)
- [XMM8:](#XMM8:) (const)
- [XMM9:](#XMM9:) (const)
- [XMM10:](#XMM10:) (const)
- [XMM11:](#XMM11:) (const)
- [XMM12:](#XMM12:) (const)
- [XMM13:](#XMM13:) (const)
- [XMM14:](#XMM14:) (const)
- [XMM15:](#XMM15:) (const)
- [peir_reg_code](#peir_reg_code) (function)
- [reg_ext](#reg_ext) (function)
- [xmm_high](#xmm_high) (function)
- [PEIR_RELOC_REL32:](#PEIR_RELOC_REL32:) (const)
- [PEIR_RELOC_RIPREL32:](#PEIR_RELOC_RIPREL32:) (const)
- [PEIR_RELOC_LEALABEL:](#PEIR_RELOC_LEALABEL:) (const)
- [PeirReloc](#PeirReloc) (struct)
- [PeirReloc_new](#PeirReloc_new) (function)
- [PeirRipRef](#PeirRipRef) (struct)
- [PeirRipRef_new](#PeirRipRef_new) (function)
- [PeirEnc](#PeirEnc) (struct)
- [PeirEnc_new](#PeirEnc_new) (function)
- [PeirEnc_alloc_label](#PeirEnc_alloc_label) (function)
- [PeirEnc_place_label](#PeirEnc_place_label) (function)
- [PeirEnc_set_label](#PeirEnc_set_label) (function)
- [PeirEnc_emit_bytes](#PeirEnc_emit_bytes) (function)
- [PeirEnc_add_reloc](#PeirEnc_add_reloc) (function)
- [PeirEnc_add_rip_ref](#PeirEnc_add_rip_ref) (function)
- [PeirEnc_align_to](#PeirEnc_align_to) (function)
- [PeirEnc_offset](#PeirEnc_offset) (function)
- [PeirEnc_emit](#PeirEnc_emit) (function)
- [PeirEnc_emit_u8](#PeirEnc_emit_u8) (function)
- [PeirEnc_emit_u16](#PeirEnc_emit_u16) (function)
- [PeirEnc_emit_u32](#PeirEnc_emit_u32) (function)
- [PeirEnc_emit_i32](#PeirEnc_emit_i32) (function)
- [PeirEnc_emit_u64](#PeirEnc_emit_u64) (function)
- [PeirEnc_rex](#PeirEnc_rex) (function)
- [PeirEnc_rex_w](#PeirEnc_rex_w) (function)
- [PeirEnc_modrm](#PeirEnc_modrm) (function)
- [PeirEnc_modrm_rr](#PeirEnc_modrm_rr) (function)
- [PeirEnc_push](#PeirEnc_push) (function)
- [PeirEnc_pop](#PeirEnc_pop) (function)
- [PeirEnc_ret](#PeirEnc_ret) (function)
- [PeirEnc_nop](#PeirEnc_nop) (function)
- [PeirEnc_ud2](#PeirEnc_ud2) (function)
- [PeirEnc_rex_sse](#PeirEnc_rex_sse) (function)
- [PeirEnc_rex_sse_rm](#PeirEnc_rex_sse_rm) (function)
- [PeirEnc_modrm_xmm](#PeirEnc_modrm_xmm) (function)
- [PeirEnc_addpd_rr](#PeirEnc_addpd_rr) (function)
- [PeirEnc_subpd_rr](#PeirEnc_subpd_rr) (function)
- [PeirEnc_mulpd_rr](#PeirEnc_mulpd_rr) (function)
- [PeirEnc_divpd_rr](#PeirEnc_divpd_rr) (function)
- [PeirEnc_movapd_rr](#PeirEnc_movapd_rr) (function)
- [PeirEnc_movsd_rr](#PeirEnc_movsd_rr) (function)
- [PeirEnc_addpd_mem_rbp](#PeirEnc_addpd_mem_rbp) (function)
- [PeirEnc_movapd_mem_rbp](#PeirEnc_movapd_mem_rbp) (function)
- [PeirEnc_movapd_rbp_reg](#PeirEnc_movapd_rbp_reg) (function)
- [PeirEnc_vex2](#PeirEnc_vex2) (function)
- [PeirEnc_vaddpd_rr](#PeirEnc_vaddpd_rr) (function)
- [PeirEnc_vmulpd_rr](#PeirEnc_vmulpd_rr) (function)
- [PeirEnc_mov_imm64](#PeirEnc_mov_imm64) (function)
- [PeirEnc_mov_imm](#PeirEnc_mov_imm) (function)
- [PeirEnc_mov_imm32](#PeirEnc_mov_imm32) (function)
- [PeirEnc_mov_rr](#PeirEnc_mov_rr) (function)
- [PeirEnc_mov_rr_cp](#PeirEnc_mov_rr_cp) (function)
- [PeirEnc_mov_imm_cp](#PeirEnc_mov_imm_cp) (function)
- [PEIR_REG_SLOT:](#PEIR_REG_SLOT:) (const)
- [PEIR_VALUE_REGS:](#PEIR_VALUE_REGS:) (const)
- [PEIR_ALL_REGS:](#PEIR_ALL_REGS:) (const)
- [PEIR_ARG_VALUE_REGS:](#PEIR_ARG_VALUE_REGS:) (const)
- [PEIR_XMM_SLOT:](#PEIR_XMM_SLOT:) (const)
- [peir_value_reg](#peir_value_reg) (function)
- [peir_is_xmm_slot](#peir_is_xmm_slot) (function)
- [PeirEnc_movq_r64_xmm](#PeirEnc_movq_r64_xmm) (function)
- [PeirEnc_movdqu_mem_rbp_xmm](#PeirEnc_movdqu_mem_rbp_xmm) (function)
- [PeirEnc_movdqu_xmm_mem_rbp](#PeirEnc_movdqu_xmm_mem_rbp) (function)
- [PeirEnc_cmp_r_imm](#PeirEnc_cmp_r_imm) (function)
- [PeirEnc_add_r_imm](#PeirEnc_add_r_imm) (function)
- [PeirEnc_imul_r_r_imm](#PeirEnc_imul_r_r_imm) (function)
- [PeirEnc_lea_base_disp](#PeirEnc_lea_base_disp) (function)
- [PeirEnc_lea_base_index](#PeirEnc_lea_base_index) (function)
- [PeirEnc_mem_op](#PeirEnc_mem_op) (function)
- [PeirEnc_movzx_r_byte_idx](#PeirEnc_movzx_r_byte_idx) (function)
- [PeirEnc_mov_byte_idx_r](#PeirEnc_mov_byte_idx_r) (function)
- [PeirEnc_cmp_r_mem](#PeirEnc_cmp_r_mem) (function)
- [PeirEnc_cmp_mem_imm8](#PeirEnc_cmp_mem_imm8) (function)
- [PeirEnc_or_r_mem](#PeirEnc_or_r_mem) (function)
- [PeirEnc_mov_r_idx8](#PeirEnc_mov_r_idx8) (function)
- [PeirEnc_mov_idx8_r](#PeirEnc_mov_idx8_r) (function)
- [PeirEnc_mov_reg_mem_rbp](#PeirEnc_mov_reg_mem_rbp) (function)
- [PeirEnc_mov_mem_rbp_reg](#PeirEnc_mov_mem_rbp_reg) (function)
- [PeirEnc_emit_mem_rbp](#PeirEnc_emit_mem_rbp) (function)
- [PeirEnc_emit_mem_reg](#PeirEnc_emit_mem_reg) (function)
- [PeirEnc_lea_rbp](#PeirEnc_lea_rbp) (function)
- [PeirEnc_lea_rip](#PeirEnc_lea_rip) (function)
- [PeirEnc_lea_label](#PeirEnc_lea_label) (function)
- [PeirEnc_call_rip](#PeirEnc_call_rip) (function)
- [PeirEnc_mov_reg_rip](#PeirEnc_mov_reg_rip) (function)
- [PeirEnc_mov_rip_reg](#PeirEnc_mov_rip_reg) (function)
- [PeirEnc_call_label](#PeirEnc_call_label) (function)
- [PeirEnc_jmp_label](#PeirEnc_jmp_label) (function)
- [PeirEnc_jcc_label](#PeirEnc_jcc_label) (function)
- [PeirEnc_add_rr](#PeirEnc_add_rr) (function)
- [PeirEnc_sub_rr](#PeirEnc_sub_rr) (function)
- [PeirEnc_imul_r_i32](#PeirEnc_imul_r_i32) (function)
- [PeirEnc_imul_rr](#PeirEnc_imul_rr) (function)
- [PeirEnc_idiv_r](#PeirEnc_idiv_r) (function)
- [PeirEnc_and_rr](#PeirEnc_and_rr) (function)
- [PeirEnc_or_rr](#PeirEnc_or_rr) (function)
- [PeirEnc_xor_rr](#PeirEnc_xor_rr) (function)
- [PeirEnc_not_r](#PeirEnc_not_r) (function)
- [PeirEnc_neg_r](#PeirEnc_neg_r) (function)
- [PeirEnc_cmp_rr](#PeirEnc_cmp_rr) (function)
- [PeirEnc_cmp_r8_i8](#PeirEnc_cmp_r8_i8) (function)
- [PeirEnc_test_rr](#PeirEnc_test_rr) (function)
- [PeirEnc_shl_cl](#PeirEnc_shl_cl) (function)
- [PeirEnc_shl_r_i8](#PeirEnc_shl_r_i8) (function)
- [PeirEnc_div_rr](#PeirEnc_div_rr) (function)
- [PeirEnc_mul_r](#PeirEnc_mul_r) (function)
- [PeirEnc_shr_cl](#PeirEnc_shr_cl) (function)
- [PeirEnc_mov_r32_r32](#PeirEnc_mov_r32_r32) (function)
- [PeirEnc_sar_cl](#PeirEnc_sar_cl) (function)
- [PeirEnc_shiftx](#PeirEnc_shiftx) (function)
- [PeirEnc_blsr](#PeirEnc_blsr) (function)
- [PeirEnc_andn](#PeirEnc_andn) (function)
- [PeirEnc_cqo](#PeirEnc_cqo) (function)
- [PeirEnc_movzx_r32_r8](#PeirEnc_movzx_r32_r8) (function)
- [PeirEnc_movzx_r32_r16](#PeirEnc_movzx_r32_r16) (function)
- [PeirEnc_movsxd_rr](#PeirEnc_movsxd_rr) (function)
- [PeirEnc_movsx_r64_r8](#PeirEnc_movsx_r64_r8) (function)
- [PeirEnc_movsx_r64_r16](#PeirEnc_movsx_r64_r16) (function)
- [PeirEnc_setcc_r](#PeirEnc_setcc_r) (function)
- [PeirEnc_sub_rsp_i8](#PeirEnc_sub_rsp_i8) (function)
- [PeirEnc_sub_rsp_i32](#PeirEnc_sub_rsp_i32) (function)
- [PeirEnc_add_rsp_i8](#PeirEnc_add_rsp_i8) (function)
- [PeirEnc_add_rsp_i32](#PeirEnc_add_rsp_i32) (function)
- [PeirEnc_add_r_i32](#PeirEnc_add_r_i32) (function)
- [PeirEnc_add_r_i8](#PeirEnc_add_r_i8) (function)
- [PeirEnc_mov_r8_imm](#PeirEnc_mov_r8_imm) (function)
- [PeirEnc_mov_reg8_mem](#PeirEnc_mov_reg8_mem) (function)
- [PeirEnc_mov_mem_reg8_ptr](#PeirEnc_mov_mem_reg8_ptr) (function)
- [PeirEnc_mov_mem_reg8](#PeirEnc_mov_mem_reg8) (function)
- [PeirEnc_mov_mem_disp_reg8](#PeirEnc_mov_mem_disp_reg8) (function)
- [PeirEnc_mov_mem_idx_reg8](#PeirEnc_mov_mem_idx_reg8) (function)
- [PeirEnc_mov_reg8_mem_idx](#PeirEnc_mov_reg8_mem_idx) (function)
- [PeirEnc_cmp_mem_idx_imm8](#PeirEnc_cmp_mem_idx_imm8) (function)
- [PeirEnc_inc_r](#PeirEnc_inc_r) (function)
- [PeirEnc_dec_r](#PeirEnc_dec_r) (function)
- [PeirEnc_xor_self](#PeirEnc_xor_self) (function)
- [PeirEnc_mov_reg_ptr](#PeirEnc_mov_reg_ptr) (function)
- [PeirEnc_mov_ptr_reg](#PeirEnc_mov_ptr_reg) (function)
- [PeirEnc_mov_ptr_reg32](#PeirEnc_mov_ptr_reg32) (function)
- [PeirEnc_mov_ptr_reg16](#PeirEnc_mov_ptr_reg16) (function)
- [PeirEnc_mov_reg8_ptr](#PeirEnc_mov_reg8_ptr) (function)
- [PeirEnc_mov_reg32_ptr](#PeirEnc_mov_reg32_ptr) (function)
- [PeirEnc_mov_reg16_ptr](#PeirEnc_mov_reg16_ptr) (function)
- [PeirEnc_mov_reg_mem_idx](#PeirEnc_mov_reg_mem_idx) (function)
- [PeirEnc_mov_mem_idx_reg](#PeirEnc_mov_mem_idx_reg) (function)
- [PeirEnc_mov_reg_mem_disp](#PeirEnc_mov_reg_mem_disp) (function)
- [PeirEnc_mov_mem_disp_reg](#PeirEnc_mov_mem_disp_reg) (function)
- [PeirEnc_mov_mem_disp_reg32](#PeirEnc_mov_mem_disp_reg32) (function)
- [PeirEnc_imul_r_r_imm32](#PeirEnc_imul_r_r_imm32) (function)
- [PeirEnc_call_reg](#PeirEnc_call_reg) (function)
- [PeirEnc_movsd_xmm_mem_rbp](#PeirEnc_movsd_xmm_mem_rbp) (function)
- [PeirEnc_movsd_mem_rbp_xmm](#PeirEnc_movsd_mem_rbp_xmm) (function)
- [PeirEnc_movss_xmm_mem_rbp](#PeirEnc_movss_xmm_mem_rbp) (function)
- [PeirEnc_movss_mem_rbp_xmm](#PeirEnc_movss_mem_rbp_xmm) (function)
- [PeirEnc_addsd](#PeirEnc_addsd) (function)
- [PeirEnc_subsd](#PeirEnc_subsd) (function)
- [PeirEnc_mulsd](#PeirEnc_mulsd) (function)
- [PeirEnc_divsd](#PeirEnc_divsd) (function)
- [PeirEnc_xorpd_xmm_mem_rbp](#PeirEnc_xorpd_xmm_mem_rbp) (function)
- [PeirEnc_xorpd_xmm_xmm](#PeirEnc_xorpd_xmm_xmm) (function)
- [PeirEnc_movq_xmm_r64](#PeirEnc_movq_xmm_r64) (function)
- [PeirEnc_ucomisd](#PeirEnc_ucomisd) (function)
- [PeirEnc_ucomiss](#PeirEnc_ucomiss) (function)
- [PeirEnc_cvtsi2sd_r64](#PeirEnc_cvtsi2sd_r64) (function)
- [PeirEnc_cvttsd2si_r64](#PeirEnc_cvttsd2si_r64) (function)
- [PeirEnc_addss](#PeirEnc_addss) (function)
- [PeirEnc_subss](#PeirEnc_subss) (function)
- [PeirEnc_mulss](#PeirEnc_mulss) (function)
- [PeirEnc_divss](#PeirEnc_divss) (function)
- [PeirEnc_movq_xmm_mem_rbp](#PeirEnc_movq_xmm_mem_rbp) (function)
- [PeirEnc_movq_mem_rbp_xmm](#PeirEnc_movq_mem_rbp_xmm) (function)
- [PeirEnc_resolve_labels](#PeirEnc_resolve_labels) (function)
- [PeirEnc_resolve_rip_refs](#PeirEnc_resolve_rip_refs) (function)
- [pe_kernel32_imports](#pe_kernel32_imports) (function)
- [pe_msvcrt_imports](#pe_msvcrt_imports) (function)
- [PeDllImport](#PeDllImport) (struct)
- [pe_clear_extra_imports](#pe_clear_extra_imports) (function)
- [pe_add_extra_import](#pe_add_extra_import) (function)
- [pe_link_dll](#pe_link_dll) (function)
- [PE_STATIC_DLLS:](#PE_STATIC_DLLS:) (const)
- [pe_link_dll_candidates](#pe_link_dll_candidates) (function)
- [pe_dll_found](#pe_dll_found) (function)
- [pe_dll_imports](#pe_dll_imports) (function)
- [pe_import_slot](#pe_import_slot) (function)
- [PIAT_GET_STD_HANDLE:](#PIAT_GET_STD_HANDLE:) (const)
- [PIAT_WRITE_FILE:](#PIAT_WRITE_FILE:) (const)
- [PIAT_EXIT_PROCESS:](#PIAT_EXIT_PROCESS:) (const)
- [PIAT_GET_PROCESS_HEAP:](#PIAT_GET_PROCESS_HEAP:) (const)
- [PIAT_HEAP_ALLOC:](#PIAT_HEAP_ALLOC:) (const)
- [PIAT_HEAP_FREE:](#PIAT_HEAP_FREE:) (const)
- [PIAT_CREATE_FILE_A:](#PIAT_CREATE_FILE_A:) (const)
- [PIAT_READ_FILE:](#PIAT_READ_FILE:) (const)
- [PIAT_CLOSE_HANDLE:](#PIAT_CLOSE_HANDLE:) (const)
- [PIAT_GET_FILE_SIZE:](#PIAT_GET_FILE_SIZE:) (const)
- [PIAT_GET_COMMAND_LINE_A:](#PIAT_GET_COMMAND_LINE_A:) (const)
- [PIAT_CREATE_PROCESS_A:](#PIAT_CREATE_PROCESS_A:) (const)
- [PIAT_WAIT_FOR_SINGLE_OBJECT:](#PIAT_WAIT_FOR_SINGLE_OBJECT:) (const)
- [PeirRdata](#PeirRdata) (struct)
- [PeirRdata_new](#PeirRdata_new) (function)
- [PeirRdata_add_string](#PeirRdata_add_string) (function)
- [PeirRdata_string_offset](#PeirRdata_string_offset) (function)
- [PeirRdata_build_strings](#PeirRdata_build_strings) (function)
- [PeirRdata_build](#PeirRdata_build) (function)
- [PeirRdata_patch_rvas](#PeirRdata_patch_rvas) (function)
- [pe_to_hex](#pe_to_hex) (function)
- [pe_bytes_hex](#pe_bytes_hex) (function)
- [pe_push_u64](#pe_push_u64) (function)
- [pe_push_u32](#pe_push_u32) (function)
- [pe_push_u16](#pe_push_u16) (function)
- [pe_put_u32](#pe_put_u32) (function)
- [pe_push_str](#pe_push_str) (function)
- [PE_IMAGE_BASE:](#PE_IMAGE_BASE:) (const)
- [PE_SECTION_ALIGN:](#PE_SECTION_ALIGN:) (const)
- [PE_FILE_ALIGN:](#PE_FILE_ALIGN:) (const)
- [PE_TEXT_RVA:](#PE_TEXT_RVA:) (const)
- [pe_align_up](#pe_align_up) (function)
- [PE_EFI_IMAGE_BASE:](#PE_EFI_IMAGE_BASE:) (const)
- [PE_SUBSYSTEM_EFI_APPLICATION:](#PE_SUBSYSTEM_EFI_APPLICATION:) (const)
- [pe_build_efi](#pe_build_efi) (function)
- [pe_efi_relocs](#pe_efi_relocs) (function)
- [pe_build_file](#pe_build_file) (function)
- [pe_build_file_img](#pe_build_file_img) (function)
- [PE_COFF_SYMTAB_PTR_AT:](#PE_COFF_SYMTAB_PTR_AT:) (const)
- [PE_COFF_NSYMS_AT:](#PE_COFF_NSYMS_AT:) (const)
- [pe_append_coff_symbols](#pe_append_coff_symbols) (function)
- [pe_append_coff_symbols_pre](#pe_append_coff_symbols_pre) (function)
- [pe_dwarf_section_names](#pe_dwarf_section_names) (function)
- [pe_get_u32](#pe_get_u32) (function)
- [pe_set_u32](#pe_set_u32) (function)
- [pe_dwarf_abbrev](#pe_dwarf_abbrev) (function)
- [pe_dwarf_info](#pe_dwarf_info) (function)
- [pe_dwarf_line](#pe_dwarf_line) (function)
- [pe_add_debug_sections](#pe_add_debug_sections) (function)
- [pe_add_debug_sections_n](#pe_add_debug_sections_n) (function)
- [pe_add_dwarf](#pe_add_dwarf) (function)

## PEIR_RAX:

```fides
pub const PEIR_RAX: i64 = 0;
```

*Defined at line 28*

---

## PEIR_RCX:

```fides
pub const PEIR_RCX: i64 = 1;
```

*Defined at line 29*

---

## PEIR_RDX:

```fides
pub const PEIR_RDX: i64 = 2;
```

*Defined at line 30*

---

## PEIR_RBX:

```fides
pub const PEIR_RBX: i64 = 3;
```

*Defined at line 31*

---

## PEIR_RSP:

```fides
pub const PEIR_RSP: i64 = 4;
```

*Defined at line 32*

---

## PEIR_RBP:

```fides
pub const PEIR_RBP: i64 = 5;
```

*Defined at line 33*

---

## PEIR_RSI:

```fides
pub const PEIR_RSI: i64 = 6;
```

*Defined at line 34*

---

## PEIR_RDI:

```fides
pub const PEIR_RDI: i64 = 7;
```

*Defined at line 35*

---

## PEIR_R8:

```fides
pub const PEIR_R8: i64 = 8;
```

*Defined at line 36*

---

## PEIR_R9:

```fides
pub const PEIR_R9: i64 = 9;
```

*Defined at line 37*

---

## PEIR_R10:

```fides
pub const PEIR_R10: i64 = 10;
```

*Defined at line 38*

---

## PEIR_R11:

```fides
pub const PEIR_R11: i64 = 11;
```

*Defined at line 39*

---

## PEIR_R12:

```fides
pub const PEIR_R12: i64 = 12;
```

*Defined at line 40*

---

## PEIR_R13:

```fides
pub const PEIR_R13: i64 = 13;
```

*Defined at line 41*

---

## PEIR_R14:

```fides
pub const PEIR_R14: i64 = 14;
```

*Defined at line 42*

---

## PEIR_R15:

```fides
pub const PEIR_R15: i64 = 15;
```

*Defined at line 43*

---

## XMM0:

```fides
pub const XMM0: i64 = 0;
```

*Defined at line 44*

---

## XMM1:

```fides
pub const XMM1: i64 = 1;
```

*Defined at line 45*

---

## XMM2:

```fides
pub const XMM2: i64 = 2;
```

*Defined at line 46*

---

## XMM3:

```fides
pub const XMM3: i64 = 3;
```

*Defined at line 47*

---

## XMM4:

```fides
pub const XMM4: i64 = 4;
```

*Defined at line 48*

---

## XMM5:

```fides
pub const XMM5: i64 = 5;
```

*Defined at line 49*

---

## XMM6:

```fides
pub const XMM6: i64 = 6;
```

*Defined at line 50*

---

## XMM7:

```fides
pub const XMM7: i64 = 7;
```

*Defined at line 51*

---

## XMM8:

```fides
pub const XMM8: i64 = 8;
```

*Defined at line 52*

---

## XMM9:

```fides
pub const XMM9: i64 = 9;
```

*Defined at line 53*

---

## XMM10:

```fides
pub const XMM10: i64 = 10;
```

*Defined at line 54*

---

## XMM11:

```fides
pub const XMM11: i64 = 11;
```

*Defined at line 55*

---

## XMM12:

```fides
pub const XMM12: i64 = 12;
```

*Defined at line 56*

---

## XMM13:

```fides
pub const XMM13: i64 = 13;
```

*Defined at line 57*

---

## XMM14:

```fides
pub const XMM14: i64 = 14;
```

*Defined at line 58*

---

## XMM15:

```fides
pub const XMM15: i64 = 15;
```

*Defined at line 59*

---

## peir_reg_code

Low 3 bits (ModR/M encoding) — Reg::code

```fides
pub fn peir_reg_code(r: i64) -> i64 {
```

**Parameters:**

- `r`: `i64`

**Returns:** `i64`

*Defined at line 61*

---

## reg_ext

True if this is an extended register (R8-R15, needs REX.B/R)

```fides
pub fn reg_ext(r: i64) -> bool {
```

**Parameters:**

- `r`: `i64`

**Returns:** `bool`

*Defined at line 66*

---

## xmm_high

Is this a high XMM register (8-15) requiring REX prefix? — XmmReg::is_high

```fides
pub fn xmm_high(x: i64) -> bool {
```

**Parameters:**

- `x`: `i64`

**Returns:** `bool`

*Defined at line 71*

---

## PEIR_RELOC_REL32:

Relative jump/call (rel32): disp32 = target - (code_offset + 4)

```fides
pub const PEIR_RELOC_REL32: i64 = 0;
```

*Defined at line 80*

---

## PEIR_RELOC_RIPREL32:

RIP-relative data access: disp32 = target_rva - (code_rva + 4)
(marker only — rip refs use PeirRipRef, not PeirReloc)

```fides
pub const PEIR_RELOC_RIPREL32: i64 = 1;
```

*Defined at line 83*

---

## PEIR_RELOC_LEALABEL:

RIP-relative lea to a code label: disp32 = target_code - (code_offset + 4)

```fides
pub const PEIR_RELOC_LEALABEL: i64 = 2;
```

*Defined at line 85*

---

## PeirReloc

Pending relocation — a location in code that needs to be patched
after all labels are resolved

```fides
pub struct PeirReloc {
```

**Fields:**

| Name | Type |
|------|------|
| `code_offset` | `i64` |
| `target_label` | `i64` |
| `kind` | `i64` |

*Defined at line 88*

---

## PeirReloc_new

```fides
pub fn PeirReloc_new(code_offset: i64, target_label: i64, kind: i64) -> PeirReloc {
```

**Parameters:**

- `code_offset`: `i64`
- `target_label`: `i64`
- `kind`: `i64`

**Returns:** `PeirReloc`

*Defined at line 97*

---

## PeirRipRef

Pending RIP-relative reference to .rdata section

```fides
pub struct PeirRipRef {
```

**Fields:**

| Name | Type |
|------|------|
| `code_offset` | `i64` |
| `rdata_offset` | `i64` |

*Defined at line 104*

---

## PeirRipRef_new

```fides
pub fn PeirRipRef_new(code_offset: i64, rdata_offset: i64) -> PeirRipRef {
```

**Parameters:**

- `code_offset`: `i64`
- `rdata_offset`: `i64`

**Returns:** `PeirRipRef`

*Defined at line 111*

---

## PeirEnc

```fides
pub struct PeirEnc {
```

**Fields:**

| Name | Type |
|------|------|
| `code` | `Vec<i64>` |
| `labels` | `Vec<i64>` |
| `pending_labels` | `Vec<PeirReloc>` |
| `pending_rip_refs` | `Vec<PeirRipRef>` |
| `next_label` | `i64` |
| `last_store_disp` | `i64` |
| `last_store_reg` | `i64` |
| `last_store_end` | `i64` |
| `last_mov_kind` | `i64` |
| `last_mov_dst` | `i64` |
| `last_mov_src` | `i64` |
| `last_mov_end` | `i64` |

*Defined at line 121*

---

## PeirEnc_new

```fides
pub fn PeirEnc_new() -> PeirEnc {
```

**Returns:** `PeirEnc`

*Defined at line 152*

---

## PeirEnc_alloc_label

Allocate a new label ID

```fides
pub fn PeirEnc_alloc_label(self: &mut PeirEnc) -> i64 {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `i64`

*Defined at line 159*

---

## PeirEnc_place_label

Mark a label at the current code position

```fides
pub fn PeirEnc_place_label(self: &mut PeirEnc, label: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `label`: `i64`

**Returns:** `void`

*Defined at line 167*

---

## PeirEnc_set_label

B5 (the per-function cache): a label at code offset `pos`

```fides
pub fn PeirEnc_set_label(self: &mut PeirEnc, label: i64, pos: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `label`: `i64`
- `pos`: `i64`

**Returns:** `void`

*Defined at line 174*

---

## PeirEnc_emit_bytes

B5: bytes copied in as they are (their references come after)

```fides
pub fn PeirEnc_emit_bytes(self: &mut PeirEnc, bytes: Vec<i64>) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `bytes`: `Vec<i64>`

**Returns:** `void`

*Defined at line 179*

---

## PeirEnc_add_reloc

B5: a label reference at code offset `pos` (4 bytes)

```fides
pub fn PeirEnc_add_reloc(self: &mut PeirEnc, pos: i64, label: i64, kind: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `pos`: `i64`
- `label`: `i64`
- `kind`: `i64`

**Returns:** `void`

*Defined at line 190*

---

## PeirEnc_add_rip_ref

B5: an .rdata reference at code offset `pos` (4 bytes)

```fides
pub fn PeirEnc_add_rip_ref(self: &mut PeirEnc, pos: i64, rdata_offset: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `pos`: `i64`
- `rdata_offset`: `i64`

**Returns:** `void`

*Defined at line 195*

---

## PeirEnc_align_to

Align code output to a boundary using multi-byte NOPs.

```fides
pub fn PeirEnc_align_to(self: &mut PeirEnc, boundary: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `boundary`: `i64`

**Returns:** `void`

*Defined at line 200*

---

## PeirEnc_offset

Get current code offset

```fides
pub fn PeirEnc_offset(self: &PeirEnc) -> i64 {
```

**Parameters:**

- `self`: `&PeirEnc`

**Returns:** `i64`

*Defined at line 262*

---

## PeirEnc_emit

Emit raw bytes (masked to 0..255)

```fides
pub fn PeirEnc_emit(self: &mut PeirEnc, bytes: Vec<i64>) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `bytes`: `Vec<i64>`

**Returns:** `void`

*Defined at line 267*

---

## PeirEnc_emit_u8

```fides
pub fn PeirEnc_emit_u8(self: &mut PeirEnc, b: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `b`: `i64`

**Returns:** `void`

*Defined at line 275*

---

## PeirEnc_emit_u16

```fides
pub fn PeirEnc_emit_u16(self: &mut PeirEnc, w: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `w`: `i64`

**Returns:** `void`

*Defined at line 279*

---

## PeirEnc_emit_u32

```fides
pub fn PeirEnc_emit_u32(self: &mut PeirEnc, d: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `d`: `i64`

**Returns:** `void`

*Defined at line 284*

---

## PeirEnc_emit_i32

emit_i32 — same little-endian byte layout as emit_u32

```fides
pub fn PeirEnc_emit_i32(self: &mut PeirEnc, d: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `d`: `i64`

**Returns:** `void`

*Defined at line 292*

---

## PeirEnc_emit_u64

```fides
pub fn PeirEnc_emit_u64(self: &mut PeirEnc, q: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `q`: `i64`

**Returns:** `void`

*Defined at line 296*

---

## PeirEnc_rex

Emit REX prefix
w: 64-bit operand size
r: extends ModR/M.reg (dst register)
x: extends SIB.index
b: extends ModR/M.r/m or SIB.base or opcode reg

```fides
pub fn PeirEnc_rex(self: &mut PeirEnc, w: bool, r: bool, x: bool, b: bool) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `w`: `bool`
- `r`: `bool`
- `x`: `bool`
- `b`: `bool`

**Returns:** `void`

*Defined at line 312*

---

## PeirEnc_rex_w

Emit REX.W prefix (64-bit operand, no extended regs)

```fides
pub fn PeirEnc_rex_w(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 330*

---

## PeirEnc_modrm

Emit ModR/M byte

```fides
pub fn PeirEnc_modrm(self: &mut PeirEnc, m: i64, reg: i64, rm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `m`: `i64`
- `reg`: `i64`
- `rm`: `i64`

**Returns:** `void`

*Defined at line 335*

---

## PeirEnc_modrm_rr

Encode a register-to-register ModR/M (mod=11)

```fides
pub fn PeirEnc_modrm_rr(self: &mut PeirEnc, reg: i64, rm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `reg`: `i64`
- `rm`: `i64`

**Returns:** `void`

*Defined at line 340*

---

## PeirEnc_push

push r64

```fides
pub fn PeirEnc_push(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 347*

---

## PeirEnc_pop

pop r64

```fides
pub fn PeirEnc_pop(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 355*

---

## PeirEnc_ret

ret

```fides
pub fn PeirEnc_ret(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 363*

---

## PeirEnc_nop

nop

```fides
pub fn PeirEnc_nop(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 368*

---

## PeirEnc_ud2

ud2

```fides
pub fn PeirEnc_ud2(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 373*

---

## PeirEnc_rex_sse

Emit REX prefix for XMM register pair (dst, src)

```fides
pub fn PeirEnc_rex_sse(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 383*

---

## PeirEnc_rex_sse_rm

BG89: the REX prefix of an SSE instruction whose ModRM.reg register is
`reg` and ModRM.rm register is `rm` (xmm or general registers, numbered
0-15): 0100WR0B, left out when W, R and B are all 0. It goes after the
mandatory 66/F2/F3 prefix, right before 0F.

```fides
pub fn PeirEnc_rex_sse_rm(self: &mut PeirEnc, w: bool, reg: i64, rm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `w`: `bool`
- `reg`: `i64`
- `rm`: `i64`

**Returns:** `void`

*Defined at line 406*

---

## PeirEnc_modrm_xmm

ModR/M byte for XMM reg-to-reg: 11 | dst_code | src_code

```fides
pub fn PeirEnc_modrm_xmm(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 413*

---

## PeirEnc_addpd_rr

addpd xmm_dst, xmm_src — SSE2 packed double add
Encoding: 66 0F 58 ModR/M

```fides
pub fn PeirEnc_addpd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 420*

---

## PeirEnc_subpd_rr

subpd xmm_dst, xmm_src — SSE2 packed double subtract
Encoding: 66 0F 5C ModR/M

```fides
pub fn PeirEnc_subpd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 430*

---

## PeirEnc_mulpd_rr

mulpd xmm_dst, xmm_src — SSE2 packed double multiply
Encoding: 66 0F 59 ModR/M

```fides
pub fn PeirEnc_mulpd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 440*

---

## PeirEnc_divpd_rr

divpd xmm_dst, xmm_src — SSE2 packed double divide
Encoding: 66 0F 5E ModR/M

```fides
pub fn PeirEnc_divpd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 450*

---

## PeirEnc_movapd_rr

movapd xmm_dst, xmm_src — SSE2 move aligned packed double
Encoding: 66 0F 28 ModR/M

```fides
pub fn PeirEnc_movapd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 460*

---

## PeirEnc_movsd_rr

movsd xmm_dst, xmm_src — SSE2 move scalar double
Encoding: F2 0F 10 ModR/M

```fides
pub fn PeirEnc_movsd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 470*

---

## PeirEnc_addpd_mem_rbp

addpd xmm_dst, [rbp+disp] — SSE2 add from memory

```fides
pub fn PeirEnc_addpd_mem_rbp(self: &mut PeirEnc, dst: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 479*

---

## PeirEnc_movapd_mem_rbp

movapd [rbp+disp], xmm_src — SSE2 store to memory

```fides
pub fn PeirEnc_movapd_mem_rbp(self: &mut PeirEnc, disp: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 498*

---

## PeirEnc_movapd_rbp_reg

movapd xmm_dst, [rbp+disp] — SSE2 load from memory

```fides
pub fn PeirEnc_movapd_rbp_reg(self: &mut PeirEnc, dst: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 519*

---

## PeirEnc_vex2

The VEX prefix of an instruction in map 0F (BG90: the old one set bit 6
always, dropped L and pp, and had no 3-byte form). `reg` is ModRM.reg,
`v` the vvvv register, `rm` ModRM.rm (0-15 each), `l` VEX.L (1 = 256
bits), `pp` the implied prefix (0 none, 1 66, 2 F3, 3 F2); W is 0.
When rm is 0-7 it is the 2-byte form C5 [~R ~vvvv L pp]; otherwise the
3-byte form C4 [~R ~X ~B 00001] [W=0 ~vvvv L pp] (B extends rm).

```fides
pub fn PeirEnc_vex2(self: &mut PeirEnc, reg: i64, v: i64, rm: i64, l: i64, pp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `reg`: `i64`
- `v`: `i64`
- `rm`: `i64`
- `l`: `i64`
- `pp`: `i64`

**Returns:** `void`

*Defined at line 546*

---

## PeirEnc_vaddpd_rr

vaddpd ymm_dst, ymm_src1, ymm_src2 — AVX packed double add (f64x4):
VEX.256.66.0F.WIG 58 /r, ModRM.reg = dst, vvvv = src1, ModRM.rm = src2.
`vaddpd ymm0, ymm1, ymm2` = C5 F5 58 C2.

```fides
pub fn PeirEnc_vaddpd_rr(self: &mut PeirEnc, dst: i64, src1: i64, src2: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src1`: `i64`
- `src2`: `i64`

**Returns:** `void`

*Defined at line 563*

---

## PeirEnc_vmulpd_rr

vmulpd ymm_dst, ymm_src1, ymm_src2 — AVX packed double multiply:
VEX.256.66.0F.WIG 59 /r. `vmulpd ymm0, ymm1, ymm2` = C5 F5 59 C2.

```fides
pub fn PeirEnc_vmulpd_rr(self: &mut PeirEnc, dst: i64, src1: i64, src2: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src1`: `i64`
- `src2`: `i64`

**Returns:** `void`

*Defined at line 573*

---

## PeirEnc_mov_imm64

mov r64, imm64 (10 bytes)

```fides
pub fn PeirEnc_mov_imm64(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 581*

---

## PeirEnc_mov_imm

mov r64, imm in its shortest form: mov r32 (zero-extends), imm32
sign-extended, or movabs. Flags are left alone.

```fides
pub fn PeirEnc_mov_imm(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 589*

---

## PeirEnc_mov_imm32

mov r64, imm32 (sign-extended, 7 bytes)

```fides
pub fn PeirEnc_mov_imm32(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 604*

---

## PeirEnc_mov_rr

mov r64, r64

```fides
pub fn PeirEnc_mov_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 612*

---

## PeirEnc_mov_rr_cp

D13 copy propagation: `mov dst, src`, left out when dst is src or when
the move emitted just before (through a _cp move, nothing in between)
was this one or `mov src, dst`. Only for the moves between values that
pe_ir.fd's generator emits (no hand-counted jump crosses them).

```fides
pub fn PeirEnc_mov_rr_cp(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 626*

---

## PeirEnc_mov_imm_cp

D13 copy propagation: `mov r, val` (PeirEnc_mov_imm), left out when the
move emitted just before (through a _cp move) put the same value in r.

```fides
pub fn PeirEnc_mov_imm_cp(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 644*

---

## PEIR_REG_SLOT:

```fides
pub const PEIR_REG_SLOT: i64 = 0x7F000000;
```

*Defined at line 659*

---

## PEIR_VALUE_REGS:

```fides
pub const PEIR_VALUE_REGS: i64 = 7;
```

*Defined at line 660*

---

## PEIR_ALL_REGS:

Value registers 7 and 8 are r10 and r11: caller-saved, so only a value
whose live range has no call (and nothing else that uses them) gets one
(competitive OP16). Value registers 9 and 10 are r8 and r9 (D4): also
argument registers, so only a value whose live range neither holds a
call nor starts or ends at one (nor is a parameter) gets one.

```fides
pub const PEIR_ALL_REGS: i64 = 11;
```

*Defined at line 666*

---

## PEIR_ARG_VALUE_REGS:

The first of the value registers that are argument registers (r8, r9)

```fides
pub const PEIR_ARG_VALUE_REGS: i64 = 9;
```

*Defined at line 668*

---

## PEIR_XMM_SLOT:

A frame offset at or above this names xmm register (offset -
PEIR_XMM_SLOT): an f64 value kept in a register (competitive OP17).

```fides
pub const PEIR_XMM_SLOT: i64 = 0x7F000100;
```

*Defined at line 671*

---

## peir_value_reg

The registers that hold values instead of stack slots: seven
callee-saved ones, then r10 and r11, then r8 and r9 (D4).

```fides
pub fn peir_value_reg(i: i64) -> i64 {
```

**Parameters:**

- `i`: `i64`

**Returns:** `i64`

*Defined at line 674*

---

## peir_is_xmm_slot

```fides
pub fn peir_is_xmm_slot(disp: i64) -> bool {
```

**Parameters:**

- `disp`: `i64`

**Returns:** `bool`

*Defined at line 699*

---

## PeirEnc_movq_r64_xmm

movq r64, xmm: 66 REX.W 0F 7E /r, ModRM.reg = xmm (REX.R for
xmm8-15, BG89), ModRM.rm = r64 (REX.B for r8-r15)

```fides
pub fn PeirEnc_movq_r64_xmm(self: &mut PeirEnc, r: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 705*

---

## PeirEnc_movdqu_mem_rbp_xmm

movdqu [rbp + disp], xmm (a callee-saved xmm register, all 128 bits):
F3 [REX.R for xmm8-15, BG89] 0F 7F /r

```fides
pub fn PeirEnc_movdqu_mem_rbp_xmm(self: &mut PeirEnc, disp: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 715*

---

## PeirEnc_movdqu_xmm_mem_rbp

movdqu xmm, [rbp + disp]: F3 [REX.R for xmm8-15, BG89] 0F 6F /r

```fides
pub fn PeirEnc_movdqu_xmm_mem_rbp(self: &mut PeirEnc, xmm: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 724*

---

## PeirEnc_cmp_r_imm

cmp r64, imm (the 8-bit form when it fits)

```fides
pub fn PeirEnc_cmp_r_imm(self: &mut PeirEnc, r: i64, imm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `imm`: `i64`

**Returns:** `void`

*Defined at line 733*

---

## PeirEnc_add_r_imm

add r64, imm (the 8-bit form when it fits)

```fides
pub fn PeirEnc_add_r_imm(self: &mut PeirEnc, r: i64, imm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `imm`: `i64`

**Returns:** `void`

*Defined at line 747*

---

## PeirEnc_imul_r_r_imm

imul dst, src, imm (the 8-bit form when it fits)

```fides
pub fn PeirEnc_imul_r_r_imm(self: &mut PeirEnc, dst: i64, src: i64, imm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`
- `imm`: `i64`

**Returns:** `void`

*Defined at line 756*

---

## PeirEnc_lea_base_disp

lea dst, [base + disp]

```fides
pub fn PeirEnc_lea_base_disp(self: &mut PeirEnc, dst: i64, base: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 768*

---

## PeirEnc_lea_base_index

lea dst, [base + index * (1 << scale)]

```fides
pub fn PeirEnc_lea_base_index(self: &mut PeirEnc, dst: i64, base: i64, index: i64, scale: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`
- `index`: `i64`
- `scale`: `i64`

**Returns:** `void`

*Defined at line 790*

---

## PeirEnc_mem_op

A 64-bit instruction `opcode` with a memory operand [base + disp], or
[base + index*8 + disp] when index >= 0; `reg` is the ModRM reg field
(a register, or an opcode extension when `reg_is_reg` is false).

```fides
pub fn PeirEnc_mem_op(self: &mut PeirEnc, opcode: i64, reg: i64, reg_is_reg: bool, base: i64, index: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `opcode`: `i64`
- `reg`: `i64`
- `reg_is_reg`: `bool`
- `base`: `i64`
- `index`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 808*

---

## PeirEnc_movzx_r_byte_idx

movzx r32, byte [base + index] (zero-extends into all of r64)

```fides
pub fn PeirEnc_movzx_r_byte_idx(self: &mut PeirEnc, r: i64, base: i64, index: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `base`: `i64`
- `index`: `i64`

**Returns:** `void`

*Defined at line 837*

---

## PeirEnc_mov_byte_idx_r

mov byte [base + index], r8 (the low byte of r)

```fides
pub fn PeirEnc_mov_byte_idx_r(self: &mut PeirEnc, base: i64, index: i64, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `index`: `i64`
- `r`: `i64`

**Returns:** `void`

*Defined at line 847*

---

## PeirEnc_cmp_r_mem

cmp r64, [base + disp]

```fides
pub fn PeirEnc_cmp_r_mem(self: &mut PeirEnc, r: i64, base: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `base`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 870*

---

## PeirEnc_cmp_mem_imm8

cmp qword [base + disp], imm8

```fides
pub fn PeirEnc_cmp_mem_imm8(self: &mut PeirEnc, base: i64, disp: i64, imm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `disp`: `i64`
- `imm`: `i64`

**Returns:** `void`

*Defined at line 875*

---

## PeirEnc_or_r_mem

or r64, [base + disp]

```fides
pub fn PeirEnc_or_r_mem(self: &mut PeirEnc, r: i64, base: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `base`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 881*

---

## PeirEnc_mov_r_idx8

mov r64, [base + index*8]

```fides
pub fn PeirEnc_mov_r_idx8(self: &mut PeirEnc, r: i64, base: i64, index: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `base`: `i64`
- `index`: `i64`

**Returns:** `void`

*Defined at line 886*

---

## PeirEnc_mov_idx8_r

mov [base + index*8], r64

```fides
pub fn PeirEnc_mov_idx8_r(self: &mut PeirEnc, base: i64, index: i64, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `index`: `i64`
- `r`: `i64`

**Returns:** `void`

*Defined at line 891*

---

## PeirEnc_mov_reg_mem_rbp

mov r64, [rbp + disp]

```fides
pub fn PeirEnc_mov_reg_mem_rbp(self: &mut PeirEnc, dst: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 896*

---

## PeirEnc_mov_mem_rbp_reg

mov [rbp + disp], r64

```fides
pub fn PeirEnc_mov_mem_rbp_reg(self: &mut PeirEnc, disp: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 926*

---

## PeirEnc_emit_mem_rbp

Emit RBP-relative memory operand (ModR/M + optional SIB + displacement)
For RBP base: mod=01 for disp8, mod=10 for disp32
RBP with mod=00 means RIP-relative, so we always need a displacement.

```fides
pub fn PeirEnc_emit_mem_rbp(self: &mut PeirEnc, regc: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `regc`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 951*

---

## PeirEnc_emit_mem_reg

Emit memory operand for [reg + disp] where reg is NOT RBP/RSP

```fides
pub fn PeirEnc_emit_mem_reg(self: &mut PeirEnc, regc: i64, base: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `regc`: `i64`
- `base`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 970*

---

## PeirEnc_lea_rbp

lea r64, [rbp + disp32] — compute address of RBP-relative slot

```fides
pub fn PeirEnc_lea_rbp(self: &mut PeirEnc, dst: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 1001*

---

## PeirEnc_lea_rip

lea r64, [rip + rdata_offset] — RIP-relative, patched later

```fides
pub fn PeirEnc_lea_rip(self: &mut PeirEnc, dst: i64, rdata_offset: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `rdata_offset`: `i64`

**Returns:** `void`

*Defined at line 1008*

---

## PeirEnc_lea_label

lea r64, [rip + disp32] — load address of a code label (for function ptrs)

```fides
pub fn PeirEnc_lea_label(self: &mut PeirEnc, dst: i64, label: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `label`: `i64`

**Returns:** `void`

*Defined at line 1020*

---

## PeirEnc_call_rip

call [rip + rdata_offset] — call through IAT, patched later

```fides
pub fn PeirEnc_call_rip(self: &mut PeirEnc, rdata_offset: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `rdata_offset`: `i64`

**Returns:** `void`

*Defined at line 1031*

---

## PeirEnc_mov_reg_rip

mov r64, [rip + rdata_offset] — load from .rdata, patched later

```fides
pub fn PeirEnc_mov_reg_rip(self: &mut PeirEnc, dst: i64, rdata_offset: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `rdata_offset`: `i64`

**Returns:** `void`

*Defined at line 1041*

---

## PeirEnc_mov_rip_reg

mov [rip + rdata_offset], r64 — store to .rdata, patched later

```fides
pub fn PeirEnc_mov_rip_reg(self: &mut PeirEnc, rdata_offset: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `rdata_offset`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1051*

---

## PeirEnc_call_label

call rel32 — direct call to a local label

```fides
pub fn PeirEnc_call_label(self: &mut PeirEnc, label: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `label`: `i64`

**Returns:** `void`

*Defined at line 1061*

---

## PeirEnc_jmp_label

jmp rel32 — unconditional jump to a local label

```fides
pub fn PeirEnc_jmp_label(self: &mut PeirEnc, label: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `label`: `i64`

**Returns:** `void`

*Defined at line 1069*

---

## PeirEnc_jcc_label

jcc rel32 — conditional jump to a local label
cc is the condition code (e.g., 0x0=jo, 0x4=je, 0x5=jne, etc.)

```fides
pub fn PeirEnc_jcc_label(self: &mut PeirEnc, cc: i64, label: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `cc`: `i64`
- `label`: `i64`

**Returns:** `void`

*Defined at line 1078*

---

## PeirEnc_add_rr

add r64, r64

```fides
pub fn PeirEnc_add_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1089*

---

## PeirEnc_sub_rr

sub r64, r64

```fides
pub fn PeirEnc_sub_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1096*

---

## PeirEnc_imul_r_i32

imul r, r, imm32: OF is set as by imul r, r/m

```fides
pub fn PeirEnc_imul_r_i32(self: &mut PeirEnc, r: i64, imm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `imm`: `i64`

**Returns:** `void`

*Defined at line 1103*

---

## PeirEnc_imul_rr

imul r64, r64

```fides
pub fn PeirEnc_imul_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1111*

---

## PeirEnc_idiv_r

idiv r/m64

```fides
pub fn PeirEnc_idiv_r(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1119*

---

## PeirEnc_and_rr

and r64, r64

```fides
pub fn PeirEnc_and_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1126*

---

## PeirEnc_or_rr

or r64, r64

```fides
pub fn PeirEnc_or_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1133*

---

## PeirEnc_xor_rr

xor r64, r64

```fides
pub fn PeirEnc_xor_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1140*

---

## PeirEnc_not_r

not r64

```fides
pub fn PeirEnc_not_r(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1147*

---

## PeirEnc_neg_r

neg r64

```fides
pub fn PeirEnc_neg_r(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1154*

---

## PeirEnc_cmp_rr

cmp r64, r64

```fides
pub fn PeirEnc_cmp_rr(self: &mut PeirEnc, left: i64, right: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `left`: `i64`
- `right`: `i64`

**Returns:** `void`

*Defined at line 1161*

---

## PeirEnc_cmp_r8_i8

cmp r8, imm8 (compare low byte of reg with immediate)

```fides
pub fn PeirEnc_cmp_r8_i8(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1168*

---

## PeirEnc_test_rr

test r64, r64

```fides
pub fn PeirEnc_test_rr(self: &mut PeirEnc, left: i64, right: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `left`: `i64`
- `right`: `i64`

**Returns:** `void`

*Defined at line 1179*

---

## PeirEnc_shl_cl

shl r64, cl

```fides
pub fn PeirEnc_shl_cl(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1186*

---

## PeirEnc_shl_r_i8

shl r64, imm8

```fides
pub fn PeirEnc_shl_r_i8(self: &mut PeirEnc, r: i64, count: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `count`: `i64`

**Returns:** `void`

*Defined at line 1193*

---

## PeirEnc_div_rr

div r/m64 (unsigned divide RDX:RAX by operand, quotient RAX, rem RDX)

```fides
pub fn PeirEnc_div_rr(self: &mut PeirEnc, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1201*

---

## PeirEnc_mul_r

mul r/m64 (unsigned RDX:RAX = RAX * operand; OF and CF set when RDX != 0)

```fides
pub fn PeirEnc_mul_r(self: &mut PeirEnc, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1208*

---

## PeirEnc_shr_cl

shr r64, cl

```fides
pub fn PeirEnc_shr_cl(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1215*

---

## PeirEnc_mov_r32_r32

mov r32, r32 (zero-extends into the 64-bit register)

```fides
pub fn PeirEnc_mov_r32_r32(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1222*

---

## PeirEnc_sar_cl

sar r64, cl

```fides
pub fn PeirEnc_sar_cl(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1229*

---

## PeirEnc_shiftx

OP20 (BMI2): d = a shifted by b (count masked to 6 bits, as by CL;
flags untouched): VEX.LZ.pp.0F38.W1 F7 /r, ModRM.reg = d, ModRM.rm =
a, vvvv = b. pp 1 SHLX, 2 SARX, 3 SHRX. `shlx rax, rbx, rcx` =
C4 E2 F1 F7 C3.

```fides
pub fn PeirEnc_shiftx(self: &mut PeirEnc, pp: i64, d: i64, a: i64, b: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `pp`: `i64`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

**Returns:** `void`

*Defined at line 1256*

---

## PeirEnc_blsr

OP20 (BMI1): d = a & (a - 1): VEX.LZ.0F38.W1 F3 /1, vvvv = d,
ModRM.rm = a. `blsr rax, rcx` = C4 E2 F8 F3 C9.

```fides
pub fn PeirEnc_blsr(self: &mut PeirEnc, d: i64, a: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `d`: `i64`
- `a`: `i64`

**Returns:** `void`

*Defined at line 1266*

---

## PeirEnc_andn

OP20 (BMI1): d = ~s & o: VEX.LZ.0F38.W1 F2 /r, ModRM.reg = d, vvvv =
s, ModRM.rm = o. `andn rax, rbx, rcx` = C4 E2 E0 F2 C1.

```fides
pub fn PeirEnc_andn(self: &mut PeirEnc, d: i64, s: i64, o: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `d`: `i64`
- `s`: `i64`
- `o`: `i64`

**Returns:** `void`

*Defined at line 1276*

---

## PeirEnc_cqo

cqo (sign-extend RAX into RDX:RAX)

```fides
pub fn PeirEnc_cqo(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 1285*

---

## PeirEnc_movzx_r32_r8

movzx r32, r/m8 (zero-extend byte to dword)

```fides
pub fn PeirEnc_movzx_r32_r8(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1291*

---

## PeirEnc_movzx_r32_r16

movzx r32, r16 (zero-extend word to dword)

```fides
pub fn PeirEnc_movzx_r32_r16(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1304*

---

## PeirEnc_movsxd_rr

movsxd r64, r32 (sign-extend dword to qword)

```fides
pub fn PeirEnc_movsxd_rr(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1314*

---

## PeirEnc_movsx_r64_r8

movsx r64, r8 (sign-extend byte to qword)

```fides
pub fn PeirEnc_movsx_r64_r8(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1321*

---

## PeirEnc_movsx_r64_r16

movsx r64, r16 (sign-extend word to qword)

```fides
pub fn PeirEnc_movsx_r64_r16(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1329*

---

## PeirEnc_setcc_r

setcc r/m8 — set byte on condition

```fides
pub fn PeirEnc_setcc_r(self: &mut PeirEnc, cc: i64, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `cc`: `i64`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1337*

---

## PeirEnc_sub_rsp_i8

sub rsp, imm8

```fides
pub fn PeirEnc_sub_rsp_i8(self: &mut PeirEnc, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1349*

---

## PeirEnc_sub_rsp_i32

sub rsp, imm32

```fides
pub fn PeirEnc_sub_rsp_i32(self: &mut PeirEnc, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1358*

---

## PeirEnc_add_rsp_i8

add rsp, imm8

```fides
pub fn PeirEnc_add_rsp_i8(self: &mut PeirEnc, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1367*

---

## PeirEnc_add_rsp_i32

add rsp, imm32

```fides
pub fn PeirEnc_add_rsp_i32(self: &mut PeirEnc, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1376*

---

## PeirEnc_add_r_i32

add r64, imm32

```fides
pub fn PeirEnc_add_r_i32(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1385*

---

## PeirEnc_add_r_i8

add r64, imm8

```fides
pub fn PeirEnc_add_r_i8(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1393*

---

## PeirEnc_mov_r8_imm

mov r8, imm8 (for byte stores)

```fides
pub fn PeirEnc_mov_r8_imm(self: &mut PeirEnc, r: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1401*

---

## PeirEnc_mov_reg8_mem

mov r8b, [reg] (byte load through pointer, handles RBP/R13 and RSP/R12)

```fides
pub fn PeirEnc_mov_reg8_mem(self: &mut PeirEnc, dst: i64, base: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`

**Returns:** `void`

*Defined at line 1412*

---

## PeirEnc_mov_mem_reg8_ptr

mov [reg], r8b (byte store through pointer, handles RBP/R13 and RSP/R12)

```fides
pub fn PeirEnc_mov_mem_reg8_ptr(self: &mut PeirEnc, base: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1433*

---

## PeirEnc_mov_mem_reg8

mov [reg], r8 (byte store through pointer)

```fides
pub fn PeirEnc_mov_mem_reg8(self: &mut PeirEnc, base: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1454*

---

## PeirEnc_mov_mem_disp_reg8

mov [reg + disp], r8 (byte store with displacement)

```fides
pub fn PeirEnc_mov_mem_disp_reg8(self: &mut PeirEnc, base: i64, disp: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `disp`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1476*

---

## PeirEnc_mov_mem_idx_reg8

mov [reg + reg], r8 (byte store with index)

```fides
pub fn PeirEnc_mov_mem_idx_reg8(self: &mut PeirEnc, base: i64, index: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `index`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1503*

---

## PeirEnc_mov_reg8_mem_idx

mov r8, [reg + reg] (byte load with index)

```fides
pub fn PeirEnc_mov_reg8_mem_idx(self: &mut PeirEnc, dst: i64, base: i64, index: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`
- `index`: `i64`

**Returns:** `void`

*Defined at line 1521*

---

## PeirEnc_cmp_mem_idx_imm8

cmp byte [reg + reg], imm8

```fides
pub fn PeirEnc_cmp_mem_idx_imm8(self: &mut PeirEnc, base: i64, index: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `index`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1536*

---

## PeirEnc_inc_r

inc r64

```fides
pub fn PeirEnc_inc_r(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1554*

---

## PeirEnc_dec_r

dec r64

```fides
pub fn PeirEnc_dec_r(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1561*

---

## PeirEnc_xor_self

xor r64, r64 (zero a register)

```fides
pub fn PeirEnc_xor_self(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1568*

---

## PeirEnc_mov_reg_ptr

mov rax, [reg] (load through pointer, 64-bit)

```fides
pub fn PeirEnc_mov_reg_ptr(self: &mut PeirEnc, dst: i64, base: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`

**Returns:** `void`

*Defined at line 1573*

---

## PeirEnc_mov_ptr_reg

mov [reg], r64 (store through pointer, 64-bit)

```fides
pub fn PeirEnc_mov_ptr_reg(self: &mut PeirEnc, base: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1590*

---

## PeirEnc_mov_ptr_reg32

mov [reg], r32 (32-bit store through pointer, no REX.W)

```fides
pub fn PeirEnc_mov_ptr_reg32(self: &mut PeirEnc, base: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1605*

---

## PeirEnc_mov_ptr_reg16

mov [reg], r16 (16-bit store through pointer, 0x66 prefix)

```fides
pub fn PeirEnc_mov_ptr_reg16(self: &mut PeirEnc, base: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1623*

---

## PeirEnc_mov_reg8_ptr

mov r8, [reg] (byte load through pointer)

```fides
pub fn PeirEnc_mov_reg8_ptr(self: &mut PeirEnc, dst: i64, base: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`

**Returns:** `void`

*Defined at line 1643*

---

## PeirEnc_mov_reg32_ptr

mov r32, [reg] (32-bit load through pointer, no REX.W)

```fides
pub fn PeirEnc_mov_reg32_ptr(self: &mut PeirEnc, dst: i64, base: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`

**Returns:** `void`

*Defined at line 1661*

---

## PeirEnc_mov_reg16_ptr

mov r16, [reg] (16-bit load through pointer, 0x66 prefix)

```fides
pub fn PeirEnc_mov_reg16_ptr(self: &mut PeirEnc, dst: i64, base: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`

**Returns:** `void`

*Defined at line 1679*

---

## PeirEnc_mov_reg_mem_idx

mov rax, [reg + reg*1] (load with index, 64-bit)

```fides
pub fn PeirEnc_mov_reg_mem_idx(self: &mut PeirEnc, dst: i64, base: i64, index: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`
- `index`: `i64`

**Returns:** `void`

*Defined at line 1699*

---

## PeirEnc_mov_mem_idx_reg

mov [reg + reg*1], r64 (store with index, 64-bit)

```fides
pub fn PeirEnc_mov_mem_idx_reg(self: &mut PeirEnc, base: i64, index: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `index`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1715*

---

## PeirEnc_mov_reg_mem_disp

mov rax, [reg + disp32] (load through pointer with displacement)

```fides
pub fn PeirEnc_mov_reg_mem_disp(self: &mut PeirEnc, dst: i64, base: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `base`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 1731*

---

## PeirEnc_mov_mem_disp_reg

mov [reg + disp32], r64 (store through pointer with displacement)

```fides
pub fn PeirEnc_mov_mem_disp_reg(self: &mut PeirEnc, base: i64, disp: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `disp`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1754*

---

## PeirEnc_mov_mem_disp_reg32

mov [reg + disp], r32 (32-bit store, no REX.W)

```fides
pub fn PeirEnc_mov_mem_disp_reg32(self: &mut PeirEnc, base: i64, disp: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `base`: `i64`
- `disp`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1777*

---

## PeirEnc_imul_r_r_imm32

imul r64, r64, imm32 (three-operand imul)

```fides
pub fn PeirEnc_imul_r_r_imm32(self: &mut PeirEnc, dst: i64, src: i64, val: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 1803*

---

## PeirEnc_call_reg

call r64 (indirect call through register)
NOTE: Rust emits the REX byte unconditionally (0x40 when not extended).

```fides
pub fn PeirEnc_call_reg(self: &mut PeirEnc, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1812*

---

## PeirEnc_movsd_xmm_mem_rbp

movsd xmm, [rbp + disp]

```fides
pub fn PeirEnc_movsd_xmm_mem_rbp(self: &mut PeirEnc, xmm: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 1821*

---

## PeirEnc_movsd_mem_rbp_xmm

movsd [rbp + disp], xmm

```fides
pub fn PeirEnc_movsd_mem_rbp_xmm(self: &mut PeirEnc, disp: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 1843*

---

## PeirEnc_movss_xmm_mem_rbp

movss xmm, [rbp + disp]

```fides
pub fn PeirEnc_movss_xmm_mem_rbp(self: &mut PeirEnc, xmm: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 1864*

---

## PeirEnc_movss_mem_rbp_xmm

movss [rbp + disp], xmm

```fides
pub fn PeirEnc_movss_mem_rbp_xmm(self: &mut PeirEnc, disp: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 1874*

---

## PeirEnc_addsd

addsd xmm0, xmm1

```fides
pub fn PeirEnc_addsd(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1884*

---

## PeirEnc_subsd

subsd xmm0, xmm1

```fides
pub fn PeirEnc_subsd(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1893*

---

## PeirEnc_mulsd

mulsd xmm0, xmm1

```fides
pub fn PeirEnc_mulsd(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1902*

---

## PeirEnc_divsd

divsd xmm0, xmm1

```fides
pub fn PeirEnc_divsd(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1911*

---

## PeirEnc_xorpd_xmm_mem_rbp

xorpd xmm, [rbp + disp]

```fides
pub fn PeirEnc_xorpd_xmm_mem_rbp(self: &mut PeirEnc, xmm: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 1920*

---

## PeirEnc_xorpd_xmm_xmm

xorpd xmm, xmm (register-to-register)

```fides
pub fn PeirEnc_xorpd_xmm_xmm(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1930*

---

## PeirEnc_movq_xmm_r64

movq xmm, r64 (move 64-bit GPR to XMM): 66 REX.W 0F 6E /r, ModRM.reg
= xmm (REX.R for xmm8-15, BG89), ModRM.rm = r64 (REX.B)

```fides
pub fn PeirEnc_movq_xmm_r64(self: &mut PeirEnc, xmm: i64, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1940*

---

## PeirEnc_ucomisd

ucomisd xmm0, xmm1

```fides
pub fn PeirEnc_ucomisd(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1949*

---

## PeirEnc_ucomiss

ucomiss xmm0, xmm1

```fides
pub fn PeirEnc_ucomiss(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1958*

---

## PeirEnc_cvtsi2sd_r64

cvtsi2sd xmm, r64: F2 REX.W 0F 2A /r, ModRM.reg = xmm (REX.R, BG89),
ModRM.rm = r64 (REX.B)

```fides
pub fn PeirEnc_cvtsi2sd_r64(self: &mut PeirEnc, xmm: i64, r: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `r`: `i64`

**Returns:** `void`

*Defined at line 1969*

---

## PeirEnc_cvttsd2si_r64

cvttsd2si r64, xmm: F2 REX.W 0F 2C /r, ModRM.reg = r64 (REX.R),
ModRM.rm = xmm (REX.B). BG88: r8-r15 set REX.B, which named xmm8-15
and left the result in the low register. `cvttsd2si rax, xmm0` =
F2 48 0F 2C C0; `cvttsd2si r9, xmm0` = F2 4C 0F 2C C8.

```fides
pub fn PeirEnc_cvttsd2si_r64(self: &mut PeirEnc, r: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `r`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 1981*

---

## PeirEnc_addss

addss xmm0, xmm1

```fides
pub fn PeirEnc_addss(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1990*

---

## PeirEnc_subss

subss xmm0, xmm1

```fides
pub fn PeirEnc_subss(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 1999*

---

## PeirEnc_mulss

mulss xmm0, xmm1

```fides
pub fn PeirEnc_mulss(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 2008*

---

## PeirEnc_divss

divss xmm0, xmm1

```fides
pub fn PeirEnc_divss(self: &mut PeirEnc, dst: i64, src: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `dst`: `i64`
- `src`: `i64`

**Returns:** `void`

*Defined at line 2017*

---

## PeirEnc_movq_xmm_mem_rbp

movq xmm, [rbp + disp] (64-bit SSE move)

```fides
pub fn PeirEnc_movq_xmm_mem_rbp(self: &mut PeirEnc, xmm: i64, disp: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `xmm`: `i64`
- `disp`: `i64`

**Returns:** `void`

*Defined at line 2026*

---

## PeirEnc_movq_mem_rbp_xmm

movq [rbp + disp], xmm

```fides
pub fn PeirEnc_movq_mem_rbp_xmm(self: &mut PeirEnc, disp: i64, xmm: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `disp`: `i64`
- `xmm`: `i64`

**Returns:** `void`

*Defined at line 2031*

---

## PeirEnc_resolve_labels

Resolve all pending label relocations (local jumps/calls)

```fides
pub fn PeirEnc_resolve_labels(self: &mut PeirEnc) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`

**Returns:** `void`

*Defined at line 2038*

---

## PeirEnc_resolve_rip_refs

Resolve all RIP-relative references to .rdata
text_rva: virtual address of .text section start
rdata_rva: virtual address of .rdata section start

```fides
pub fn PeirEnc_resolve_rip_refs(self: &mut PeirEnc, text_rva: i64, rdata_rva: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirEnc`
- `text_rva`: `i64`
- `rdata_rva`: `i64`

**Returns:** `void`

*Defined at line 2063*

---

## pe_kernel32_imports

Names of kernel32.dll functions we import (the first 13 have PIAT_*
constants; the rest are reached by name through `pe_import_slot`).

```fides
pub fn pe_kernel32_imports() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 2085*

---

## pe_msvcrt_imports

msvcrt.dll functions: the C library behind `extern "C"` declarations
(math.h, string.h, stdio.h, stdlib.h, ctype.h) and the Fides PE runtime
(src/pe_runtime/). Every name here must be an msvcrt.dll export: the
loader refuses to start a program importing one that is not.

```fides
pub fn pe_msvcrt_imports() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 2164*

---

## PeDllImport

One DLL and its imported names, in IAT order (DLL_IMPORTS in Rust).

```fides
pub struct PeDllImport {
```

**Fields:**

| Name | Type |
|------|------|
| `dll` | `str` |
| `names` | `Vec<str>` |

*Defined at line 2255*

---

## pe_clear_extra_imports

```fides
pub fn pe_clear_extra_imports() {
```

*Defined at line 2262*

---

## pe_add_extra_import

Import `name` from `dll` as well.

```fides
pub fn pe_add_extra_import(dll: str, name: str) {
```

**Parameters:**

- `dll`: `str`
- `name`: `str`

*Defined at line 2267*

---

## pe_link_dll

The DLL of a `link` library, "" for the Unix libraries the C runtime covers.

```fides
pub fn pe_link_dll(lib: str) -> str {
```

**Parameters:**

- `lib`: `str`

**Returns:** `str`

*Defined at line 2292*

---

## PE_STATIC_DLLS:

The DLLs the loader binds before the program starts (kernel32, msvcrt).
The others (`link "x"`) have IAT slots too, but the program loads them
itself at start-up (PeirGen_gen_dll_loader), so that a missing library
is reported by name instead of the loader failing silently (release E4).

```fides
pub const PE_STATIC_DLLS: i64 = 2;
```

*Defined at line 2312*

---

## pe_link_dll_candidates

The files to try, in order, for a `link` DLL: SQLite's own sqlite3.dll,
else the copy every Windows 10 and 11 has (winsqlite3.dll, same API).

```fides
pub fn pe_link_dll_candidates(dll: str) -> Vec<str> {
```

**Parameters:**

- `dll`: `str`

**Returns:** `Vec<str>`

*Defined at line 2315*

---

## pe_dll_found

Is `dll` where LoadLibraryA looks for it (beside `out_dir`, the current
folder, the system folder or a PATH entry)? For a compile-time warning.

```fides
pub fn pe_dll_found(dll: str, out_dir: str) -> bool {
```

**Parameters:**

- `dll`: `str`
- `out_dir`: `str`

**Returns:** `bool`

*Defined at line 2326*

---

## pe_dll_imports

The DLLs of the import table, in IAT order.

```fides
pub fn pe_dll_imports() -> Vec<PeDllImport> {
```

**Returns:** `Vec<PeDllImport>`

*Defined at line 2368*

---

## pe_import_slot

The IAT slot of an imported function (an index into `iat_rdata_offsets`).
Returns -1 when `name` is not imported (Rust: Option<usize> -> None).

```fides
pub fn pe_import_slot(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 2386*

---

## PIAT_GET_STD_HANDLE:

IAT indices for each imported function (first 13 kernel32 imports)

```fides
pub const PIAT_GET_STD_HANDLE: i64 = 0;
```

*Defined at line 2406*

---

## PIAT_WRITE_FILE:

```fides
pub const PIAT_WRITE_FILE: i64 = 1;
```

*Defined at line 2407*

---

## PIAT_EXIT_PROCESS:

```fides
pub const PIAT_EXIT_PROCESS: i64 = 2;
```

*Defined at line 2408*

---

## PIAT_GET_PROCESS_HEAP:

```fides
pub const PIAT_GET_PROCESS_HEAP: i64 = 3;
```

*Defined at line 2409*

---

## PIAT_HEAP_ALLOC:

```fides
pub const PIAT_HEAP_ALLOC: i64 = 4;
```

*Defined at line 2410*

---

## PIAT_HEAP_FREE:

```fides
pub const PIAT_HEAP_FREE: i64 = 5;
```

*Defined at line 2411*

---

## PIAT_CREATE_FILE_A:

```fides
pub const PIAT_CREATE_FILE_A: i64 = 6;
```

*Defined at line 2412*

---

## PIAT_READ_FILE:

```fides
pub const PIAT_READ_FILE: i64 = 7;
```

*Defined at line 2413*

---

## PIAT_CLOSE_HANDLE:

```fides
pub const PIAT_CLOSE_HANDLE: i64 = 8;
```

*Defined at line 2414*

---

## PIAT_GET_FILE_SIZE:

```fides
pub const PIAT_GET_FILE_SIZE: i64 = 9;
```

*Defined at line 2415*

---

## PIAT_GET_COMMAND_LINE_A:

```fides
pub const PIAT_GET_COMMAND_LINE_A: i64 = 10;
```

*Defined at line 2416*

---

## PIAT_CREATE_PROCESS_A:

```fides
pub const PIAT_CREATE_PROCESS_A: i64 = 11;
```

*Defined at line 2417*

---

## PIAT_WAIT_FOR_SINGLE_OBJECT:

```fides
pub const PIAT_WAIT_FOR_SINGLE_OBJECT: i64 = 12;
```

*Defined at line 2418*

---

## PeirRdata

```fides
pub struct PeirRdata {
```

**Fields:**

| Name | Type |
|------|------|
| `data` | `Vec<i64>` |
| `iat_offsets` | `Vec<i64>` |
| `import_dir_offset` | `i64` |
| `string_offsets` | `i64` |
| `ilt_offsets` | `Vec<i64>` |
| `iat_offset` | `i64` |
| `iat_size` | `i64` |
| `dll_iat_offsets` | `Vec<i64>` |
| `dll_name_offs` | `Vec<i64>` |
| `import_by_name_offsets` | `Vec<i64>` |

*Defined at line 2423*

---

## PeirRdata_new

```fides
pub fn PeirRdata_new() -> PeirRdata {
```

**Returns:** `PeirRdata`

*Defined at line 2444*

---

## PeirRdata_add_string

Add a null-terminated string to .rdata, return its offset

```fides
pub fn PeirRdata_add_string(self: &mut PeirRdata, s: str) -> i64 {
```

**Parameters:**

- `self`: `&mut PeirRdata`
- `s`: `str`

**Returns:** `i64`

*Defined at line 2451*

---

## PeirRdata_string_offset

Look up a string's .rdata offset, or -1 (Rust Option<usize> -> None).

```fides
pub fn PeirRdata_string_offset(self: &PeirRdata, s: str) -> i64 {
```

**Parameters:**

- `self`: `&PeirRdata`
- `s`: `str`

**Returns:** `i64`

*Defined at line 2477*

---

## PeirRdata_build_strings

The data of a bare build: the string constants, no import table.

```fides
pub fn PeirRdata_build_strings(self: &mut PeirRdata, string_constants: Vec<str>) -> void {
```

**Parameters:**

- `self`: `&mut PeirRdata`
- `string_constants`: `Vec<str>`

**Returns:** `void`

*Defined at line 2503*

---

## PeirRdata_build

Build the complete .rdata section
string_constants: all string literals from the IR

```fides
pub fn PeirRdata_build(self: &mut PeirRdata, string_constants: Vec<str>) -> void {
```

**Parameters:**

- `self`: `&mut PeirRdata`
- `string_constants`: `Vec<str>`

**Returns:** `void`

*Defined at line 2519*

---

## PeirRdata_patch_rvas

Patch RVAs in the import table now that we know the .rdata RVA

```fides
pub fn PeirRdata_patch_rvas(self: &mut PeirRdata, rdata_rva: i64) -> void {
```

**Parameters:**

- `self`: `&mut PeirRdata`
- `rdata_rva`: `i64`

**Returns:** `void`

*Defined at line 2661*

---

## pe_to_hex

Lowercase hex of a non-negative i64 (no 0x prefix), for diagnostics/dumps.

```fides
pub fn pe_to_hex(v: i64) -> str {
```

**Parameters:**

- `v`: `i64`

**Returns:** `str`

*Defined at line 2705*

---

## pe_bytes_hex

Hex dump of a byte vector (contiguous lowercase hex).

```fides
pub fn pe_bytes_hex(bytes: Vec<i64>) -> str {
```

**Parameters:**

- `bytes`: `Vec<i64>`

**Returns:** `str`

*Defined at line 2721*

---

## pe_push_u64

```fides
pub fn pe_push_u64(t: Vec<i64>, v: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `Vec<i64>`
- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2734*

---

## pe_push_u32

```fides
pub fn pe_push_u32(t: Vec<i64>, v: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `Vec<i64>`
- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2747*

---

## pe_push_u16

```fides
pub fn pe_push_u16(t: Vec<i64>, v: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `Vec<i64>`
- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2756*

---

## pe_put_u32

```fides
pub fn pe_put_u32(t: Vec<i64>, at: i64, v: i64) -> Vec<i64> {
```

**Parameters:**

- `t`: `Vec<i64>`
- `at`: `i64`
- `v`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2763*

---

## pe_push_str

```fides
pub fn pe_push_str(t: Vec<i64>, s: str) -> Vec<i64> {
```

**Parameters:**

- `t`: `Vec<i64>`
- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 2772*

---

## PE_IMAGE_BASE:

```fides
pub const PE_IMAGE_BASE: i64 = 0x140000000;
```

*Defined at line 2786*

---

## PE_SECTION_ALIGN:

```fides
pub const PE_SECTION_ALIGN: i64 = 0x1000;
```

*Defined at line 2787*

---

## PE_FILE_ALIGN:

```fides
pub const PE_FILE_ALIGN: i64 = 0x200;
```

*Defined at line 2788*

---

## PE_TEXT_RVA:

```fides
pub const PE_TEXT_RVA: i64 = 0x1000;
```

*Defined at line 2789*

---

## pe_align_up

```fides
pub fn pe_align_up(val: i64, align: i64) -> i64 {
```

**Parameters:**

- `val`: `i64`
- `align`: `i64`

**Returns:** `i64`

*Defined at line 2790*

---

## PE_EFI_IMAGE_BASE:

ImageBase of a UEFI application. The firmware loads it where it likes
and applies the .reloc section (pe_build_efi).

```fides
pub const PE_EFI_IMAGE_BASE: i64 = 0x400000;
```

*Defined at line 2796*

---

## PE_SUBSYSTEM_EFI_APPLICATION:

PE subsystem of a UEFI application (EFI_APPLICATION)

```fides
pub const PE_SUBSYSTEM_EFI_APPLICATION: i64 = 10;
```

*Defined at line 2798*

---

## pe_build_efi

A UEFI application (`fid emit --uefi`, pe_ir_generate_uefi): PE32+,
machine AMD64, subsystem EFI_APPLICATION, no imports (the firmware hands
the entry point its system table), three sections:
.text   RVA 0x1000: the flat binary's code, `code_size` bytes (a
multiple of 4 KB); the entry point is its first byte
.data   right after it: the rest of `flat` (strings, globals, the
allocator state), read/write
.reloc  DIR64 base relocations of `abs`: offsets in `flat` of 8-byte
absolute addresses linked at PE_EFI_IMAGE_BASE
The relocations are kept (no IMAGE_FILE_RELOCS_STRIPPED), so the image
loads at any address.

```fides
pub fn pe_build_efi(flat: Vec, code_size: i64, abs: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `flat`: `Vec`
- `code_size`: `i64`
- `abs`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2811*

---

## pe_efi_relocs

The .reloc section of a UEFI image: one block per 4 KB page of DIR64
entries (type 10) for each offset in `abs` (RVA = PE_TEXT_RVA + offset),
each block padded to 4 bytes with an ABSOLUTE entry. Without any, one
block of padding only: the image still says it can be relocated.

```fides
pub fn pe_efi_relocs(abs: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `abs`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 2874*

---

## pe_build_file

Build a complete PE32+ executable from .text and .rdata section data

```fides
pub fn pe_build_file(text: Vec<i64>, rdata: Vec<i64>, entry_point_rva: i64, import_dir_rva: i64, iat_rva: i64, iat_size: i64, rdata_rva: i64) -> Vec<i64> {
```

**Parameters:**

- `text`: `Vec<i64>`
- `rdata`: `Vec<i64>`
- `entry_point_rva`: `i64`
- `import_dir_rva`: `i64`
- `iat_rva`: `i64`
- `iat_size`: `i64`
- `rdata_rva`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2887*

---

## pe_build_file_img

TG13: pe_build_file as a layout over image_writer (same bytes): a full
DOS header, COFF header, PE32+ optional header (console, 4 GB stack
reserve, imports in directories 1 and 12), .text and .rdata.

```fides
pub fn pe_build_file_img(text: &Vec<i64>, rdata: &Vec<i64>, entry_point_rva: i64, import_dir_rva: i64, iat_rva: i64, iat_size: i64, rdata_rva: i64) -> Vec<i64> {
```

**Parameters:**

- `text`: `&Vec<i64>`
- `rdata`: `&Vec<i64>`
- `entry_point_rva`: `i64`
- `import_dir_rva`: `i64`
- `iat_rva`: `i64`
- `iat_size`: `i64`
- `rdata_rva`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2897*

---

## PE_COFF_SYMTAB_PTR_AT:

File offsets in the COFF header of a pe_build_file image (DOS header 64
bytes, "PE\0\0" 4, then Machine, NumberOfSections, TimeDateStamp).

```fides
pub const PE_COFF_SYMTAB_PTR_AT: i64 = 76;
```

*Defined at line 2939*

---

## PE_COFF_NSYMS_AT:

```fides
pub const PE_COFF_NSYMS_AT: i64 = 80;
```

*Defined at line 2940*

---

## pe_append_coff_symbols

Append a COFF symbol table to `image`, one function symbol per entry of
`names` at .text offset `offs[i]`, and point the COFF header at it.
The table goes after the last section (the loader maps sections only,
so it is not part of the image in memory), as MinGW's linker writes it:
llvm-nm, llvm-objdump -t, dumpbin /symbols and gdb name our functions
from it. It is not a PDB: Visual Studio and WinDbg want one (DB3, left).
Record (18 bytes): Name (8: inline when at most 8 bytes, else 0 then the
string table offset), Value (offset in the section), SectionNumber 1
(.text), Type 0x20 (function), StorageClass 2 (external), 0 aux records.
Then the string table: its size (4 bytes, counted) and NUL-ended names.

```fides
pub fn pe_append_coff_symbols(image: &mut Vec<i64>, names: Vec<str>, offs: Vec<i64>) {
```

**Parameters:**

- `image`: `&mut Vec<i64>`
- `names`: `Vec<str>`
- `offs`: `Vec<i64>`

*Defined at line 2962*

---

## pe_append_coff_symbols_pre

pe_append_coff_symbols with the string table starting with the names
`pre` (DB3: the long names of the debug sections, which their section
headers name as "/4", "/18", ... : pe_add_debug_sections).

```fides
pub fn pe_append_coff_symbols_pre(image: &mut Vec<i64>, pre: Vec<str>, names: Vec<str>, offs: Vec<i64>) {
```

**Parameters:**

- `image`: `&mut Vec<i64>`
- `pre`: `Vec<str>`
- `names`: `Vec<str>`
- `offs`: `Vec<i64>`

*Defined at line 2969*

---

## pe_dwarf_section_names

The DWARF sections pe_add_debug_sections adds, in that order (N9:
pe_add_dwarf adds the fourth, .debug_frame, too; a three-section image
leaves its name unused in the string table).

```fides
pub fn pe_dwarf_section_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 2995*

---

## pe_get_u32

```fides
pub fn pe_get_u32(v: &Vec<i64>, at: i64) -> i64 {
```

**Parameters:**

- `v`: `&Vec<i64>`
- `at`: `i64`

**Returns:** `i64`

*Defined at line 3044*

---

## pe_set_u32

```fides
pub fn pe_set_u32(v: &mut Vec<i64>, at: i64, val: i64) {
```

**Parameters:**

- `v`: `&mut Vec<i64>`
- `at`: `i64`
- `val`: `i64`

*Defined at line 3048*

---

## pe_dwarf_abbrev

.debug_abbrev: 1 the compile unit (producer, language, name, low_pc,
high_pc as a length, stmt_list), 2 a function (name, low_pc, high_pc,
external). The same as nat_enc.fd's ELF debug sections.

```fides
pub fn pe_dwarf_abbrev() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 3064*

---

## pe_dwarf_info

.debug_info (DWARF 4): the compile unit of `file`, a function entry per
[names, starts, ends) (.text offsets, in address order).

```fides
pub fn pe_dwarf_info(file: str, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `file`: `str`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 3102*

---

## pe_dwarf_line

.debug_line (DWARF 4) of the rows [.text offset, line, end?]: one file,
a sequence per function (an end row closes it).

```fides
pub fn pe_dwarf_line(file: str, rows: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `file`: `str`
- `rows`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 3144*

---

## pe_add_debug_sections

A pe_build_file image (two sections, 512 bytes of headers) with the
three DWARF sections after .rdata. The headers grow to 1024 bytes for
the five section headers: .text and .rdata move 512 bytes down the file
(their addresses in memory do not change), SizeOfImage covers the new
sections. Another image comes back as it is. The COFF string table
with the section names is written after (pe_append_coff_symbols_pre).

```fides
pub fn pe_add_debug_sections(image: Vec<i64>, abbrev: Vec<i64>, info: Vec<i64>, line: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `image`: `Vec<i64>`
- `abbrev`: `Vec<i64>`
- `info`: `Vec<i64>`
- `line`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 3259*

---

## pe_add_debug_sections_n

N9: pe_add_debug_sections with the sections `parts`, named in the
order of pe_dwarf_section_names (at most its four: .debug_abbrev,
.debug_info, .debug_line, .debug_frame). Three parts give the bytes
pe_add_debug_sections always wrote.

```fides
pub fn pe_add_debug_sections_n(image: Vec<i64>, parts: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `image`: `Vec<i64>`
- `parts`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 3271*

---

## pe_add_dwarf

N9: `image` (a pe_build_file image whose .text is `code`) with the
DWARF of a debug build: .debug_abbrev, .debug_info (functions with
their declaration lines, parameters, variables and types), .debug_line
(pe_dwarf_line of `rows`) and .debug_frame (an FDE per function).
`vars` and `types` are in nat_enc.fd's g_nat_dbg_vars / g_nat_dbg_types
formats (vars: one entry per name of `names`, same order). nat_enc's
globals are set for the call and given back after.
CT9: a function shows under its name as written (`Point::len` for
`Point_len`, `max<i64>` for `max__i64`) when g_pe_dbg_shown_pairs
names it (pe_dbg_shown_for).
BG243: the calls inlined into each function (g_pe_dwarf_inl, when it
has an entry per name) become DW_TAG_inlined_subroutine entries of an
abstract DW_TAG_subprogram per callee (nat_enc.fd g_nat_dbg_inl).

```fides
pub fn pe_add_dwarf(image: Vec<i64>, code: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, rows: Vec<i64>, file: str, vars: Vec<str>, types: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `image`: `Vec<i64>`
- `code`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `rows`: `Vec<i64>`
- `file`: `str`
- `vars`: `Vec<str>`
- `types`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 3432*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
