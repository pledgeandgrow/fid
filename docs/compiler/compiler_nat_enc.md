[All modules](index.md)

# compiler::nat_enc -- Fides Documentation

## Contents

- [NAT_A64:](#NAT_A64:) (const)
- [NAT_RV64:](#NAT_RV64:) (const)
- [NAT_IMAGE_BASE:](#NAT_IMAGE_BASE:) (const)
- [NATFIX_A64_B26:](#NATFIX_A64_B26:) (const)
- [NATFIX_A64_B19:](#NATFIX_A64_B19:) (const)
- [NATFIX_A64_ADRP_ADD:](#NATFIX_A64_ADRP_ADD:) (const)
- [NATFIX_RV_JAL:](#NATFIX_RV_JAL:) (const)
- [NATFIX_RV_AUIPC_I:](#NATFIX_RV_AUIPC_I:) (const)
- [NATFIX_ABS64:](#NATFIX_ABS64:) (const)
- [NATFIX_X64_REL8:](#NATFIX_X64_REL8:) (const)
- [NATFIX_RV_B12:](#NATFIX_RV_B12:) (const)
- [NATFIX_A64_B14:](#NATFIX_A64_B14:) (const)
- [NATFIX_SELF32:](#NATFIX_SELF32:) (const)
- [NatFix](#NatFix) (struct)
- [NatEnc](#NatEnc) (struct)
- [NatEnc_new](#NatEnc_new) (function)
- [NatEnc_pos](#NatEnc_pos) (function)
- [NatEnc_label](#NatEnc_label) (function)
- [NatEnc_place](#NatEnc_place) (function)
- [NatEnc_byte](#NatEnc_byte) (function)
- [NatEnc_word](#NatEnc_word) (function)
- [NatEnc_quad](#NatEnc_quad) (function)
- [NatEnc_align](#NatEnc_align) (function)
- [NatEnc_fix](#NatEnc_fix) (function)
- [NatEnc_fix_at](#NatEnc_fix_at) (function)
- [NatEnc_resolve](#NatEnc_resolve) (function)
- [A64_SP:](#A64_SP:) (const)
- [A64_ZR:](#A64_ZR:) (const)
- [A64_FP:](#A64_FP:) (const)
- [A64_LR:](#A64_LR:) (const)
- [A64_EQ:](#A64_EQ:) (const)
- [A64_NE:](#A64_NE:) (const)
- [A64_HS:](#A64_HS:) (const)
- [A64_LO:](#A64_LO:) (const)
- [A64_MI:](#A64_MI:) (const)
- [A64_VS:](#A64_VS:) (const)
- [A64_HI:](#A64_HI:) (const)
- [A64_LS:](#A64_LS:) (const)
- [A64_GE:](#A64_GE:) (const)
- [A64_LT:](#A64_LT:) (const)
- [A64_GT:](#A64_GT:) (const)
- [A64_LE:](#A64_LE:) (const)
- [a64_rrr](#a64_rrr) (function)
- [a64_add](#a64_add) (function)
- [a64_adds](#a64_adds) (function)
- [a64_sub](#a64_sub) (function)
- [a64_subs](#a64_subs) (function)
- [a64_and](#a64_and) (function)
- [a64_orr](#a64_orr) (function)
- [a64_eor](#a64_eor) (function)
- [a64_mul](#a64_mul) (function)
- [a64_smulh](#a64_smulh) (function)
- [a64_umulh](#a64_umulh) (function)
- [a64_sdiv](#a64_sdiv) (function)
- [a64_udiv](#a64_udiv) (function)
- [a64_lslv](#a64_lslv) (function)
- [a64_lsrv](#a64_lsrv) (function)
- [a64_asrv](#a64_asrv) (function)
- [a64_msub](#a64_msub) (function)
- [a64_madd](#a64_madd) (function)
- [nat_par_moves](#nat_par_moves) (function)
- [a64_bic](#a64_bic) (function)
- [a64_add_imm](#a64_add_imm) (function)
- [a64_sub_imm](#a64_sub_imm) (function)
- [a64_add_ext](#a64_add_ext) (function)
- [a64_add_base](#a64_add_base) (function)
- [a64_sub_ext](#a64_sub_ext) (function)
- [a64_sub_imm_lsl12](#a64_sub_imm_lsl12) (function)
- [a64_add_lsl](#a64_add_lsl) (function)
- [a64_mov](#a64_mov) (function)
- [a64_mvn](#a64_mvn) (function)
- [a64_neg](#a64_neg) (function)
- [a64_negs](#a64_negs) (function)
- [a64_cmp](#a64_cmp) (function)
- [a64_cmp_asr63](#a64_cmp_asr63) (function)
- [a64_cset](#a64_cset) (function)
- [a64_csel](#a64_csel) (function)
- [a64_sxtb](#a64_sxtb) (function)
- [a64_sxth](#a64_sxth) (function)
- [a64_sxtw](#a64_sxtw) (function)
- [a64_uxtb](#a64_uxtb) (function)
- [a64_uxth](#a64_uxth) (function)
- [a64_uxtw](#a64_uxtw) (function)
- [a64_asr63](#a64_asr63) (function)
- [a64_li](#a64_li) (function)
- [a64_movz_hw](#a64_movz_hw) (function)
- [a64_movn_hw](#a64_movn_hw) (function)
- [a64_shift_imm](#a64_shift_imm) (function)
- [a64_ldst_fits](#a64_ldst_fits) (function)
- [a64_ldr_sized](#a64_ldr_sized) (function)
- [a64_str_sized](#a64_str_sized) (function)
- [a64_ldr_roff](#a64_ldr_roff) (function)
- [a64_str_roff](#a64_str_roff) (function)
- [a64_ldr](#a64_ldr) (function)
- [a64_str](#a64_str) (function)
- [a64_ldstp_fits](#a64_ldstp_fits) (function)
- [a64_stp](#a64_stp) (function)
- [a64_ldp](#a64_ldp) (function)
- [a64_wb_fits](#a64_wb_fits) (function)
- [a64_ldst_opc](#a64_ldst_opc) (function)
- [a64_wb_word](#a64_wb_word) (function)
- [a64_ldr_wb](#a64_ldr_wb) (function)
- [a64_str_wb](#a64_str_wb) (function)
- [a64_b](#a64_b) (function)
- [a64_bl](#a64_bl) (function)
- [a64_bcond](#a64_bcond) (function)
- [a64_cbz](#a64_cbz) (function)
- [a64_cbnz](#a64_cbnz) (function)
- [a64_bcond_skip1](#a64_bcond_skip1) (function)
- [a64_blr](#a64_blr) (function)
- [a64_br](#a64_br) (function)
- [nat_jt_entry](#nat_jt_entry) (function)
- [a64_ret](#a64_ret) (function)
- [a64_svc0](#a64_svc0) (function)
- [a64_brk](#a64_brk) (function)
- [a64_addr](#a64_addr) (function)
- [a64_fmov_dx](#a64_fmov_dx) (function)
- [a64_fmov_xd](#a64_fmov_xd) (function)
- [a64_fmov_sw](#a64_fmov_sw) (function)
- [a64_fmov_ws](#a64_fmov_ws) (function)
- [a64_fadd](#a64_fadd) (function)
- [a64_fsub](#a64_fsub) (function)
- [a64_fmul](#a64_fmul) (function)
- [a64_fdiv](#a64_fdiv) (function)
- [a64_fcmp](#a64_fcmp) (function)
- [a64_fcvt_ds](#a64_fcvt_ds) (function)
- [a64_fcvt_sd](#a64_fcvt_sd) (function)
- [a64_scvtf](#a64_scvtf) (function)
- [a64_fcvtzs](#a64_fcvtzs) (function)
- [a64_fsqrt](#a64_fsqrt) (function)
- [a64_fabs](#a64_fabs) (function)
- [a64_frintm](#a64_frintm) (function)
- [a64_frintp](#a64_frintp) (function)
- [a64_frintz](#a64_frintz) (function)
- [a64_frinta](#a64_frinta) (function)
- [a64_ldaxr](#a64_ldaxr) (function)
- [a64_stlxr](#a64_stlxr) (function)
- [a64_ldar](#a64_ldar) (function)
- [a64_stlr](#a64_stlr) (function)
- [a64_clrex](#a64_clrex) (function)
- [a64_dmb](#a64_dmb) (function)
- [a64_cbnz_w_words](#a64_cbnz_w_words) (function)
- [a64_bcond_words](#a64_bcond_words) (function)
- [a64_cbnz_words](#a64_cbnz_words) (function)
- [a64_tbz_words](#a64_tbz_words) (function)
- [a64_tbz_label](#a64_tbz_label) (function)
- [RV_ZERO:](#RV_ZERO:) (const)
- [RV_RA:](#RV_RA:) (const)
- [RV_SP:](#RV_SP:) (const)
- [RV_S0:](#RV_S0:) (const)
- [RV_A0:](#RV_A0:) (const)
- [RV_A7:](#RV_A7:) (const)
- [RV_T0:](#RV_T0:) (const)
- [RV_T6:](#RV_T6:) (const)
- [rv_r](#rv_r) (function)
- [rv_i](#rv_i) (function)
- [rv_s](#rv_s) (function)
- [rv_b_off](#rv_b_off) (function)
- [rv_b_label](#rv_b_label) (function)
- [rv_call_near](#rv_call_near) (function)
- [RV_BEQ:](#RV_BEQ:) (const)
- [RV_BNE:](#RV_BNE:) (const)
- [RV_BLT:](#RV_BLT:) (const)
- [RV_BGE:](#RV_BGE:) (const)
- [RV_BLTU:](#RV_BLTU:) (const)
- [RV_BGEU:](#RV_BGEU:) (const)
- [rv_add](#rv_add) (function)
- [rv_sub](#rv_sub) (function)
- [rv_sll](#rv_sll) (function)
- [rv_slt](#rv_slt) (function)
- [rv_sltu](#rv_sltu) (function)
- [rv_xor](#rv_xor) (function)
- [rv_srl](#rv_srl) (function)
- [rv_sra](#rv_sra) (function)
- [rv_or](#rv_or) (function)
- [rv_and](#rv_and) (function)
- [rv_mul](#rv_mul) (function)
- [rv_mulh](#rv_mulh) (function)
- [rv_mulhu](#rv_mulhu) (function)
- [rv_div](#rv_div) (function)
- [rv_divu](#rv_divu) (function)
- [rv_rem](#rv_rem) (function)
- [rv_remu](#rv_remu) (function)
- [rv_addi](#rv_addi) (function)
- [rv_xori](#rv_xori) (function)
- [rv_andi](#rv_andi) (function)
- [rv_sltiu](#rv_sltiu) (function)
- [rv_slli](#rv_slli) (function)
- [rv_srli](#rv_srli) (function)
- [rv_srai](#rv_srai) (function)
- [rv_addiw](#rv_addiw) (function)
- [rv_addw](#rv_addw) (function)
- [rv_subw](#rv_subw) (function)
- [rv_mulw](#rv_mulw) (function)
- [rv_mv](#rv_mv) (function)
- [rv_shadd](#rv_shadd) (function)
- [rv_zext_w](#rv_zext_w) (function)
- [rv_sext_b](#rv_sext_b) (function)
- [rv_sext_h](#rv_sext_h) (function)
- [rv_zext_h](#rv_zext_h) (function)
- [rv_andn](#rv_andn) (function)
- [rv_czero_eqz](#rv_czero_eqz) (function)
- [rv_czero_nez](#rv_czero_nez) (function)
- [rv_lui](#rv_lui) (function)
- [rv_jalr](#rv_jalr) (function)
- [rv_ret](#rv_ret) (function)
- [rv_ecall](#rv_ecall) (function)
- [rv_ebreak](#rv_ebreak) (function)
- [rv_load](#rv_load) (function)
- [rv_store](#rv_store) (function)
- [rv_fits12](#rv_fits12) (function)
- [rv_li](#rv_li) (function)
- [rv_jal](#rv_jal) (function)
- [rv_addr](#rv_addr) (function)
- [rv_call](#rv_call) (function)
- [rv_far_jump](#rv_far_jump) (function)
- [rv_fmv_dx](#rv_fmv_dx) (function)
- [rv_fmv_xd](#rv_fmv_xd) (function)
- [rv_fmv_wx](#rv_fmv_wx) (function)
- [rv_fmv_xw](#rv_fmv_xw) (function)
- [rv_fop_d](#rv_fop_d) (function)
- [rv_fadd_d](#rv_fadd_d) (function)
- [rv_fsub_d](#rv_fsub_d) (function)
- [rv_fmul_d](#rv_fmul_d) (function)
- [rv_fdiv_d](#rv_fdiv_d) (function)
- [rv_feq_d](#rv_feq_d) (function)
- [rv_flt_d](#rv_flt_d) (function)
- [rv_fle_d](#rv_fle_d) (function)
- [rv_fcvt_dl](#rv_fcvt_dl) (function)
- [rv_fcvt_ld](#rv_fcvt_ld) (function)
- [rv_fcvt_ds](#rv_fcvt_ds) (function)
- [rv_fcvt_ld_rm](#rv_fcvt_ld_rm) (function)
- [rv_fsqrt_d](#rv_fsqrt_d) (function)
- [rv_fsgnj_d](#rv_fsgnj_d) (function)
- [rv_fsgnjx_d](#rv_fsgnjx_d) (function)
- [rv_amoadd_d](#rv_amoadd_d) (function)
- [rv_lr_d](#rv_lr_d) (function)
- [rv_sc_d](#rv_sc_d) (function)
- [rv_fence](#rv_fence) (function)
- [rv_fcvt_sd](#rv_fcvt_sd) (function)
- [nat_rvc_of](#nat_rvc_of) (function)
- [nat_rv_b_bits](#nat_rv_b_bits) (function)
- [nat_rv_j_bits](#nat_rv_j_bits) (function)
- [nat_rv_b_off_of](#nat_rv_b_off_of) (function)
- [nat_rv_j_off_of](#nat_rv_j_off_of) (function)
- [nat_rvc_expand](#nat_rvc_expand) (function)
- [NatRvcMap](#NatRvcMap) (struct)
- [NatRvcMap_new](#NatRvcMap_new) (function)
- [nat_rvc_pos](#nat_rvc_pos) (function)
- [nat_rvc_compress](#nat_rvc_compress) (function)
- [nat_rvc_unit_check](#nat_rvc_unit_check) (function)
- [nat_elf_page](#nat_elf_page) (function)
- [nat_elf_image_base](#nat_elf_image_base) (function)
- [NAT_ELF_NOTE_AT:](#NAT_ELF_NOTE_AT:) (const)
- [nat_elf_osabi](#nat_elf_osabi) (function)
- [nat_elf_note_name](#nat_elf_note_name) (function)
- [nat_elf_note_section](#nat_elf_note_section) (function)
- [nat_elf_note](#nat_elf_note) (function)
- [nat_elf](#nat_elf) (function)
- [nat_elf_machine](#nat_elf_machine) (function)
- [nat_elf_flags](#nat_elf_flags) (function)
- [nat_elf_so_machine](#nat_elf_so_machine) (function)
- [nat_elf_dynsym_defines](#nat_elf_dynsym_defines) (function)
- [nat_elf_find_lib](#nat_elf_find_lib) (function)
- [nat_elf_link_missing](#nat_elf_link_missing) (function)
- [nat_elf_img](#nat_elf_img) (function)
- [nat_pe_dll_for](#nat_pe_dll_for) (function)
- [nat_pe_export_set](#nat_pe_export_set) (function)
- [nat_pe_find_dll](#nat_pe_find_dll) (function)
- [nat_pe_link_missing](#nat_pe_link_missing) (function)
- [nat_pe_sig_plain](#nat_pe_sig_plain) (function)
- [nat_pe_win64_stub](#nat_pe_win64_stub) (function)
- [nat_pe_img](#nat_pe_img) (function)
- [nat_pe](#nat_pe) (function)
- [nat_pe_symbols](#nat_pe_symbols) (function)
- [nat_pe_unwind_pdata](#nat_pe_unwind_pdata) (function)
- [nat_pe_unwind](#nat_pe_unwind) (function)
- [nat_pe_set_dir](#nat_pe_set_dir) (function)
- [NAT_PE_CODE_VA:](#NAT_PE_CODE_VA:) (const)
- [nat_pe_add_sections](#nat_pe_add_sections) (function)
- [nat_pe_symbols_pre](#nat_pe_symbols_pre) (function)
- [nat_pe_debug](#nat_pe_debug) (function)
- [nat_debug_line_at](#nat_debug_line_at) (function)
- [NatDeclSrc](#NatDeclSrc) (struct)
- [NatDeclSrc_new](#NatDeclSrc_new) (function)
- [nat_dbg_decl_on](#nat_dbg_decl_on) (function)
- [nat_dbg_decl_line](#nat_dbg_decl_line) (function)
- [nat_dbg_decl_of](#nat_dbg_decl_of) (function)
- [nat_dbg_set_full_paths](#nat_dbg_set_full_paths) (function)
- [nat_dbg_written_path](#nat_dbg_written_path) (function)
- [nat_dbg_scope_end](#nat_dbg_scope_end) (function)
- [nat_elf_debug](#nat_elf_debug) (function)
- [NatDwarf](#NatDwarf) (struct)
- [nat_dwarf_build](#nat_dwarf_build) (function)
- [nat_eh_frame_on](#nat_eh_frame_on) (function)
- [nat_eh_frame](#nat_eh_frame) (function)
- [nat_eh_frame_hdr](#nat_eh_frame_hdr) (function)
- [nat_elf_eh_frame](#nat_elf_eh_frame) (function)
- [NAT_X64:](#NAT_X64:) (const)
- [NATFIX_X64_REL32:](#NATFIX_X64_REL32:) (const)
- [NATFIX_RVA32:](#NATFIX_RVA32:) (const)
- [X64_O:](#X64_O:) (const)
- [X64_B:](#X64_B:) (const)
- [X64_AE:](#X64_AE:) (const)
- [X64_E:](#X64_E:) (const)
- [X64_NE:](#X64_NE:) (const)
- [X64_BE:](#X64_BE:) (const)
- [X64_A:](#X64_A:) (const)
- [X64_S:](#X64_S:) (const)
- [X64_P:](#X64_P:) (const)
- [X64_NP:](#X64_NP:) (const)
- [X64_L:](#X64_L:) (const)
- [X64_GE:](#X64_GE:) (const)
- [X64_LE:](#X64_LE:) (const)
- [X64_G:](#X64_G:) (const)
- [x64_rex](#x64_rex) (function)
- [x64_modrm_rr](#x64_modrm_rr) (function)
- [NatEnc_u32](#NatEnc_u32) (function)
- [x64_mov](#x64_mov) (function)
- [x64_mov32](#x64_mov32) (function)
- [x64_li](#x64_li) (function)
- [x64_li_zero](#x64_li_zero) (function)
- [x64_li_u32](#x64_li_u32) (function)
- [x64_li_s32](#x64_li_s32) (function)
- [x64_li_abs](#x64_li_abs) (function)
- [x64_alu](#x64_alu) (function)
- [x64_imul](#x64_imul) (function)
- [x64_alu_mem](#x64_alu_mem) (function)
- [x64_cmp_mem_imm](#x64_cmp_mem_imm) (function)
- [x64_alu_to_mem](#x64_alu_to_mem) (function)
- [x64_alu_mem_imm](#x64_alu_mem_imm) (function)
- [x64_f7](#x64_f7) (function)
- [x64_shift_cl](#x64_shift_cl) (function)
- [x64_shiftx](#x64_shiftx) (function)
- [x64_andn](#x64_andn) (function)
- [x64_blsr](#x64_blsr) (function)
- [x64_blsmsk](#x64_blsmsk) (function)
- [x64_blsi](#x64_blsi) (function)
- [x64_shift_imm](#x64_shift_imm) (function)
- [x64_cqo](#x64_cqo) (function)
- [x64_setcc](#x64_setcc) (function)
- [x64_movx](#x64_movx) (function)
- [x64_test](#x64_test) (function)
- [x64_test_bit](#x64_test_bit) (function)
- [x64_bit_cc](#x64_bit_cc) (function)
- [x64_cmov](#x64_cmov) (function)
- [x64_load](#x64_load) (function)
- [x64_store](#x64_store) (function)
- [x64_lea](#x64_lea) (function)
- [x64_lea_idx](#x64_lea_idx) (function)
- [x64_load_idx](#x64_load_idx) (function)
- [x64_store_idx](#x64_store_idx) (function)
- [x64_lea_label](#x64_lea_label) (function)
- [x64_store_label](#x64_store_label) (function)
- [x64_load_label](#x64_load_label) (function)
- [x64_load_fs](#x64_load_fs) (function)
- [x64_store_fs](#x64_store_fs) (function)
- [x64_load_gs](#x64_load_gs) (function)
- [x64_store_gs](#x64_store_gs) (function)
- [x64_align_sp16](#x64_align_sp16) (function)
- [NatEnc_rva32](#NatEnc_rva32) (function)
- [x64_call](#x64_call) (function)
- [x64_jmp](#x64_jmp) (function)
- [x64_jcc](#x64_jcc) (function)
- [x64_jcc8](#x64_jcc8) (function)
- [x64_jmp_short](#x64_jmp_short) (function)
- [x64_jcc_short](#x64_jcc_short) (function)
- [x64_call_r](#x64_call_r) (function)
- [x64_jmp_r](#x64_jmp_r) (function)
- [x64_ret](#x64_ret) (function)
- [x64_push](#x64_push) (function)
- [x64_pop](#x64_pop) (function)
- [x64_ud2](#x64_ud2) (function)
- [x64_syscall](#x64_syscall) (function)
- [x64_mfence](#x64_mfence) (function)
- [x64_add_imm](#x64_add_imm) (function)
- [x64_sub_imm](#x64_sub_imm) (function)
- [NVK_ADD64:](#NVK_ADD64:) (const)
- [NVK_SUB64:](#NVK_SUB64:) (const)
- [NVK_ADD32:](#NVK_ADD32:) (const)
- [NVK_SUB32:](#NVK_SUB32:) (const)
- [NVK_MUL32:](#NVK_MUL32:) (const)
- [NVK_AND:](#NVK_AND:) (const)
- [NVK_OR:](#NVK_OR:) (const)
- [NVK_XOR:](#NVK_XOR:) (const)
- [NVK_FADD64:](#NVK_FADD64:) (const)
- [NVK_FSUB64:](#NVK_FSUB64:) (const)
- [NVK_FMUL64:](#NVK_FMUL64:) (const)
- [NVK_FDIV64:](#NVK_FDIV64:) (const)
- [NVK_LAST:](#NVK_LAST:) (const)
- [nvk_commutes](#nvk_commutes) (function)
- [x64_vec](#x64_vec) (function)
- [x64_vld](#x64_vld) (function)
- [x64_vst](#x64_vst) (function)
- [x64_pshufd](#x64_pshufd) (function)
- [x64_punpcklqdq](#x64_punpcklqdq) (function)
- [x64_vvec](#x64_vvec) (function)
- [x64_vvld](#x64_vvld) (function)
- [x64_vvst](#x64_vvst) (function)
- [a64_vec](#a64_vec) (function)
- [a64_ldr_q](#a64_ldr_q) (function)
- [a64_str_q](#a64_str_q) (function)
- [a64_dup_2d](#a64_dup_2d) (function)
- [a64_addp_d](#a64_addp_d) (function)
- [a64_umov_d](#a64_umov_d) (function)
- [a64_ins_d](#a64_ins_d) (function)
- [rv_vsetvli](#rv_vsetvli) (function)
- [rv_vle](#rv_vle) (function)
- [rv_vse](#rv_vse) (function)
- [rv_vec](#rv_vec) (function)
- [rv_vredsum](#rv_vredsum) (function)
- [rv_vmv_x_s](#rv_vmv_x_s) (function)
- [rv_vmv_v_x](#rv_vmv_v_x) (function)
- [x64_cmppd](#x64_cmppd) (function)
- [x64_vcmppd](#x64_vcmppd) (function)
- [x64_cmppd_m](#x64_cmppd_m) (function)
- [x64_vcmppd_m](#x64_vcmppd_m) (function)
- [x64_vpbroadcastq](#x64_vpbroadcastq) (function)
- [x64_vzeroupper](#x64_vzeroupper) (function)
- [a64_fcmeq_zero_2d](#a64_fcmeq_zero_2d) (function)
- [a64_bic_16b](#a64_bic_16b) (function)
- [a64_dup_elem_2d](#a64_dup_elem_2d) (function)
- [a64_dup_d_elem](#a64_dup_d_elem) (function)
- [rv_vsll_vi](#rv_vsll_vi) (function)
- [rv_vmseq_vi](#rv_vmseq_vi) (function)
- [rv_vmerge_vim](#rv_vmerge_vim) (function)
- [rv_vslidedown_vi](#rv_vslidedown_vi) (function)
- [rv_vfmv_f_s](#rv_vfmv_f_s) (function)
- [rv_vfmv_v_f](#rv_vfmv_v_f) (function)
- [NatEnc_half](#NatEnc_half) (function)
- [t2_word](#t2_word) (function)
- [t2_movw](#t2_movw) (function)
- [t2_movt](#t2_movt) (function)
- [t2_addw](#t2_addw) (function)
- [t2_subw](#t2_subw) (function)
- [t2_alu](#t2_alu) (function)
- [t2_div](#t2_div) (function)
- [t2_shift](#t2_shift) (function)
- [t2_mov](#t2_mov) (function)
- [t2_cmp](#t2_cmp) (function)
- [t2_ldst](#t2_ldst) (function)
- [t2_push](#t2_push) (function)
- [t2_pop](#t2_pop) (function)
- [t2_b](#t2_b) (function)
- [t2_bl](#t2_bl) (function)
- [t2_bcc](#t2_bcc) (function)
- [t_bx](#t_bx) (function)
- [t_nop](#t_nop) (function)
- [t_udf](#t_udf) (function)
- [t_bkpt](#t_bkpt) (function)
- [TDP_AND:](#TDP_AND:) (const)
- [TDP_BIC:](#TDP_BIC:) (const)
- [TDP_ORR:](#TDP_ORR:) (const)
- [TDP_ORN:](#TDP_ORN:) (const)
- [TDP_EOR:](#TDP_EOR:) (const)
- [TDP_ADD:](#TDP_ADD:) (const)
- [TDP_ADC:](#TDP_ADC:) (const)
- [TDP_SBC:](#TDP_SBC:) (const)
- [TDP_SUB:](#TDP_SUB:) (const)
- [TDP_RSB:](#TDP_RSB:) (const)
- [t2_dp](#t2_dp) (function)
- [t2_dpi](#t2_dpi) (function)
- [t2_shift_imm](#t2_shift_imm) (function)
- [t2_umull](#t2_umull) (function)
- [t2_mla](#t2_mla) (function)
- [t2_mls](#t2_mls) (function)
- [t2_ext](#t2_ext) (function)
- [t2_ldst_x](#t2_ldst_x) (function)
- [t_svc](#t_svc) (function)
- [t_blx](#t_blx) (function)
- [t_mov](#t_mov) (function)
- [x64_movq_xr](#x64_movq_xr) (function)
- [x64_movq_rx](#x64_movq_rx) (function)
- [x64_sse](#x64_sse) (function)
- [x64_cvtsi2sd](#x64_cvtsi2sd) (function)
- [x64_cvttsd2si](#x64_cvttsd2si) (function)
- [x64_roundsd](#x64_roundsd) (function)
- [x64_vfmadd213sd_012](#x64_vfmadd213sd_012) (function)
- [x64_xchg_mem](#x64_xchg_mem) (function)
- [x64_lock_xadd](#x64_lock_xadd) (function)
- [x64_lock_cmpxchg](#x64_lock_cmpxchg) (function)
- [NAT_MACHO_BASE:](#NAT_MACHO_BASE:) (const)
- [NAT_MACHO_HDR:](#NAT_MACHO_HDR:) (const)
- [nat_macho_set_platform](#nat_macho_set_platform) (function)
- [nat_macho_dylib_path](#nat_macho_dylib_path) (function)
- [nat_macho_import](#nat_macho_import) (function)
- [nat_tbd_has](#nat_tbd_has) (function)
- [nat_macho_sdk](#nat_macho_sdk) (function)
- [nat_macho_tbd_path](#nat_macho_tbd_path) (function)
- [nat_macho_link_missing](#nat_macho_link_missing) (function)
- [nat_macho_split_imports](#nat_macho_split_imports) (function)
- [nat_macho_a64_stub](#nat_macho_a64_stub) (function)
- [nat_macho](#nat_macho) (function)
- [nat_macho_img](#nat_macho_img) (function)
- [nat_macho_debug](#nat_macho_debug) (function)
- [nat_macho_debug_cfi](#nat_macho_debug_cfi) (function)
- [nat_macho_dwarf_full](#nat_macho_dwarf_full) (function)
- [nat_macho_dwarf](#nat_macho_dwarf) (function)
- [nat_macho_cpu](#nat_macho_cpu) (function)
- [nat_macho_fat](#nat_macho_fat) (function)
- [nat_hot_first](#nat_hot_first) (function)
- [nat_line_files_reset](#nat_line_files_reset) (function)
- [nat_line_file_index](#nat_line_file_index) (function)
- [nat_line_fn_set](#nat_line_fn_set) (function)
- [nat_line_fn_file](#nat_line_fn_file) (function)
- [nat_elf_object](#nat_elf_object) (function)
- [nat_elf_object_img](#nat_elf_object_img) (function)
- [nat_obj_export_syms](#nat_obj_export_syms) (function)
- [nat_coff_dll_lib](#nat_coff_dll_lib) (function)
- [nat_coff_imports](#nat_coff_imports) (function)
- [nat_coff_object](#nat_coff_object) (function)
- [nat_macho_object](#nat_macho_object) (function)
- [NatAsmCode](#NatAsmCode) (struct)
- [nat_enc_image_start](#nat_enc_image_start) (function)
- [nat_asm_add](#nat_asm_add) (function)
- [nat_asm_emit](#nat_asm_emit) (function)
- [nat_asm_reg](#nat_asm_reg) (function)
- [NatAsmImm](#NatAsmImm) (struct)
- [nat_asm_encode](#nat_asm_encode) (function)
- [nat_asm_thunk](#nat_asm_thunk) (function)
- [nat_sel_ref](#nat_sel_ref) (function)
- [nat_sel_cases](#nat_sel_cases) (function)
- [nsel_addi_imms](#nsel_addi_imms) (function)
- [nat_sel_eval_case](#nat_sel_eval_case) (function)
- [nsel_exec_m](#nsel_exec_m) (function)
- [NselM](#NselM) (struct)
- [NselM_new](#NselM_new) (function)
- [nsel_mem_init](#nsel_mem_init) (function)
- [nsel_mem_rd](#nsel_mem_rd) (function)
- [nsel_ext_sz](#nsel_ext_sz) (function)
- [nsf_isnan](#nsf_isnan) (function)
- [nsf_isnan32](#nsf_isnan32) (function)
- [nsf_widen](#nsf_widen) (function)
- [nsf_narrow](#nsf_narrow) (function)
- [nsf_op](#nsf_op) (function)
- [nsf_cmp](#nsf_cmp) (function)
- [nsf_f2i_ok](#nsf_f2i_ok) (function)
- [nsf_f2i](#nsf_f2i) (function)
- [nsf_i2f](#nsf_i2f) (function)
- [NSEL_FBIN:](#NSEL_FBIN:) (const)
- [NSEL_FCMP:](#NSEL_FCMP:) (const)
- [NSEL_I2F:](#NSEL_I2F:) (const)
- [NSEL_F2I:](#NSEL_F2I:) (const)
- [NSEL_BNEZ:](#NSEL_BNEZ:) (const)
- [NSEL_BEQZ:](#NSEL_BEQZ:) (const)
- [NSEL_BRCC:](#NSEL_BRCC:) (const)
- [NSEL_TBNZ:](#NSEL_TBNZ:) (const)
- [nsf_values](#nsf_values) (function)
- [nat_sel_fref](#nat_sel_fref) (function)
- [nat_sel_eval_float](#nat_sel_eval_float) (function)
- [nat_sel_eval_mem](#nat_sel_eval_mem) (function)
- [nat_sel_eval_branch](#nat_sel_eval_branch) (function)
- [nat_sel_eval_call](#nat_sel_eval_call) (function)

## NAT_A64:

```fides
pub const NAT_A64: i64 = 1;
```

*Defined at line 18*

---

## NAT_RV64:

```fides
pub const NAT_RV64: i64 = 2;
```

*Defined at line 19*

---

## NAT_IMAGE_BASE:

Where the image (code, then data) is mapped: file offset 0x1000 holds
it, the ELF header is in front, one segment maps the file at 0x400000.

```fides
pub const NAT_IMAGE_BASE: i64 = 0x401000;
```

*Defined at line 22*

---

## NATFIX_A64_B26:

```fides
pub const NATFIX_A64_B26: i64 = 1;  // B, BL: imm26 words
```

*Defined at line 27*

---

## NATFIX_A64_B19:

```fides
pub const NATFIX_A64_B19: i64 = 2;  // B.cond, CBZ, CBNZ: imm19 at bit 5
```

*Defined at line 28*

---

## NATFIX_A64_ADRP_ADD:

```fides
pub const NATFIX_A64_ADRP_ADD: i64 = 3;  // ADRP + ADD lo12: the label's address
```

*Defined at line 29*

---

## NATFIX_RV_JAL:

```fides
pub const NATFIX_RV_JAL: i64 = 4;  // JAL: J-type imm20
```

*Defined at line 30*

---

## NATFIX_RV_AUIPC_I:

```fides
pub const NATFIX_RV_AUIPC_I: i64 = 5;  // AUIPC + I-type (ADDI, JALR): pc-relative
```

*Defined at line 31*

---

## NATFIX_ABS64:

```fides
pub const NATFIX_ABS64: i64 = 6;  // 8 data bytes: the label's absolute address
```

*Defined at line 32*

---

## NATFIX_X64_REL8:

```fides
pub const NATFIX_X64_REL8: i64 = 9;  // 1 byte: rel8 from the end of that byte
```

*Defined at line 36*

---

## NATFIX_RV_B12:

```fides
pub const NATFIX_RV_B12: i64 = 10;  // B-type word: imm[12:1], -4096..4094
```

*Defined at line 37*

---

## NATFIX_A64_B14:

```fides
pub const NATFIX_A64_B14: i64 = 21;
```

*Defined at line 41*

---

## NATFIX_SELF32:

```fides
pub const NATFIX_SELF32: i64 = 22;
```

*Defined at line 45*

---

## NatFix

```fides
pub struct NatFix {
```

**Fields:**

| Name | Type |
|------|------|
| `pos` | `i64` |
| `label` | `i64` |
| `kind` | `i64` |

*Defined at line 47*

---

## NatEnc

The image being built: bytes, label offsets (-1 = not placed yet; label
0 is never used, so a HashMap miss of 0 is never a label) and fixups.

```fides
pub struct NatEnc {
```

**Fields:**

| Name | Type |
|------|------|
| `code` | `Vec<i64>` |
| `labels` | `Vec<i64>` |
| `fixups` | `Vec<NatFix>` |
| `last_place` | `i64` |
| `mul_end` | `i64` |

*Defined at line 55*

---

## NatEnc_new

```fides
pub fn NatEnc_new() -> NatEnc {
```

**Returns:** `NatEnc`

*Defined at line 66*

---

## NatEnc_pos

```fides
pub fn NatEnc_pos(e: &NatEnc) -> i64 {
```

**Parameters:**

- `e`: `&NatEnc`

**Returns:** `i64`

*Defined at line 74*

---

## NatEnc_label

```fides
pub fn NatEnc_label(e: &mut NatEnc) -> i64 {
```

**Parameters:**

- `e`: `&mut NatEnc`

**Returns:** `i64`

*Defined at line 78*

---

## NatEnc_place

```fides
pub fn NatEnc_place(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 83*

---

## NatEnc_byte

```fides
pub fn NatEnc_byte(e: &mut NatEnc, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `b`: `i64`

*Defined at line 88*

---

## NatEnc_word

```fides
pub fn NatEnc_word(e: &mut NatEnc, w: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `w`: `i64`

*Defined at line 92*

---

## NatEnc_quad

```fides
pub fn NatEnc_quad(e: &mut NatEnc, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `v`: `i64`

*Defined at line 99*

---

## NatEnc_align

```fides
pub fn NatEnc_align(e: &mut NatEnc, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`

*Defined at line 107*

---

## NatEnc_fix

Record that the word(s) at the current position refer to `label`.

```fides
pub fn NatEnc_fix(e: &mut NatEnc, label: i64, kind: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`
- `kind`: `i64`

*Defined at line 114*

---

## NatEnc_fix_at

Record that the word(s) at `pos` refer to `label` — the CS6 function
cache replays fixups whose patch site is not the current position.

```fides
pub fn NatEnc_fix_at(e: &mut NatEnc, pos: i64, label: i64, kind: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `pos`: `i64`
- `label`: `i64`
- `kind`: `i64`

*Defined at line 122*

---

## NatEnc_resolve

```fides
pub fn NatEnc_resolve(e: &mut NatEnc) -> str {
```

**Parameters:**

- `e`: `&mut NatEnc`

**Returns:** `str`

*Defined at line 146*

---

## A64_SP:

```fides
pub const A64_SP: i64 = 31;
```

*Defined at line 241*

---

## A64_ZR:

```fides
pub const A64_ZR: i64 = 31;
```

*Defined at line 242*

---

## A64_FP:

```fides
pub const A64_FP: i64 = 29;
```

*Defined at line 243*

---

## A64_LR:

```fides
pub const A64_LR: i64 = 30;  // Condition codes
```

*Defined at line 244*

---

## A64_EQ:

```fides
pub const A64_EQ: i64 = 0;
```

*Defined at line 245*

---

## A64_NE:

```fides
pub const A64_NE: i64 = 1;
```

*Defined at line 246*

---

## A64_HS:

```fides
pub const A64_HS: i64 = 2;
```

*Defined at line 247*

---

## A64_LO:

```fides
pub const A64_LO: i64 = 3;
```

*Defined at line 248*

---

## A64_MI:

```fides
pub const A64_MI: i64 = 4;
```

*Defined at line 249*

---

## A64_VS:

```fides
pub const A64_VS: i64 = 6;
```

*Defined at line 250*

---

## A64_HI:

```fides
pub const A64_HI: i64 = 8;
```

*Defined at line 251*

---

## A64_LS:

```fides
pub const A64_LS: i64 = 9;
```

*Defined at line 252*

---

## A64_GE:

```fides
pub const A64_GE: i64 = 10;
```

*Defined at line 253*

---

## A64_LT:

```fides
pub const A64_LT: i64 = 11;
```

*Defined at line 254*

---

## A64_GT:

```fides
pub const A64_GT: i64 = 12;
```

*Defined at line 255*

---

## A64_LE:

```fides
pub const A64_LE: i64 = 13;
```

*Defined at line 256*

---

## a64_rrr

Three-register data processing: base | m << 16 | n << 5 | d.

```fides
pub fn a64_rrr(e: &mut NatEnc, base: i64, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 258*

---

## a64_add

```fides
pub fn a64_add(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 262*

---

## a64_adds

```fides
pub fn a64_adds(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 265*

---

## a64_sub

```fides
pub fn a64_sub(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 268*

---

## a64_subs

```fides
pub fn a64_subs(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 271*

---

## a64_and

```fides
pub fn a64_and(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 274*

---

## a64_orr

```fides
pub fn a64_orr(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 277*

---

## a64_eor

```fides
pub fn a64_eor(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 280*

---

## a64_mul

```fides
pub fn a64_mul(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 283*

---

## a64_smulh

```fides
pub fn a64_smulh(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 286*

---

## a64_umulh

```fides
pub fn a64_umulh(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 289*

---

## a64_sdiv

```fides
pub fn a64_sdiv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 292*

---

## a64_udiv

```fides
pub fn a64_udiv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 295*

---

## a64_lslv

```fides
pub fn a64_lslv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 298*

---

## a64_lsrv

```fides
pub fn a64_lsrv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 301*

---

## a64_asrv

```fides
pub fn a64_asrv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 304*

---

## a64_msub

d = a - n * m

```fides
pub fn a64_msub(e: &mut NatEnc, d: i64, n: i64, m: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `a`: `i64`

*Defined at line 309*

---

## a64_madd

MI3/TG8: d = a + n * m (MADD; MUL is MADD with a = xzr)

```fides
pub fn a64_madd(e: &mut NatEnc, d: i64, n: i64, m: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `a`: `i64`

*Defined at line 314*

---

## nat_par_moves

RA11: a parallel move as a sequence of single moves. dst[i] <- src[i]
all at once: every destination takes what its source held before any
of them is written. Locations are plain numbers (registers, or slots
numbered apart from them by the caller); the destinations are
distinct. Returns [d0, s0, d1, s1, ...], to run in order. A move whose
destination no other pending move still reads goes first; when none is
left, what remains are disjoint cycles (each location read once), and
one is broken by saving one destination's value in `scratch` (a
location neither list names) and reading it from there: the cycle then
unwinds to its end before another is broken, so one scratch serves
them all. A move to its own source is dropped. Empty when a
destination repeats or the lists differ in length (no such move
exists), or scratch is named by either list. Its user: the moves of
live-range splitting (RA4, nat_ir.fd ng_ls_emit), one parallel move per
phase at each place (a block edge, a jump, an edge block).

```fides
pub fn nat_par_moves(dst: &Vec<i64>, src: &Vec<i64>, scratch: i64) -> Vec<i64> {
```

**Parameters:**

- `dst`: `&Vec<i64>`
- `src`: `&Vec<i64>`
- `scratch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 333*

---

## a64_bic

MI3: d = n & ~m (BIC, shifted register, no shift)

```fides
pub fn a64_bic(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 414*

---

## a64_add_imm

ADD d, n, #imm (0..4095); n and d may be sp.

```fides
pub fn a64_add_imm(e: &mut NatEnc, d: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 419*

---

## a64_sub_imm

```fides
pub fn a64_sub_imm(e: &mut NatEnc, d: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 423*

---

## a64_add_ext

ADD d, n, m, UXTX: n and d may be sp (the extended-register form).

```fides
pub fn a64_add_ext(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 428*

---

## a64_add_base

RL6: d = n + m for an address whose base n may be sp (31): ADD
(extended register) reads sp there, ADD (shifted register) xzr. The
same word as a64_add for every other base.

```fides
pub fn a64_add_base(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 435*

---

## a64_sub_ext

```fides
pub fn a64_sub_ext(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 443*

---

## a64_sub_imm_lsl12

SUB d, n, #imm, LSL #12 (imm 0..4095: d = n - imm * 4096); n and d may
be sp. The stack probes of large frames (MI29) step sp by #1, LSL #12.

```fides
pub fn a64_sub_imm_lsl12(e: &mut NatEnc, d: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 449*

---

## a64_add_lsl

ADD d, n, m, LSL #sh (the shifted-register form: register 31 is xzr,
not sp). d = n + (m << sh), one instruction for an element address
(MI22).

```fides
pub fn a64_add_lsl(e: &mut NatEnc, d: i64, n: i64, m: i64, sh: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `sh`: `i64`

*Defined at line 456*

---

## a64_mov

MOV d, m (ORR d, xzr, m): not for sp.

```fides
pub fn a64_mov(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 461*

---

## a64_mvn

```fides
pub fn a64_mvn(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 468*

---

## a64_neg

```fides
pub fn a64_neg(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 471*

---

## a64_negs

```fides
pub fn a64_negs(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 474*

---

## a64_cmp

```fides
pub fn a64_cmp(e: &mut NatEnc, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`
- `m`: `i64`

*Defined at line 477*

---

## a64_cmp_asr63

CMP n, m, ASR #63 (SUBS xzr, n, m, ASR #63): the high half of a signed
product against the sign of its low half.

```fides
pub fn a64_cmp_asr63(e: &mut NatEnc, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`
- `m`: `i64`

*Defined at line 483*

---

## a64_cset

CSET d, cond (CSINC d, xzr, xzr, !cond)

```fides
pub fn a64_cset(e: &mut NatEnc, d: i64, cond: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `cond`: `i64`

*Defined at line 488*

---

## a64_csel

MI24: CSEL d, n, m, cond (64-bit): d = cond ? n : m.
`csel x0, x1, x2, ne` = 0x9A821020.

```fides
pub fn a64_csel(e: &mut NatEnc, d: i64, n: i64, m: i64, cond: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `cond`: `i64`

*Defined at line 494*

---

## a64_sxtb

```fides
pub fn a64_sxtb(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 499*

---

## a64_sxth

```fides
pub fn a64_sxth(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 502*

---

## a64_sxtw

```fides
pub fn a64_sxtw(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 505*

---

## a64_uxtb

```fides
pub fn a64_uxtb(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 508*

---

## a64_uxth

```fides
pub fn a64_uxth(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 511*

---

## a64_uxtw

```fides
pub fn a64_uxtw(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 514*

---

## a64_asr63

d = n >> 63 (arithmetic): 0 or -1

```fides
pub fn a64_asr63(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 519*

---

## a64_li

```fides
pub fn a64_li(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 525*

---

## a64_movz_hw

MOVZ d, #h, LSL #(16 k): d = h << 16k (h 0..0xFFFF, k 0..3)

```fides
pub fn a64_movz_hw(e: &mut NatEnc, d: i64, h: i64, k: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `h`: `i64`
- `k`: `i64`

*Defined at line 579*

---

## a64_movn_hw

MOVN d, #h, LSL #(16 k): d = ~(h << 16k)

```fides
pub fn a64_movn_hw(e: &mut NatEnc, d: i64, h: i64, k: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `h`: `i64`
- `k`: `i64`

*Defined at line 584*

---

## a64_shift_imm

TG1: d = n shifted by the constant s (0..63): kind 0 LSL (UBFM d, n,
#(-s mod 64), #(63 - s)), 1 ASR (SBFM d, n, #s, #63), 2 LSR (UBFM d, n,
#s, #63). `lsl x0, x1, #3` = 0xD37DF020, `asr x0, x1, #3` =
0x9343FC20, `lsr x0, x1, #3` = 0xD343FC20.

```fides
pub fn a64_shift_imm(e: &mut NatEnc, kind: i64, d: i64, n: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `n`: `i64`
- `s`: `i64`

*Defined at line 592*

---

## a64_ldst_fits

```fides
pub fn a64_ldst_fits(size: i64, imm: i64) -> bool {
```

**Parameters:**

- `size`: `i64`
- `imm`: `i64`

**Returns:** `bool`

*Defined at line 605*

---

## a64_ldr_sized

```fides
pub fn a64_ldr_sized(e: &mut NatEnc, t: i64, n: i64, imm: i64, size: i64, signed: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`
- `size`: `i64`
- `signed`: `bool`

*Defined at line 609*

---

## a64_str_sized

```fides
pub fn a64_str_sized(e: &mut NatEnc, t: i64, n: i64, imm: i64, size: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`
- `size`: `i64`

*Defined at line 634*

---

## a64_ldr_roff

```fides
pub fn a64_ldr_roff(e: &mut NatEnc, t: i64, n: i64, m: i64, size: i64, signed: bool, scaled: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `m`: `i64`
- `size`: `i64`
- `signed`: `bool`
- `scaled`: `bool`

*Defined at line 660*

---

## a64_str_roff

```fides
pub fn a64_str_roff(e: &mut NatEnc, t: i64, n: i64, m: i64, size: i64, scaled: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `m`: `i64`
- `size`: `i64`
- `scaled`: `bool`

*Defined at line 684*

---

## a64_ldr

```fides
pub fn a64_ldr(e: &mut NatEnc, t: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 698*

---

## a64_str

```fides
pub fn a64_str(e: &mut NatEnc, t: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 701*

---

## a64_ldstp_fits

```fides
pub fn a64_ldstp_fits(imm: i64) -> bool {
```

**Parameters:**

- `imm`: `i64`

**Returns:** `bool`

*Defined at line 708*

---

## a64_stp

stp t1, t2, [n, #imm]: t1 at n + imm, t2 at n + imm + 8.

```fides
pub fn a64_stp(e: &mut NatEnc, t1: i64, t2: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t1`: `i64`
- `t2`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 713*

---

## a64_ldp

ldp t1, t2, [n, #imm]: t1 from n + imm, t2 from n + imm + 8.

```fides
pub fn a64_ldp(e: &mut NatEnc, t1: i64, t2: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t1`: `i64`
- `t2`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 718*

---

## a64_wb_fits

```fides
pub fn a64_wb_fits(c: i64) -> bool {
```

**Parameters:**

- `c`: `i64`

**Returns:** `bool`

*Defined at line 729*

---

## a64_ldst_opc

TG8: the unsigned-offset opcode of an access of `size` bytes: STR
(load false), LDR zero-extending, or LDRS sign-extending into the x
register (signed, size below 8).

```fides
pub fn a64_ldst_opc(size: i64, load: bool, signed: bool) -> i64 {
```

**Parameters:**

- `size`: `i64`
- `load`: `bool`
- `signed`: `bool`

**Returns:** `i64`

*Defined at line 736*

---

## a64_wb_word

TG8: the writeback form of the unsigned-offset access word `opc` (its
size, V and opc bits; register fields ignored) with t, n and step c.

```fides
pub fn a64_wb_word(opc: i64, t: i64, n: i64, c: i64, pre: bool) -> i64 {
```

**Parameters:**

- `opc`: `i64`
- `t`: `i64`
- `n`: `i64`
- `c`: `i64`
- `pre`: `bool`

**Returns:** `i64`

*Defined at line 757*

---

## a64_ldr_wb

TG8: t = *(n + c) then n = n + c (pre), or t = *n then n = n + c
(post); `size` bytes, sign-extended when signed.

```fides
pub fn a64_ldr_wb(e: &mut NatEnc, t: i64, n: i64, c: i64, size: i64, signed: bool, pre: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `c`: `i64`
- `size`: `i64`
- `signed`: `bool`
- `pre`: `bool`

*Defined at line 767*

---

## a64_str_wb

TG8: *(n + c) = t then n = n + c (pre), or *n = t then n = n + c (post).

```fides
pub fn a64_str_wb(e: &mut NatEnc, t: i64, n: i64, c: i64, size: i64, pre: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `c`: `i64`
- `size`: `i64`
- `pre`: `bool`

*Defined at line 772*

---

## a64_b

```fides
pub fn a64_b(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 777*

---

## a64_bl

```fides
pub fn a64_bl(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 782*

---

## a64_bcond

```fides
pub fn a64_bcond(e: &mut NatEnc, cond: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cond`: `i64`
- `label`: `i64`

*Defined at line 787*

---

## a64_cbz

```fides
pub fn a64_cbz(e: &mut NatEnc, t: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `label`: `i64`

*Defined at line 792*

---

## a64_cbnz

```fides
pub fn a64_cbnz(e: &mut NatEnc, t: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `label`: `i64`

*Defined at line 797*

---

## a64_bcond_skip1

B.cond over the next instruction (+8): the inverse of a far branch.

```fides
pub fn a64_bcond_skip1(e: &mut NatEnc, cond: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cond`: `i64`

*Defined at line 803*

---

## a64_blr

```fides
pub fn a64_blr(e: &mut NatEnc, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`

*Defined at line 807*

---

## a64_br

MI27: BR xn, the indirect jump of a jump table (`br x16` = D61F0200).

```fides
pub fn a64_br(e: &mut NatEnc, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`

*Defined at line 811*

---

## nat_jt_entry

MI27: one jump-table entry: 4 bytes holding `label` minus the entry's
own address (NATFIX_SELF32), on every target.

```fides
pub fn nat_jt_entry(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 817*

---

## a64_ret

```fides
pub fn a64_ret(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 821*

---

## a64_svc0

```fides
pub fn a64_svc0(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 824*

---

## a64_brk

```fides
pub fn a64_brk(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 827*

---

## a64_addr

d = the address of `label` (ADRP + ADD).

```fides
pub fn a64_addr(e: &mut NatEnc, d: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `label`: `i64`

*Defined at line 832*

---

## a64_fmov_dx

```fides
pub fn a64_fmov_dx(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 839*

---

## a64_fmov_xd

```fides
pub fn a64_fmov_xd(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 842*

---

## a64_fmov_sw

```fides
pub fn a64_fmov_sw(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 845*

---

## a64_fmov_ws

```fides
pub fn a64_fmov_ws(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 848*

---

## a64_fadd

```fides
pub fn a64_fadd(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 851*

---

## a64_fsub

```fides
pub fn a64_fsub(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 854*

---

## a64_fmul

```fides
pub fn a64_fmul(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 857*

---

## a64_fdiv

```fides
pub fn a64_fdiv(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 860*

---

## a64_fcmp

```fides
pub fn a64_fcmp(e: &mut NatEnc, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`
- `m`: `i64`

*Defined at line 863*

---

## a64_fcvt_ds

```fides
pub fn a64_fcvt_ds(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 866*

---

## a64_fcvt_sd

```fides
pub fn a64_fcvt_sd(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 869*

---

## a64_scvtf

```fides
pub fn a64_scvtf(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 872*

---

## a64_fcvtzs

```fides
pub fn a64_fcvtzs(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 875*

---

## a64_fsqrt

```fides
pub fn a64_fsqrt(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 878*

---

## a64_fabs

```fides
pub fn a64_fabs(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 881*

---

## a64_frintm

```fides
pub fn a64_frintm(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 884*

---

## a64_frintp

```fides
pub fn a64_frintp(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 887*

---

## a64_frintz

```fides
pub fn a64_frintz(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 890*

---

## a64_frinta

```fides
pub fn a64_frinta(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 893*

---

## a64_ldaxr

```fides
pub fn a64_ldaxr(e: &mut NatEnc, t: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`

*Defined at line 898*

---

## a64_stlxr

STLXR Ws, Xt, [Xn]: Ws = 0 on success

```fides
pub fn a64_stlxr(e: &mut NatEnc, s: i64, t: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `s`: `i64`
- `t`: `i64`
- `n`: `i64`

*Defined at line 902*

---

## a64_ldar

```fides
pub fn a64_ldar(e: &mut NatEnc, t: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`

*Defined at line 905*

---

## a64_stlr

```fides
pub fn a64_stlr(e: &mut NatEnc, t: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`

*Defined at line 908*

---

## a64_clrex

```fides
pub fn a64_clrex(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 911*

---

## a64_dmb

```fides
pub fn a64_dmb(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 915*

---

## a64_cbnz_w_words

CBNZ Wt by a fixed word count (no fixup); negative jumps back

```fides
pub fn a64_cbnz_w_words(e: &mut NatEnc, t: i64, words: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `words`: `i64`

*Defined at line 919*

---

## a64_bcond_words

B.cond / CBNZ X by a fixed word count

```fides
pub fn a64_bcond_words(e: &mut NatEnc, cond: i64, words: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cond`: `i64`
- `words`: `i64`

*Defined at line 923*

---

## a64_cbnz_words

```fides
pub fn a64_cbnz_words(e: &mut NatEnc, t: i64, words: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `words`: `i64`

*Defined at line 926*

---

## a64_tbz_words

TG8: TBZ (nz false) / TBNZ (nz true) Xt, #bit by a fixed word count
(no fixup): b5 011011 op b40 imm14 Rt. `tbnz x3, #37, .+8` =
B7280043. bit 0..63.

```fides
pub fn a64_tbz_words(e: &mut NatEnc, t: i64, bit: i64, nz: bool, words: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `bit`: `i64`
- `nz`: `bool`
- `words`: `i64`

*Defined at line 933*

---

## a64_tbz_label

TG8: TBZ / TBNZ Xt, #bit straight to `label` (+-32 KB; NatEnc_resolve
checks the reach).

```fides
pub fn a64_tbz_label(e: &mut NatEnc, t: i64, bit: i64, nz: bool, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `bit`: `i64`
- `nz`: `bool`
- `label`: `i64`

*Defined at line 943*

---

## RV_ZERO:

```fides
pub const RV_ZERO: i64 = 0;
```

*Defined at line 953*

---

## RV_RA:

```fides
pub const RV_RA: i64 = 1;
```

*Defined at line 954*

---

## RV_SP:

```fides
pub const RV_SP: i64 = 2;
```

*Defined at line 955*

---

## RV_S0:

```fides
pub const RV_S0: i64 = 8;
```

*Defined at line 956*

---

## RV_A0:

```fides
pub const RV_A0: i64 = 10;
```

*Defined at line 957*

---

## RV_A7:

```fides
pub const RV_A7: i64 = 17;
```

*Defined at line 958*

---

## RV_T0:

```fides
pub const RV_T0: i64 = 5;
```

*Defined at line 959*

---

## RV_T6:

```fides
pub const RV_T6: i64 = 31;
```

*Defined at line 960*

---

## rv_r

```fides
pub fn rv_r(e: &mut NatEnc, f7: i64, rs2: i64, rs1: i64, f3: i64, rd: i64, op: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `f7`: `i64`
- `rs2`: `i64`
- `rs1`: `i64`
- `f3`: `i64`
- `rd`: `i64`
- `op`: `i64`

*Defined at line 961*

---

## rv_i

```fides
pub fn rv_i(e: &mut NatEnc, imm: i64, rs1: i64, f3: i64, rd: i64, op: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `imm`: `i64`
- `rs1`: `i64`
- `f3`: `i64`
- `rd`: `i64`
- `op`: `i64`

*Defined at line 965*

---

## rv_s

```fides
pub fn rv_s(e: &mut NatEnc, imm: i64, rs2: i64, rs1: i64, f3: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `imm`: `i64`
- `rs2`: `i64`
- `rs1`: `i64`
- `f3`: `i64`

*Defined at line 969*

---

## rv_b_off

A conditional branch by a fixed byte offset (no fixup): -4096..4094.

```fides
pub fn rv_b_off(e: &mut NatEnc, f3: i64, rs1: i64, rs2: i64, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `f3`: `i64`
- `rs1`: `i64`
- `rs2`: `i64`
- `off`: `i64`

*Defined at line 974*

---

## rv_b_label

TG6: a conditional branch straight to `label` (-4096..4094 bytes away;
NatEnc_resolve checks the reach).

```fides
pub fn rv_b_label(e: &mut NatEnc, f3: i64, rs1: i64, rs2: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `f3`: `i64`
- `rs1`: `i64`
- `rs2`: `i64`
- `label`: `i64`

*Defined at line 981*

---

## rv_call_near

TG6: JAL ra, label: a call in one word (+-1 MB) where AUIPC + JALR took two.

```fides
pub fn rv_call_near(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 987*

---

## RV_BEQ:

```fides
pub const RV_BEQ: i64 = 0;
```

*Defined at line 992*

---

## RV_BNE:

```fides
pub const RV_BNE: i64 = 1;
```

*Defined at line 993*

---

## RV_BLT:

```fides
pub const RV_BLT: i64 = 4;
```

*Defined at line 994*

---

## RV_BGE:

```fides
pub const RV_BGE: i64 = 5;
```

*Defined at line 995*

---

## RV_BLTU:

```fides
pub const RV_BLTU: i64 = 6;
```

*Defined at line 996*

---

## RV_BGEU:

```fides
pub const RV_BGEU: i64 = 7;
```

*Defined at line 997*

---

## rv_add

```fides
pub fn rv_add(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 998*

---

## rv_sub

```fides
pub fn rv_sub(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1001*

---

## rv_sll

```fides
pub fn rv_sll(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1004*

---

## rv_slt

```fides
pub fn rv_slt(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1007*

---

## rv_sltu

```fides
pub fn rv_sltu(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1010*

---

## rv_xor

```fides
pub fn rv_xor(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1013*

---

## rv_srl

```fides
pub fn rv_srl(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1016*

---

## rv_sra

```fides
pub fn rv_sra(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1019*

---

## rv_or

```fides
pub fn rv_or(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1022*

---

## rv_and

```fides
pub fn rv_and(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1025*

---

## rv_mul

```fides
pub fn rv_mul(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1028*

---

## rv_mulh

```fides
pub fn rv_mulh(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1031*

---

## rv_mulhu

```fides
pub fn rv_mulhu(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1034*

---

## rv_div

```fides
pub fn rv_div(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1037*

---

## rv_divu

```fides
pub fn rv_divu(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1040*

---

## rv_rem

```fides
pub fn rv_rem(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1043*

---

## rv_remu

```fides
pub fn rv_remu(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1046*

---

## rv_addi

```fides
pub fn rv_addi(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1050*

---

## rv_xori

```fides
pub fn rv_xori(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1053*

---

## rv_andi

```fides
pub fn rv_andi(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1056*

---

## rv_sltiu

```fides
pub fn rv_sltiu(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1059*

---

## rv_slli

```fides
pub fn rv_slli(e: &mut NatEnc, d: i64, a: i64, sh: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `sh`: `i64`

*Defined at line 1062*

---

## rv_srli

```fides
pub fn rv_srli(e: &mut NatEnc, d: i64, a: i64, sh: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `sh`: `i64`

*Defined at line 1065*

---

## rv_srai

```fides
pub fn rv_srai(e: &mut NatEnc, d: i64, a: i64, sh: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `sh`: `i64`

*Defined at line 1068*

---

## rv_addiw

```fides
pub fn rv_addiw(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1071*

---

## rv_addw

TG9: the 32-bit operations of RV64 (OP-32, 0x3B): the low 32 bits of
the result, sign-extended to 64. `addw a0, a1, a2` = 00C5853B,
`subw t0, t1, t2` = 407302BB, `mulw a0, a1, a2` = 02C5853B.

```fides
pub fn rv_addw(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1077*

---

## rv_subw

```fides
pub fn rv_subw(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1080*

---

## rv_mulw

```fides
pub fn rv_mulw(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1083*

---

## rv_mv

```fides
pub fn rv_mv(e: &mut NatEnc, d: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`

*Defined at line 1086*

---

## rv_shadd

Zba sh1add / sh2add / sh3add: d = a + (b << n), n = 1..3 (rd = rs2 +
(rs1 << n), rs1 = b, rs2 = a).

```fides
pub fn rv_shadd(e: &mut NatEnc, d: i64, a: i64, b: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`
- `n`: `i64`

*Defined at line 1097*

---

## rv_zext_w

Zba zext.w (add.uw d, s, zero): the low 32 bits of s, zero-extended.

```fides
pub fn rv_zext_w(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 1101*

---

## rv_sext_b

Zbb sext.b / sext.h: the low byte / halfword of s, sign-extended.

```fides
pub fn rv_sext_b(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 1105*

---

## rv_sext_h

```fides
pub fn rv_sext_h(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 1108*

---

## rv_zext_h

Zbb zext.h (RV64: packw d, s, zero): the low halfword, zero-extended.

```fides
pub fn rv_zext_h(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 1112*

---

## rv_andn

MI3: Zbb andn: d = a & ~b (rd = rs1 & ~rs2, rs1 = a, rs2 = b).

```fides
pub fn rv_andn(e: &mut NatEnc, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1116*

---

## rv_czero_eqz

MI24: Zicond czero.eqz d, a, c: d = (c == 0) ? 0 : a (funct7 0000111,
funct3 101, OP), used only under `--cpu rva23u64` (nat_ir ng_rv_zicond).

```fides
pub fn rv_czero_eqz(e: &mut NatEnc, d: i64, a: i64, c: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `c`: `i64`

*Defined at line 1121*

---

## rv_czero_nez

MI24: Zicond czero.nez d, a, c: d = (c != 0) ? 0 : a (funct3 111).

```fides
pub fn rv_czero_nez(e: &mut NatEnc, d: i64, a: i64, c: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `c`: `i64`

*Defined at line 1125*

---

## rv_lui

```fides
pub fn rv_lui(e: &mut NatEnc, d: i64, imm20: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `imm20`: `i64`

*Defined at line 1129*

---

## rv_jalr

```fides
pub fn rv_jalr(e: &mut NatEnc, d: i64, a: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`

*Defined at line 1132*

---

## rv_ret

```fides
pub fn rv_ret(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 1135*

---

## rv_ecall

```fides
pub fn rv_ecall(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 1138*

---

## rv_ebreak

```fides
pub fn rv_ebreak(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 1141*

---

## rv_load

```fides
pub fn rv_load(e: &mut NatEnc, d: i64, a: i64, imm: i64, size: i64, signed: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `imm`: `i64`
- `size`: `i64`
- `signed`: `bool`

*Defined at line 1146*

---

## rv_store

```fides
pub fn rv_store(e: &mut NatEnc, v: i64, a: i64, imm: i64, size: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `v`: `i64`
- `a`: `i64`
- `imm`: `i64`
- `size`: `i64`

*Defined at line 1172*

---

## rv_fits12

```fides
pub fn rv_fits12(v: i64) -> bool {
```

**Parameters:**

- `v`: `i64`

**Returns:** `bool`

*Defined at line 1186*

---

## rv_li

```fides
pub fn rv_li(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 1192*

---

## rv_jal

JAL d, label (+-1 MB)

```fides
pub fn rv_jal(e: &mut NatEnc, d: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `label`: `i64`

*Defined at line 1219*

---

## rv_addr

d = the address of `label` (AUIPC + ADDI).

```fides
pub fn rv_addr(e: &mut NatEnc, d: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `label`: `i64`

*Defined at line 1225*

---

## rv_call

A call anywhere in the image: AUIPC ra + JALR ra.

```fides
pub fn rv_call(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 1232*

---

## rv_far_jump

A jump anywhere in the image through t6: AUIPC t6 + JALR zero.

```fides
pub fn rv_far_jump(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 1239*

---

## rv_fmv_dx

```fides
pub fn rv_fmv_dx(e: &mut NatEnc, fd: i64, rs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `rs`: `i64`

*Defined at line 1246*

---

## rv_fmv_xd

```fides
pub fn rv_fmv_xd(e: &mut NatEnc, rd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `fs`: `i64`

*Defined at line 1249*

---

## rv_fmv_wx

```fides
pub fn rv_fmv_wx(e: &mut NatEnc, fd: i64, rs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `rs`: `i64`

*Defined at line 1252*

---

## rv_fmv_xw

```fides
pub fn rv_fmv_xw(e: &mut NatEnc, rd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `fs`: `i64`

*Defined at line 1255*

---

## rv_fop_d

```fides
pub fn rv_fop_d(e: &mut NatEnc, base: i64, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1259*

---

## rv_fadd_d

```fides
pub fn rv_fadd_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1262*

---

## rv_fsub_d

```fides
pub fn rv_fsub_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1265*

---

## rv_fmul_d

```fides
pub fn rv_fmul_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1268*

---

## rv_fdiv_d

```fides
pub fn rv_fdiv_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1271*

---

## rv_feq_d

```fides
pub fn rv_feq_d(e: &mut NatEnc, rd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1274*

---

## rv_flt_d

```fides
pub fn rv_flt_d(e: &mut NatEnc, rd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1277*

---

## rv_fle_d

```fides
pub fn rv_fle_d(e: &mut NatEnc, rd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1280*

---

## rv_fcvt_dl

```fides
pub fn rv_fcvt_dl(e: &mut NatEnc, fd: i64, rs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `rs`: `i64`

*Defined at line 1283*

---

## rv_fcvt_ld

```fides
pub fn rv_fcvt_ld(e: &mut NatEnc, rd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `fs`: `i64`

*Defined at line 1287*

---

## rv_fcvt_ds

```fides
pub fn rv_fcvt_ds(e: &mut NatEnc, fd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `fs`: `i64`

*Defined at line 1290*

---

## rv_fcvt_ld_rm

fcvt.l.d with an explicit rounding mode (1 rtz, 2 rdn, 3 rup, 4 rmm)

```fides
pub fn rv_fcvt_ld_rm(e: &mut NatEnc, rd: i64, fs: i64, rm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `fs`: `i64`
- `rm`: `i64`

*Defined at line 1294*

---

## rv_fsqrt_d

```fides
pub fn rv_fsqrt_d(e: &mut NatEnc, fd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `fs`: `i64`

*Defined at line 1297*

---

## rv_fsgnj_d

```fides
pub fn rv_fsgnj_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1300*

---

## rv_fsgnjx_d

```fides
pub fn rv_fsgnjx_d(e: &mut NatEnc, fd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 1303*

---

## rv_amoadd_d

```fides
pub fn rv_amoadd_d(e: &mut NatEnc, rd: i64, rs2: i64, rs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `rs2`: `i64`
- `rs1`: `i64`

*Defined at line 1308*

---

## rv_lr_d

```fides
pub fn rv_lr_d(e: &mut NatEnc, rd: i64, rs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `rs1`: `i64`

*Defined at line 1311*

---

## rv_sc_d

sc.d rd, rs2, (rs1): rd = 0 on success

```fides
pub fn rv_sc_d(e: &mut NatEnc, rd: i64, rs2: i64, rs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `rs2`: `i64`
- `rs1`: `i64`

*Defined at line 1315*

---

## rv_fence

```fides
pub fn rv_fence(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 1318*

---

## rv_fcvt_sd

```fides
pub fn rv_fcvt_sd(e: &mut NatEnc, fd: i64, fs: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `fs`: `i64`

*Defined at line 1321*

---

## nat_rvc_of

```fides
pub fn nat_rvc_of(w0: i64) -> i64 {
```

**Parameters:**

- `w0`: `i64`

**Returns:** `i64`

*Defined at line 1363*

---

## nat_rv_b_bits

```fides
pub fn nat_rv_b_bits(off: i64) -> i64 {
```

**Parameters:**

- `off`: `i64`

**Returns:** `i64`

*Defined at line 1631*

---

## nat_rv_j_bits

```fides
pub fn nat_rv_j_bits(off: i64) -> i64 {
```

**Parameters:**

- `off`: `i64`

**Returns:** `i64`

*Defined at line 1635*

---

## nat_rv_b_off_of

```fides
pub fn nat_rv_b_off_of(w: i64) -> i64 {
```

**Parameters:**

- `w`: `i64`

**Returns:** `i64`

*Defined at line 1640*

---

## nat_rv_j_off_of

```fides
pub fn nat_rv_j_off_of(w: i64) -> i64 {
```

**Parameters:**

- `w`: `i64`

**Returns:** `i64`

*Defined at line 1649*

---

## nat_rvc_expand

```fides
pub fn nat_rvc_expand(h0: i64) -> i64 {
```

**Parameters:**

- `h0`: `i64`

**Returns:** `i64`

*Defined at line 1663*

---

## NatRvcMap

TG9: where nat_rvc_compress moved each old 4-byte unit of the code:
`units` holds the new offset of old offset 4k at k, and the code's new
end at the last index; offsets from `old_end` on move by
new_end - old_end. Counts: the instructions looked at, those made 2
bytes, the 2-byte paddings put before data.

```fides
pub struct NatRvcMap {
```

**Fields:**

| Name | Type |
|------|------|
| `units` | `Vec<i64>` |
| `old_end` | `i64` |
| `new_end` | `i64` |
| `n_insn` | `i64` |
| `n_short` | `i64` |
| `n_pad` | `i64` |

*Defined at line 1850*

---

## NatRvcMap_new

```fides
pub fn NatRvcMap_new() -> NatRvcMap {
```

**Returns:** `NatRvcMap`

*Defined at line 1859*

---

## nat_rvc_pos

TG9: the new offset of old image offset `pos` (negative: unchanged).

```fides
pub fn nat_rvc_pos(m: &NatRvcMap, pos: i64) -> i64 {
```

**Parameters:**

- `m`: `&NatRvcMap`
- `pos`: `i64`

**Returns:** `i64`

*Defined at line 1866*

---

## nat_rvc_compress

```fides
pub fn nat_rvc_compress(e: &mut NatEnc, end: i64, starts: &Vec<i64>, op_lo: i64, op_hi: i64, m: &mut NatRvcMap) -> str {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `end`: `i64`
- `starts`: `&Vec<i64>`
- `op_lo`: `i64`
- `op_hi`: `i64`
- `m`: `&mut NatRvcMap`

**Returns:** `str`

*Defined at line 1903*

---

## nat_rvc_unit_check

```fides
pub fn nat_rvc_unit_check() -> str {
```

**Returns:** `str`

*Defined at line 2267*

---

## nat_elf_page

TG14: the page size the split of code and data is aligned to: 64 KB on
AArch64 (kernels with 64 KB pages), 4 KB on x86-64 and RISC-V.

```fides
pub fn nat_elf_page(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 2679*

---

## nat_elf_image_base

The link address of image offset 0 in the ELF: NAT_IMAGE_BASE, or 0x1000
for a position-independent executable.

```fides
pub fn nat_elf_image_base() -> i64 {
```

**Returns:** `i64`

*Defined at line 2688*

---

## NAT_ELF_NOTE_AT:

The file offset of the identification note: in the header page, which
the first PT_LOAD maps (R), at its end, so the room after the program
headers stays free for nat_elf_eh_frame's PT_GNU_EH_FRAME.

```fides
pub const NAT_ELF_NOTE_AT: i64 = 0xF00;
```

*Defined at line 2725*

---

## nat_elf_osabi

EI_OSABI: ELFOSABI_FREEBSD (9) for FreeBSD (its kernel brands a static
executable by it or by the note); System V (0) otherwise: OpenBSD's own
linker writes 0 there and its kernel reads the note, Android's are 0.

```fides
pub fn nat_elf_osabi() -> i64 {
```

**Returns:** `i64`

*Defined at line 2730*

---

## nat_elf_note_name

The note's owner ("" for a Linux image: no note).

```fides
pub fn nat_elf_note_name() -> str {
```

**Returns:** `str`

*Defined at line 2738*

---

## nat_elf_note_section

The section the note is in (as each system's crt objects name it).

```fides
pub fn nat_elf_note_section() -> str {
```

**Returns:** `str`

*Defined at line 2752*

---

## nat_elf_note

The identification note (empty for Linux): namesz, descsz 4, type 1
(FreeBSD's NT_FREEBSD_ABI_TAG, OpenBSD's ELF_NOTE_TYPE_OSVERSION,
Android's NT_TYPE_IDENT), the owner NUL-terminated and padded to 4, the
4-byte word (g_nat_elf_os_ver; 0 on OpenBSD).

```fides
pub fn nat_elf_note() -> Vec<i64> {
```

**Returns:** `Vec<i64>`

*Defined at line 2766*

---

## nat_elf

A static Linux executable of `image`, entered at image offset `entry`
(CI2: dynamically linked when g_nat_ldyn_dyn is set: PT_INTERP and
PT_DYNAMIC around the one PT_LOAD).

```fides
pub fn nat_elf(arch: i64, image: Vec<i64>, entry: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 2790*

---

## nat_elf_machine

e_machine of the machine layer's targets.

```fides
pub fn nat_elf_machine(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 2811*

---

## nat_elf_flags

e_flags: RISC-V's double-float ABI, else 0. TG9: with EF_RISCV_RVC (1)
when the code holds 2-byte instructions (g_nat_rvc).

```fides
pub fn nat_elf_flags(arch: i64) -> i64 {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `i64`

*Defined at line 2826*

---

## nat_elf_so_machine

CB9: the e_machine of a 64-bit little-endian ELF shared object (or
executable), -1 when `so` is not one (a linker script such as libc.so).

```fides
pub fn nat_elf_so_machine(so: &Vec<i64>) -> i64 {
```

**Parameters:**

- `so`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 2877*

---

## nat_elf_dynsym_defines

CB9: 1 when the ELF shared object `so` defines `sym` in its dynamic
symbol table (a global or weak symbol with a section), 0 when it does
not, -1 when it cannot tell (not a 64-bit little-endian ELF, no section
headers or no .dynsym).

```fides
pub fn nat_elf_dynsym_defines(so: &Vec<i64>, sym: str) -> i64 {
```

**Parameters:**

- `so`: `&Vec<i64>`
- `sym`: `str`

**Returns:** `i64`

*Defined at line 2892*

---

## nat_elf_find_lib

CB9: the path of `link "<lib>"`'s shared object for `arch` on this
machine (an ELF of that machine), "" when there is none.

```fides
pub fn nat_elf_find_lib(lib: str, arch: i64) -> str {
```

**Parameters:**

- `lib`: `str`
- `arch`: `i64`

**Returns:** `str`

*Defined at line 3043*

---

## nat_elf_link_missing

CB9: the C functions of `names` (C names) that no library of `libs`
(each entry the `link` library of a declaration, "" the C library;
repeats are fine) defines, as far as this machine can tell: a name is
listed only when every one of those libraries (and the C library) was
found and read, and none defines it. Nothing is listed when a library
is missing (the dynamic linker reports that by name) or unreadable.

```fides
pub fn nat_elf_link_missing(arch: i64, names: &Vec<str>, libs: &Vec<str>) -> Vec<str> {
```

**Parameters:**

- `arch`: `i64`
- `names`: `&Vec<str>`
- `libs`: `&Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 3074*

---

## nat_elf_img

nat_elf's executable: the header, the program headers (PT_INTERP,
PT_LOAD of the headers and code, or of everything; TG14 the data's
PT_LOAD and PT_GNU_STACK; PT_DYNAMIC), the image at file offset 0x1000.

```fides
pub fn nat_elf_img(arch: i64, image: &Vec<i64>, entry: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `&Vec<i64>`
- `entry`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3138*

---

## nat_pe_dll_for

TG15: the DLL a `--nat` Windows executable imports a C function of
`extern "C" link "<lib>"` from (as pe_ir's pe_link_dll): the classic C
runtime msvcrt.dll for the C library ("", "c", "msvcrt" and the Unix
names Windows has no file for: "pthread", "dl", "rt"); the Universal C
Runtime ucrtbase.dll for libm ("m": C99's hypot, cbrt, fmax, ... that
msvcrt.dll lacks; every Windows 10 and 11 has it in System32); "GL" and
"GLU" as pe_ir; a name ending in ".dll" as it is; anything else
"<lib>.dll" ("kernel32", "user32", "sqlite3", ...).

```fides
pub fn nat_pe_dll_for(lib: str) -> str {
```

**Parameters:**

- `lib`: `str`

**Returns:** `str`

*Defined at line 3253*

---

## nat_pe_export_set

CB9 (2g.106): the names the export directory of the PE image (a DLL)
`b` holds, as a set (name -> 1); 0 when `b` is not a PE image with
one.

```fides
pub fn nat_pe_export_set(b: &Vec<i64>) -> i64 {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 3364*

---

## nat_pe_find_dll

CB9 (2g.106): where the DLL `dll` (nat_pe_dll_for's name) is on this
machine: in a directory of FID_NAT_LIBPATH (separated by ';'), else the
Windows system folder (%SystemRoot%\System32, which exists only on a
Windows build machine); "" when it is in none.

```fides
pub fn nat_pe_find_dll(dll: str) -> str {
```

**Parameters:**

- `dll`: `str`

**Returns:** `str`

*Defined at line 3418*

---

## nat_pe_link_missing

CB9 (2g.106): the C functions of `names` that the DLL each one is
imported from (nat_pe_dll_for of its entry of `libs`) does not export,
as far as this machine can tell: a name is listed only when its DLL was
found and read (nat_pe_find_dll) and its export directory lacks it. A
missing DLL lists nothing (the loader reports it by name), as on Linux
(nat_elf_link_missing).

```fides
pub fn nat_pe_link_missing(names: &Vec<str>, libs: &Vec<str>) -> Vec<str> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `libs`: `&Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 3461*

---

## nat_pe_sig_plain

TG15: a stub signature (ng_ldyn_sig's form) without the variadic
marker: in "i.id>w" the arguments after the `.` are a C function's
`...` ones (the first fixed).

```fides
pub fn nat_pe_sig_plain(sig: str) -> str {
```

**Parameters:**

- `sig`: `str`

**Returns:** `str`

*Defined at line 3495*

---

## nat_pe_win64_stub

TG15: the body of a stub (its label already placed) that calls the C
function whose import address table entry is `slot` (the 8-byte word
the Windows loader fills) from the machine layer's convention:
arguments 0..3 in rdi, rsi, rdx, rcx, the rest above the return address
([rsp + 8 + 8 * (j - 4)] at entry), every float as its bits in a general
register (an f64 its 64 bits, an f32 its 32-bit pattern: MI25), the
result back in rdi. `sig` is ng_ldyn_sig's ("id>w": a class per
argument, `i` a word, `d` f64, `s` f32; `>`; the result's: `d`, `s`, `w`
i32, `W` u32, `h` i16, `H` u16, `b` i8, `B` u8 or bool, `v` none, `i` a
word).
Windows x64 passes by position: argument j < 4 in rcx, rdx, r8, r9 (an
integer) or xmm j (a float), the rest at [rsp + 32 + 8 * (j - 4)] above
32 bytes of shadow space, rsp 16-aligned at the call. A float among the
first four also goes to its integer register: a variadic callee
(printf) reads it there, the others ignore it. The registers are set
from the last position to the first, so each source (rcx for position
3, rdx for 2) is read before a lower position overwrites it. Windows
keeps rbx, rbp, rdi, rsi, r12-r15 and xmm6-xmm15; the stub changes only
what a call in the machine layer's convention may change.

```fides
pub fn nat_pe_win64_stub(e: &mut NatEnc, full_sig: str, slot: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `full_sig`: `str`
- `slot`: `i64`

*Defined at line 3552*

---

## nat_pe_img

nat_pe's executable (see above): DOS stub, COFF header, PE32+ optional
header (imports and IAT in the directories), .text and .data, the image
at file offset 0x200.

```fides
pub fn nat_pe_img(image: &Vec<i64>, entry: i64, data_off: i64, imp_off: i64, imp_size: i64, iat_off: i64, iat_size: i64) -> Vec<i64> {
```

**Parameters:**

- `image`: `&Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `imp_off`: `i64`
- `imp_size`: `i64`
- `iat_off`: `i64`
- `iat_size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3629*

---

## nat_pe

```fides
pub fn nat_pe(image: Vec<i64>, entry: i64, data_off: i64, imp_off: i64, imp_size: i64, iat_off: i64, iat_size: i64) -> Vec<i64> {
```

**Parameters:**

- `image`: `Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `imp_off`: `i64`
- `imp_size`: `i64`
- `iat_off`: `i64`
- `iat_size`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 3659*

---

## nat_pe_symbols

DB10 for `--nat` (as pe_ir_enc.fd's pe_append_coff_symbols does for
pe_ir's images, DB3): a COFF symbol table after nat_pe's sections, one
function symbol (section 1, .text; type 0x20, EXTERNAL) per name at its
image offset (.text starts the image), then the string table; the COFF
header's PointerToSymbolTable and NumberOfSymbols (file offsets 76, 80)
point at it. The loader reads none of it; llvm-nm, llvm-objdump,
dumpbin and gdb name our functions from it.

```fides
pub fn nat_pe_symbols(pe: &mut Vec<i64>, names: Vec<str>, starts: Vec<i64>) {
```

**Parameters:**

- `pe`: `&mut Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`

*Defined at line 3672*

---

## nat_pe_unwind_pdata

DB6 for `--nat`: where nat_pe_unwind's .pdata starts when its bytes go
after an image of `len` bytes (4-aligned, after the 8 bytes of .xdata).

```fides
pub fn nat_pe_unwind_pdata(len: i64) -> i64 {
```

**Parameters:**

- `len`: `i64`

**Returns:** `i64`

*Defined at line 3685*

---

## nat_pe_unwind

DB6 for `--nat`: the Windows x64 unwind data of the functions [start,
end) (image offsets, in address order; .text is at RVA 0x1000), as the
bytes to append to the image (they land in .data): padding to 4, one
UNWIND_INFO all framed functions share, then a RUNTIME_FUNCTION (begin,
end, unwind info RVAs) per framed function. Our prologue (nat_ir.fd
ng_prologue) is `push rbp; mov rbp, rsp` then the frame's allocation:
UNWIND_INFO version 1, prologue 4 bytes, FrameRegister rbp with offset
0, codes UWOP_SET_FPREG at 4 and UWOP_PUSH_NONVOL rbp at 1. From the
body the unwinder takes rsp = rbp (so the allocation and any later
change of rsp need no code), pops rbp and returns. An epilogue (`mov
rsp, rbp; pop rbp; ret`) unwinds the same at its first instruction and
is recognized from `pop rbp` on. A function without our frame record (a
WP8 leaf, nat_has_frame) gets no entry: Windows unwinds it as a leaf
(the return address at rsp), which is what it is. The callee-saved
registers the body stores below rbp have no UWOP_SAVE_NONVOL (its
offsets cannot be negative from the frame register): an unwinder gets
the stack and the frame pointers right, not their values in callers.

```fides
pub fn nat_pe_unwind(image: &Vec<i64>, starts: Vec<i64>, ends: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `image`: `&Vec<i64>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 3706*

---

## nat_pe_set_dir

Data directory `idx` of a PE32+ image nat_pe wrote (e_lfanew 0x40: the
directories at 0x40 + 4 + 20 + 112).

```fides
pub fn nat_pe_set_dir(pe: &mut Vec<i64>, idx: i64, rva: i64, size: i64) {
```

**Parameters:**

- `pe`: `&mut Vec<i64>`
- `idx`: `i64`
- `rva`: `i64`
- `size`: `i64`

*Defined at line 3743*

---

## NAT_PE_CODE_VA:

CT6: where code offset 0 of a nat_pe image is in memory (ImageBase
0x400000, .text at RVA 0x1000).

```fides
pub const NAT_PE_CODE_VA: i64 = 0x401000;
```

*Defined at line 3760*

---

## nat_pe_add_sections

CT6: `pe` (nat_pe's image: two sections, 0x200 bytes of headers) with
the sections `snames` / `parts` (none empty) after its last one: the
headers grow to 0x400 bytes (.text and .data move 0x200 bytes down the
file, their addresses do not change), each new section gets the next
page-aligned address and 0x200-aligned file space, flags
INITIALIZED_DATA | MEM_DISCARDABLE | MEM_READ, the name "/<n>" where n
counts from 4 over the names (the COFF string table nat_pe_symbols_pre
writes); NumberOfSections, SizeOfImage and SizeOfInitializedData cover
them. Another image, or more than 15 sections, comes back unchanged.

```fides
pub fn nat_pe_add_sections(pe: Vec<i64>, snames: Vec<str>, parts: Vec<Vec<i64>>) -> Vec<i64> {
```

**Parameters:**

- `pe`: `Vec<i64>`
- `snames`: `Vec<str>`
- `parts`: `Vec<Vec<i64>>`

**Returns:** `Vec<i64>`

*Defined at line 3775*

---

## nat_pe_symbols_pre

CT6: nat_pe_symbols with the COFF string table starting with the names
`pre` (the long section names of nat_pe_add_sections, at 4, 4 + len + 1,
...); written even without function names, for the sections' sake.

```fides
pub fn nat_pe_symbols_pre(pe: &mut Vec<i64>, pre: Vec<str>, names: Vec<str>, starts: Vec<i64>) {
```

**Parameters:**

- `pe`: `&mut Vec<i64>`
- `pre`: `Vec<str>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`

*Defined at line 3831*

---

## nat_pe_debug

CT6: the `--nat` PE `pe` (nat_pe) of `image` with its DWARF: the
functions `names` at [starts, ends) (image offsets), their line rows
`lrows` (against `file` and g_nat_line_files), their frame rows `cfi`
(DB6) and the variables, types and shown names of g_nat_dbg_vars,
g_nat_dbg_types and g_nat_dbg_shown, as .debug_abbrev, .debug_info,
.debug_line, .debug_frame and .debug_loc sections (nat_pe_add_sections),
then the COFF symbol table (DB10) whose string table names them. An
image nat_pe_add_sections does not take gets the symbols only.

```fides
pub fn nat_pe_debug(pe: Vec<i64>, image: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, cfi: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `pe`: `Vec<i64>`
- `image`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `cfi`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 3854*

---

## nat_debug_line_at

TL5: the same with image offset 0 at address `abase` (a Mach-O image's
code starts at NAT_MACHO_BASE + NAT_MACHO_HDR, nat_macho_dwarf).

```fides
pub fn nat_debug_line_at(file: str, rows: Vec<i64>, abase: i64) -> Vec<i64> {
```

**Parameters:**

- `file`: `str`
- `rows`: `Vec<i64>`
- `abase`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 4161*

---

## NatDeclSrc

DB2: the source files read for one image's declaration lines, with
where each line starts: line l (from 1) of file f begins at offset
`starts[first[f] + l - 1]` of `texts[f]`, which has `count[f]` lines
(0: the file could not be read).

```fides
pub struct NatDeclSrc {
```

**Fields:**

| Name | Type |
|------|------|
| `paths` | `Vec<str>` |
| `texts` | `Vec<str>` |
| `first` | `Vec<i64>` |
| `count` | `Vec<i64>` |
| `starts` | `Vec<i64>` |

*Defined at line 4533*

---

## NatDeclSrc_new

```fides
pub fn NatDeclSrc_new() -> NatDeclSrc {
```

**Returns:** `NatDeclSrc`

*Defined at line 4541*

---

## nat_dbg_decl_on

DB2: false when `FID_NAT_OFF` names `declline`.

```fides
pub fn nat_dbg_decl_on() -> bool {
```

**Returns:** `bool`

*Defined at line 4548*

---

## nat_dbg_decl_line

DB2: the declaration line of function `name` (`shown` as
nat_decl_names takes it) whose first line row is `line` of file
`path`: the nearest line at or above it, at most 400 lines up, that
holds `fn <one of its names>` before any `//`. `line` itself when there
is none, the name is a generated one (`__...`), the file cannot be read
or `FID_NAT_OFF=declline`.

```fides
pub fn nat_dbg_decl_line(src: &mut NatDeclSrc, path: str, line: i64, name: str, shown: str) -> i64 {
```

**Parameters:**

- `src`: `&mut NatDeclSrc`
- `path`: `str`
- `line`: `i64`
- `name`: `str`
- `shown`: `str`

**Returns:** `i64`

*Defined at line 4688*

---

## nat_dbg_decl_of

DB2 (2g.102): the declaration of function `name` whose code is
[start, end) of the image, from its line rows (`lrows`, triples of
image position, line, 2 * file index + end flag, in image order; the
scan starts at row `from`): [file (from 1), line]. The first row whose
line has the function's `fn` line at or above it in its file
(nat_dbg_decl_find) gives both: a function that begins with inlined
code has first rows of the callee's lines (in its file, maybe below the
callee's own `fn` and above the caller's: main got scale's line 56,
not its own 76), so a row the scan finds nothing for is passed over, up
to 8 of them. Else the first row's file and line, as before.

```fides
pub fn nat_dbg_decl_of(src: &mut NatDeclSrc, file: str, lrows: &Vec<i64>, from: i64, end: i64, name: str, shown: str) -> Vec<i64> {
```

**Parameters:**

- `src`: `&mut NatDeclSrc`
- `file`: `str`
- `lrows`: `&Vec<i64>`
- `from`: `i64`
- `end`: `i64`
- `name`: `str`
- `shown`: `str`

**Returns:** `Vec<i64>`

*Defined at line 4706*

---

## nat_dbg_set_full_paths

Set by main.fd for each native build (`--debug` or `-g`).

```fides
pub fn nat_dbg_set_full_paths(on: bool) {
```

**Parameters:**

- `on`: `bool`

*Defined at line 4817*

---

## nat_dbg_written_path

RL13 R2: a file name as the debug info writes it (.debug_line's file
table, the compile unit's name). Two hosts that build the same program
by absolute path must give the same image, so unless
g_nat_dbg_full_paths: FID_DEBUG_PREFIX_MAP=<old>=<new> replaces a
leading <old>; else a file in the input file's directory (or below)
is written relative to it (the input file is its name alone), a stdlib
module from its `stdlib/` on, any other file by its name alone. The
files are still read by their real paths (nat_decl_path).

```fides
pub fn nat_dbg_written_path(p: str) -> str {
```

**Parameters:**

- `p`: `str`

**Returns:** `str`

*Defined at line 4844*

---

## nat_dbg_scope_end

DB4: the line of the `}` that closes the block holding line `line` of
the source file `path` (a declaration's line: where its scope ends),
found by scanning forward from the start of that line and counting
braces, past string and character literals and `//` comments; 0 when
the file cannot be read, the line is past its end, or no such brace
comes within 20000 lines.

```fides
pub fn nat_dbg_scope_end(path: str, line: i64) -> i64 {
```

**Parameters:**

- `path`: `str`
- `line`: `i64`

**Returns:** `i64`

*Defined at line 4932*

---

## nat_elf_debug

```fides
pub fn nat_elf_debug(arch: i64, image: Vec<i64>, entry: i64, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, cfi: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `cfi`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 5603*

---

## NatDwarf

CT6: the DWARF of a native image that an ELF (nat_elf_debug) and a
`--nat` PE (nat_pe_debug) share: .debug_abbrev, .debug_info (the compile
unit, its types, a subprogram per function with its parameters and
variables), .debug_frame (a CIE, an FDE per function) and .debug_loc
(the location lists of values held in registers; empty when `lists` is
off or nothing needs one).

```fides
pub struct NatDwarf {
```

**Fields:**

| Name | Type |
|------|------|
| `abbrev` | `Vec<i64>` |
| `info` | `Vec<i64>` |
| `frame` | `Vec<i64>` |
| `dloc` | `Vec<i64>` |

*Defined at line 5638*

---

## nat_dwarf_build

CT6: the DWARF sections of `image` (code offset 0 at address `base`)
for nat_elf_debug and nat_pe_debug: the functions `names` at [starts,
ends), the line rows `lrows` (their declarations), the frame rows `cfi`
(DB6), the variables of g_nat_dbg_vars (DB4) with location lists when
`lists` (.debug_loc), the types and names of g_nat_dbg_types /
g_nat_dbg_shown (CT9).

```fides
pub fn nat_dwarf_build(arch: i64, image: Vec<i64>, base: i64, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, cfi: Vec<i64>, lists: bool) -> NatDwarf {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `base`: `i64`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `cfi`: `Vec<i64>`
- `lists`: `bool`

**Returns:** `NatDwarf`

*Defined at line 5651*

---

## nat_eh_frame_on

DB6: false when `FID_NAT_OFF` names `ehframe` (the image as before:
no .eh_frame, no PT_GNU_EH_FRAME).

```fides
pub fn nat_eh_frame_on() -> bool {
```

**Returns:** `bool`

*Defined at line 6118*

---

## nat_eh_frame

DB6: .eh_frame from nat_dwarf_build's .debug_frame `frame` (a version 1
CIE with no augmentation at 0, then FDEs [length, CIE 0, start (8
bytes), length (8), rules]), placed at address `eh_addr`: the CIE with
augmentation "zR" and FDE addresses pc-relative in 4 bytes
(DW_EH_PE_pcrel | sdata4), each FDE with the same rules, entries
8-aligned with DW_CFA_nop, then a zero terminator. `fdes` gets [start
address, offset in .eh_frame] for each FDE. Empty when `frame` is not
that shape.

```fides
pub fn nat_eh_frame(frame: &Vec<i64>, eh_addr: i64, fdes: &mut Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `frame`: `&Vec<i64>`
- `eh_addr`: `i64`
- `fdes`: `&mut Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 6162*

---

## nat_eh_frame_hdr

DB6: .eh_frame_hdr at address `hdr_addr` for the .eh_frame at
`eh_addr` whose FDEs nat_eh_frame listed in `fdes` ([start, offset]):
version 1, .eh_frame's address pc-relative, the FDE count, and the
binary search table [start, FDE address] relative to the header,
sorted by start (one entry per start).

```fides
pub fn nat_eh_frame_hdr(hdr_addr: i64, eh_addr: i64, fdes: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `hdr_addr`: `i64`
- `eh_addr`: `i64`
- `fdes`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 6253*

---

## nat_elf_eh_frame

DB6: the executable `elf` (nat_elf's: the image of `image_len` bytes at
file offset 0x1000, ending the file) with .eh_frame_hdr and .eh_frame
(from the .debug_frame `frame`) after the image, 8-aligned: the PT_LOAD
that ends at the image's end (the data, or the one RWX segment) grows
over them, so they are mapped at addresses that keep their file
offsets' distance, and a PT_GNU_EH_FRAME names the header for
unwinders (libgcc, libunwind, perf, gdb). `eh` gets [hdr offset,
length, address, .eh_frame offset, length, address]. Unchanged (and
`eh` empty) when the program headers leave no room or no PT_LOAD ends
at the image's end.

```fides
pub fn nat_elf_eh_frame(elf: Vec<i64>, image_len: i64, frame: &Vec<i64>, eh: &mut Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `elf`: `Vec<i64>`
- `image_len`: `i64`
- `frame`: `&Vec<i64>`
- `eh`: `&mut Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 6344*

---

## NAT_X64:

```fides
pub const NAT_X64: i64 = 3;
```

*Defined at line 6429*

---

## NATFIX_X64_REL32:

```fides
pub const NATFIX_X64_REL32: i64 = 7;
```

*Defined at line 6430*

---

## NATFIX_RVA32:

```fides
pub const NATFIX_RVA32: i64 = 8;  // 4 data bytes: the label's PE RVA
```

*Defined at line 6431*

---

## X64_O:

```fides
pub const X64_O: i64 = 0;
```

*Defined at line 6434*

---

## X64_B:

```fides
pub const X64_B: i64 = 2;
```

*Defined at line 6435*

---

## X64_AE:

```fides
pub const X64_AE: i64 = 3;
```

*Defined at line 6436*

---

## X64_E:

```fides
pub const X64_E: i64 = 4;
```

*Defined at line 6437*

---

## X64_NE:

```fides
pub const X64_NE: i64 = 5;
```

*Defined at line 6438*

---

## X64_BE:

```fides
pub const X64_BE: i64 = 6;
```

*Defined at line 6439*

---

## X64_A:

```fides
pub const X64_A: i64 = 7;
```

*Defined at line 6440*

---

## X64_S:

```fides
pub const X64_S: i64 = 8;
```

*Defined at line 6441*

---

## X64_P:

```fides
pub const X64_P: i64 = 10;
```

*Defined at line 6442*

---

## X64_NP:

```fides
pub const X64_NP: i64 = 11;
```

*Defined at line 6443*

---

## X64_L:

```fides
pub const X64_L: i64 = 12;
```

*Defined at line 6444*

---

## X64_GE:

```fides
pub const X64_GE: i64 = 13;
```

*Defined at line 6445*

---

## X64_LE:

```fides
pub const X64_LE: i64 = 14;
```

*Defined at line 6446*

---

## X64_G:

```fides
pub const X64_G: i64 = 15;
```

*Defined at line 6447*

---

## x64_rex

A REX prefix when one is needed (or `force`: byte access to spl..dil).

```fides
pub fn x64_rex(e: &mut NatEnc, w: bool, reg: i64, index: i64, base: i64, force: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `w`: `bool`
- `reg`: `i64`
- `index`: `i64`
- `base`: `i64`
- `force`: `bool`

*Defined at line 6449*

---

## x64_modrm_rr

```fides
pub fn x64_modrm_rr(e: &mut NatEnc, reg: i64, rm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `reg`: `i64`
- `rm`: `i64`

*Defined at line 6468*

---

## NatEnc_u32

```fides
pub fn NatEnc_u32(e: &mut NatEnc, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `v`: `i64`

*Defined at line 6497*

---

## x64_mov

```fides
pub fn x64_mov(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6510*

---

## x64_mov32

mov r32, r32: the low 32 bits, zero-extended

```fides
pub fn x64_mov32(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6517*

---

## x64_li

```fides
pub fn x64_li(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 6524*

---

## x64_li_zero

xor r32, r32 (d = 0; flags written)

```fides
pub fn x64_li_zero(e: &mut NatEnc, d: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`

*Defined at line 6543*

---

## x64_li_u32

```fides
pub fn x64_li_u32(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 6551*

---

## x64_li_s32

```fides
pub fn x64_li_s32(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 6559*

---

## x64_li_abs

```fides
pub fn x64_li_abs(e: &mut NatEnc, d: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `v`: `i64`

*Defined at line 6568*

---

## x64_alu

d = d op s for op 0x01 add, 0x09 or, 0x21 and, 0x29 sub, 0x31 xor,
0x39 cmp (flags only)

```fides
pub fn x64_alu(e: &mut NatEnc, op: i64, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6576*

---

## x64_imul

```fides
pub fn x64_imul(e: &mut NatEnc, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6582*

---

## x64_alu_mem

TG7: d = d op *(base + disp), a memory source operand: REX.W op /r
with op 0x03 add, 0x0B or, 0x23 and, 0x2B sub, 0x33 xor, 0x3B cmp
(flags only), or 0x0FAF imul. `add rax, [rsp+16]` = 48 03 44 24 10;
`imul r9, [rsp+8]` = 4C 0F AF 4C 24 08.

```fides
pub fn x64_alu_mem(e: &mut NatEnc, op: i64, d: i64, base: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `d`: `i64`
- `base`: `i64`
- `disp`: `i64`

*Defined at line 6593*

---

## x64_cmp_mem_imm

OP18: the flags of `size` bytes at base + disp minus an immediate
(`cmp [mem], imm`): 80 /7 ib (byte), 66 83 /7 ib or 66 81 /7 iw
(word), 83 /7 ib or 81 /7 id (dword), REX.W 83 /7 ib or REX.W 81 /7 id
(qword: the immediate sign-extended, so 32 bits signed). The short ib
form when the immediate fits 8 bits signed. `cmp qword [rsp+8], 0` =
48 83 7C 24 08 00; `cmp byte [rdi+16], 1` = 80 7F 10 01.

```fides
pub fn x64_cmp_mem_imm(e: &mut NatEnc, size: i64, base: i64, disp: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `size`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `imm`: `i64`

*Defined at line 6610*

---

## x64_alu_to_mem

OP18: *(base + disp) = *(base + disp) op s, 64 bits (a read-modify-write
destination): REX.W op /r with op 0x01 add, 0x09 or, 0x21 and, 0x29
sub, 0x31 xor. `add [rdi+8], rax` = 48 01 47 08; `or [rsp+16], r9` =
4C 09 4C 24 10.

```fides
pub fn x64_alu_to_mem(e: &mut NatEnc, op: i64, s: i64, base: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `s`: `i64`
- `base`: `i64`
- `disp`: `i64`

*Defined at line 6656*

---

## x64_alu_mem_imm

OP18: *(base + disp) op= imm, 64 bits, the immediate sign-extended from
8 bits (REX.W 83 /ext ib) or 32 bits (REX.W 81 /ext id); ext 0 add,
1 or, 4 and, 5 sub, 6 xor. `or qword [rdi+8], 4` = 48 83 4F 08 04.

```fides
pub fn x64_alu_mem_imm(e: &mut NatEnc, ext: i64, base: i64, disp: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `ext`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `imm`: `i64`

*Defined at line 6665*

---

## x64_f7

F7 /ext: 2 not, 3 neg, 4 mul, 5 imul, 6 div, 7 idiv

```fides
pub fn x64_f7(e: &mut NatEnc, ext: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `ext`: `i64`
- `r`: `i64`

*Defined at line 6679*

---

## x64_shift_cl

D3 /ext: shift by cl (4 shl, 5 shr, 7 sar)

```fides
pub fn x64_shift_cl(e: &mut NatEnc, ext: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `ext`: `i64`
- `r`: `i64`

*Defined at line 6686*

---

## x64_shiftx

TG7 (BMI2): d = a shifted by register b (count masked to 6 bits, as
by CL), flags untouched, no CL needed: VEX.LZ.pp.0F38.W1 F7 /r with
ModRM.reg = d, ModRM.rm = a, VEX.vvvv = b. pp 1 SHLX (66), 2 SARX (F3),
3 SHRX (F2). `shlx rax, rbx, rcx` = C4 E2 F1 F7 C3.

```fides
pub fn x64_shiftx(e: &mut NatEnc, pp: i64, d: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `pp`: `i64`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 6696*

---

## x64_andn

OP20/MI3 (BMI1): d = ~s & o, flags as AND: VEX.LZ.0F38.W1 F2 /r with
ModRM.reg = d, VEX.vvvv = s, ModRM.rm = o. `andn rax, rbx, rcx` =
C4 E2 E0 F2 C1.

```fides
pub fn x64_andn(e: &mut NatEnc, d: i64, s: i64, o: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`
- `o`: `i64`

*Defined at line 6707*

---

## x64_blsr

OP20/MI3 (BMI1): d = a & (a - 1), the lowest set bit cleared:
VEX.LZ.0F38.W1 F3 /1 with VEX.vvvv = d, ModRM.rm = a. `blsr rax, rcx`
= C4 E2 F8 F3 C9.

```fides
pub fn x64_blsr(e: &mut NatEnc, d: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`

*Defined at line 6718*

---

## x64_blsmsk

TG7 (BMI1): d = a ^ (a - 1), the mask up to and including the lowest
set bit (all ones for a = 0): VEX.LZ.0F38.W1 F3 /2 with VEX.vvvv = d,
ModRM.rm = a. `blsmsk rax, rcx` = C4 E2 F8 F3 D1.

```fides
pub fn x64_blsmsk(e: &mut NatEnc, d: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`

*Defined at line 6729*

---

## x64_blsi

TG7 (BMI1): d = a & -a, the lowest set bit alone: VEX.LZ.0F38.W1 F3 /3
with VEX.vvvv = d, ModRM.rm = a. `blsi rax, rcx` = C4 E2 F8 F3 D9.

```fides
pub fn x64_blsi(e: &mut NatEnc, d: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`

*Defined at line 6739*

---

## x64_shift_imm

C1 /ext ib: shift by an immediate. n == 1 is D1 /ext, one byte shorter
and the canonical form an assembler gives `C1 /ext 1` back as, so our
output reassembles to itself (TG5 round trip).

```fides
pub fn x64_shift_imm(e: &mut NatEnc, ext: i64, r: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `ext`: `i64`
- `r`: `i64`
- `n`: `i64`

*Defined at line 6750*

---

## x64_cqo

```fides
pub fn x64_cqo(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 6762*

---

## x64_setcc

```fides
pub fn x64_setcc(e: &mut NatEnc, cc: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cc`: `i64`
- `r`: `i64`

*Defined at line 6767*

---

## x64_movx

Extensions to 64 bits: 0F B6 movzx8, 0F B7 movzx16, 0F BE movsx8,
0F BF movsx16 (REX.W), 63 movsxd

```fides
pub fn x64_movx(e: &mut NatEnc, op: i64, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6776*

---

## x64_test

```fides
pub fn x64_test(e: &mut NatEnc, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `a`: `i64`
- `b`: `i64`

*Defined at line 6785*

---

## x64_test_bit

TG7/TG8: set the flags by one bit of r, for a branch on it: bits 0..7
TEST r8, imm8 (F6 /0 ib; REX for spl..dil and r8..r15), bits 8..31
TEST r32, imm32 (F7 /0 id: the 32-bit form tests bit 31 without the
sign extension of imm32), then ZF clear when the bit is set (jne / je);
bits 32..63 BT r64, imm8 (REX.W 0F BA /4 ib), CF = the bit (jb / jae).
x64_bit_cc gives the condition that holds when the bit is set.
`test cl, 4` = F6 C1 04; `test ebx, 0x100` = F7 C3 00 01 00 00;
`bt rax, 40` = 48 0F BA E0 28.

```fides
pub fn x64_test_bit(e: &mut NatEnc, r: i64, bit: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `bit`: `i64`

*Defined at line 6799*

---

## x64_bit_cc

TG7/TG8: the condition after x64_test_bit(r, bit) that holds when the
bit is set.

```fides
pub fn x64_bit_cc(bit: i64) -> i64 {
```

**Parameters:**

- `bit`: `i64`

**Returns:** `i64`

*Defined at line 6838*

---

## x64_cmov

MI24: CMOVcc d, s (REX.W 0F 40+cc /r, ModRM.reg = d, ModRM.rm = s):
d = s when cc holds, else d stays. `cmovne rax, rbx` = 48 0F 45 C3.

```fides
pub fn x64_cmov(e: &mut NatEnc, cc: i64, d: i64, s: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cc`: `i64`
- `d`: `i64`
- `s`: `i64`

*Defined at line 6847*

---

## x64_load

d = *(base + disp), `size` bytes, extended to 64 bits

```fides
pub fn x64_load(e: &mut NatEnc, d: i64, base: i64, disp: i64, size: i64, signed: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `size`: `i64`
- `signed`: `bool`

*Defined at line 6855*

---

## x64_store

*(base + disp) = v, `size` bytes

```fides
pub fn x64_store(e: &mut NatEnc, v: i64, base: i64, disp: i64, size: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `v`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `size`: `i64`

*Defined at line 6886*

---

## x64_lea

```fides
pub fn x64_lea(e: &mut NatEnc, d: i64, base: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `base`: `i64`
- `disp`: `i64`

*Defined at line 6899*

---

## x64_lea_idx

lea d, [base + index * (1 << scale)] (scale 0..3), through a SIB byte.
`index` must not be rsp (4: "no index" in a SIB); a base of rbp / r13
takes mod 01 with a zero disp8 (mod 00 there means rip/disp32).

```fides
pub fn x64_lea_idx(e: &mut NatEnc, d: i64, base: i64, index: i64, scale: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `base`: `i64`
- `index`: `i64`
- `scale`: `i64`

*Defined at line 6908*

---

## x64_load_idx

MI22: d = *(base + index * 2^scale + disp), `size` bytes, extended to
64 bits (x64_load's opcodes with a SIB operand)

```fides
pub fn x64_load_idx(e: &mut NatEnc, d: i64, base: i64, index: i64, scale: i64, disp: i64, size: i64, signed: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `base`: `i64`
- `index`: `i64`
- `scale`: `i64`
- `disp`: `i64`
- `size`: `i64`
- `signed`: `bool`

*Defined at line 6944*

---

## x64_store_idx

MI22: *(base + index * 2^scale + disp) = v, `size` bytes

```fides
pub fn x64_store_idx(e: &mut NatEnc, v: i64, base: i64, index: i64, scale: i64, disp: i64, size: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `v`: `i64`
- `base`: `i64`
- `index`: `i64`
- `scale`: `i64`
- `disp`: `i64`
- `size`: `i64`

*Defined at line 6975*

---

## x64_lea_label

d = the address of `label` (rip-relative)

```fides
pub fn x64_lea_label(e: &mut NatEnc, d: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `label`: `i64`

*Defined at line 6989*

---

## x64_store_label

[rip + label] = r (8 bytes)

```fides
pub fn x64_store_label(e: &mut NatEnc, r: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `label`: `i64`

*Defined at line 6997*

---

## x64_load_label

r = [rip + label] (8 bytes)

```fides
pub fn x64_load_label(e: &mut NatEnc, r: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `label`: `i64`

*Defined at line 7005*

---

## x64_load_fs

r = fs:[disp]

```fides
pub fn x64_load_fs(e: &mut NatEnc, r: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `disp`: `i64`

*Defined at line 7013*

---

## x64_store_fs

fs:[disp] = r (2g.99: the runtime's thread block in the executable's
static TLS, nat_ir.fd nat_tls_set)

```fides
pub fn x64_store_fs(e: &mut NatEnc, r: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `disp`: `i64`

*Defined at line 7024*

---

## x64_load_gs

r = gs:[disp] (Windows: the thread environment block)

```fides
pub fn x64_load_gs(e: &mut NatEnc, r: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `disp`: `i64`

*Defined at line 7034*

---

## x64_store_gs

gs:[disp] = r

```fides
pub fn x64_store_gs(e: &mut NatEnc, r: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `disp`: `i64`

*Defined at line 7044*

---

## x64_align_sp16

and rsp, -16

```fides
pub fn x64_align_sp16(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7054*

---

## NatEnc_rva32

4 data bytes: the PE RVA of `label`

```fides
pub fn NatEnc_rva32(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 7062*

---

## x64_call

```fides
pub fn x64_call(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 7067*

---

## x64_jmp

```fides
pub fn x64_jmp(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 7071*

---

## x64_jcc

```fides
pub fn x64_jcc(e: &mut NatEnc, cc: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cc`: `i64`
- `label`: `i64`

*Defined at line 7075*

---

## x64_jcc8

Jcc by a fixed count of bytes after this 2-byte instruction

```fides
pub fn x64_jcc8(e: &mut NatEnc, cc: i64, rel: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cc`: `i64`
- `rel`: `i64`

*Defined at line 7082*

---

## x64_jmp_short

TG6: the short forms to a label (rel8, -128..127 from the instruction's
end; NatEnc_resolve checks the reach): `jmp rel8` (EB) and `jcc rel8`.

```fides
pub fn x64_jmp_short(e: &mut NatEnc, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `label`: `i64`

*Defined at line 7089*

---

## x64_jcc_short

```fides
pub fn x64_jcc_short(e: &mut NatEnc, cc: i64, label: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cc`: `i64`
- `label`: `i64`

*Defined at line 7094*

---

## x64_call_r

```fides
pub fn x64_call_r(e: &mut NatEnc, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`

*Defined at line 7100*

---

## x64_jmp_r

MI27: JMP r (FF /4), the indirect jump of a jump table: `jmp r10` =
41 FF E2.

```fides
pub fn x64_jmp_r(e: &mut NatEnc, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`

*Defined at line 7108*

---

## x64_ret

```fides
pub fn x64_ret(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7114*

---

## x64_push

```fides
pub fn x64_push(e: &mut NatEnc, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`

*Defined at line 7117*

---

## x64_pop

```fides
pub fn x64_pop(e: &mut NatEnc, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`

*Defined at line 7121*

---

## x64_ud2

```fides
pub fn x64_ud2(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7125*

---

## x64_syscall

```fides
pub fn x64_syscall(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7129*

---

## x64_mfence

```fides
pub fn x64_mfence(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7133*

---

## x64_add_imm

add / sub r, imm32 (81 /0, /5)

```fides
pub fn x64_add_imm(e: &mut NatEnc, r: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `v`: `i64`

*Defined at line 7140*

---

## x64_sub_imm

```fides
pub fn x64_sub_imm(e: &mut NatEnc, r: i64, v: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `v`: `i64`

*Defined at line 7157*

---

## NVK_ADD64:

```fides
pub const NVK_ADD64: i64 = 0;
```

*Defined at line 7183*

---

## NVK_SUB64:

```fides
pub const NVK_SUB64: i64 = 1;
```

*Defined at line 7184*

---

## NVK_ADD32:

```fides
pub const NVK_ADD32: i64 = 2;
```

*Defined at line 7185*

---

## NVK_SUB32:

```fides
pub const NVK_SUB32: i64 = 3;
```

*Defined at line 7186*

---

## NVK_MUL32:

```fides
pub const NVK_MUL32: i64 = 4;
```

*Defined at line 7187*

---

## NVK_AND:

```fides
pub const NVK_AND: i64 = 5;
```

*Defined at line 7188*

---

## NVK_OR:

```fides
pub const NVK_OR: i64 = 6;
```

*Defined at line 7189*

---

## NVK_XOR:

```fides
pub const NVK_XOR: i64 = 7;
```

*Defined at line 7190*

---

## NVK_FADD64:

```fides
pub const NVK_FADD64: i64 = 8;
```

*Defined at line 7191*

---

## NVK_FSUB64:

```fides
pub const NVK_FSUB64: i64 = 9;
```

*Defined at line 7192*

---

## NVK_FMUL64:

```fides
pub const NVK_FMUL64: i64 = 10;
```

*Defined at line 7193*

---

## NVK_FDIV64:

```fides
pub const NVK_FDIV64: i64 = 11;
```

*Defined at line 7194*

---

## NVK_LAST:

```fides
pub const NVK_LAST: i64 = 11;
```

*Defined at line 7195*

---

## nvk_commutes

The operation commutes (VEX operand order may be swapped).

```fides
pub fn nvk_commutes(kind: i64) -> bool {
```

**Parameters:**

- `kind`: `i64`

**Returns:** `bool`

*Defined at line 7198*

---

## x64_vec

SSE2: x1 = x1 op x2 on 128-bit xmm registers (`paddq xmm1, xmm2`).

```fides
pub fn x64_vec(e: &mut NatEnc, kind: i64, x1: i64, x2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `x1`: `i64`
- `x2`: `i64`

*Defined at line 7244*

---

## x64_vld

movdqu xmm x, [base + disp] (F3 0F 6F) and movdqu [base + disp], xmm x
(F3 0F 7F): unaligned 128-bit load and store.

```fides
pub fn x64_vld(e: &mut NatEnc, x: i64, base: i64, disp: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x`: `i64`
- `base`: `i64`
- `disp`: `i64`

*Defined at line 7258*

---

## x64_vst

```fides
pub fn x64_vst(e: &mut NatEnc, base: i64, disp: i64, x: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `disp`: `i64`
- `x`: `i64`

*Defined at line 7266*

---

## x64_pshufd

pshufd x1, x2, imm (66 0F 70 /r ib): the four dwords of x2 picked by
imm's two-bit fields (0x4E swaps the two quadwords).

```fides
pub fn x64_pshufd(e: &mut NatEnc, x1: i64, x2: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x1`: `i64`
- `x2`: `i64`
- `imm`: `i64`

*Defined at line 7276*

---

## x64_punpcklqdq

punpcklqdq x1, x2 (66 0F 6C): x1 = [x1.lo, x2.lo] (two i64 into a vector)

```fides
pub fn x64_punpcklqdq(e: &mut NatEnc, x1: i64, x2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x1`: `i64`
- `x2`: `i64`

*Defined at line 7286*

---

## x64_vvec

AVX / AVX2: d = a op b, three operands (`vpaddq ymm0, ymm1, ymm2`);
l256 the 256-bit ymm form (AVX2 for the integer kinds), else 128-bit
xmm. An operation that commutes takes b first when only b needs the
three-byte prefix (the shorter encoding assemblers choose).

```fides
pub fn x64_vvec(e: &mut NatEnc, kind: i64, d: i64, a: i64, b: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`
- `l256`: `bool`

*Defined at line 7325*

---

## x64_vvld

vmovdqu ymm/xmm x, [base + disp] (VEX F3 0F 6F) and the store (7F).

```fides
pub fn x64_vvld(e: &mut NatEnc, x: i64, base: i64, disp: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `l256`: `bool`

*Defined at line 7340*

---

## x64_vvst

```fides
pub fn x64_vvst(e: &mut NatEnc, base: i64, disp: i64, x: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `disp`: `i64`
- `x`: `i64`
- `l256`: `bool`

*Defined at line 7346*

---

## a64_vec

AArch64 NEON (128-bit, Q = 1): d = n op m, the arrangement from the
kind (.2d, .4s, .16b for the bitwise ones): ADD, SUB, MUL (.4s only),
AND, ORR, EOR, FADD, FSUB, FMUL, FDIV (.2d).

```fides
pub fn a64_vec(e: &mut NatEnc, kind: i64, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7355*

---

## a64_ldr_q

ldr q t, [n, #off] / str q t, [n, #off]: off a multiple of 16 below
65536 (the unsigned scaled offset).

```fides
pub fn a64_ldr_q(e: &mut NatEnc, t: i64, n: i64, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `off`: `i64`

*Defined at line 7385*

---

## a64_str_q

```fides
pub fn a64_str_q(e: &mut NatEnc, t: i64, n: i64, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `t`: `i64`
- `n`: `i64`
- `off`: `i64`

*Defined at line 7389*

---

## a64_dup_2d

dup v d.2d, x n: both lanes = the general register

```fides
pub fn a64_dup_2d(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 7394*

---

## a64_addp_d

addp d d, v n.2d: the sum of the two lanes (a reduction)

```fides
pub fn a64_addp_d(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 7399*

---

## a64_umov_d

umov x d, v n.d[i] (`mov x0, v1.d[1]`) and ins v d.d[i], x n

```fides
pub fn a64_umov_d(e: &mut NatEnc, d: i64, n: i64, i: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `i`: `i64`

*Defined at line 7404*

---

## a64_ins_d

```fides
pub fn a64_ins_d(e: &mut NatEnc, d: i64, i: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `i`: `i64`
- `n`: `i64`

*Defined at line 7408*

---

## rv_vsetvli

RISC-V V 1.0: vsetvli rd, rs1, vtype (vtype: vma << 7 | vta << 6 |
vsew << 3 | vlmul; e64, m1, ta, ma is 0xD8, e32 0xD0)

```fides
pub fn rv_vsetvli(e: &mut NatEnc, rd: i64, rs1: i64, vtype: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `rs1`: `i64`
- `vtype`: `i64`

*Defined at line 7414*

---

## rv_vle

vle<sew>.v vd, (rs1) and vse<sew>.v vs3, (rs1), unmasked unit stride

```fides
pub fn rv_vle(e: &mut NatEnc, vd: i64, rs1: i64, sew: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `rs1`: `i64`
- `sew`: `i64`

*Defined at line 7433*

---

## rv_vse

```fides
pub fn rv_vse(e: &mut NatEnc, vs3: i64, rs1: i64, sew: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vs3`: `i64`
- `rs1`: `i64`
- `sew`: `i64`

*Defined at line 7437*

---

## rv_vec

vd = a op b, unmasked (`vadd.vv vd, a, b`; `vsub.vv vd, a, b` is a - b):
vadd, vsub, vmul, vand, vor, vxor (OPIVV / OPMVV), vfadd, vfsub, vfmul,
vfdiv (OPFVV), at the element width VSETVLI set.

```fides
pub fn rv_vec(e: &mut NatEnc, kind: i64, vd: i64, a: i64, b: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `vd`: `i64`
- `a`: `i64`
- `b`: `i64`

*Defined at line 7444*

---

## rv_vredsum

vredsum.vs vd, vs2, vs1: vd[0] = vs1[0] + the sum of vs2's elements

```fides
pub fn rv_vredsum(e: &mut NatEnc, vd: i64, vs2: i64, vs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `vs2`: `i64`
- `vs1`: `i64`

*Defined at line 7474*

---

## rv_vmv_x_s

vmv.x.s rd, vs2 (element 0 to a general register) and vmv.v.x vd, rs1
(every element = rs1)

```fides
pub fn rv_vmv_x_s(e: &mut NatEnc, rd: i64, vs2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `rd`: `i64`
- `vs2`: `i64`

*Defined at line 7480*

---

## rv_vmv_v_x

```fides
pub fn rv_vmv_v_x(e: &mut NatEnc, vd: i64, rs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `rs1`: `i64`

*Defined at line 7484*

---

## x64_cmppd

cmppd x1, x2, imm (66 0F C2 /r ib): each f64 lane of x1 = all ones when
(x1 cmp x2), imm 0 eq, 4 neq (unordered: true for NaN), 1 lt, 2 le.

```fides
pub fn x64_cmppd(e: &mut NatEnc, x1: i64, x2: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x1`: `i64`
- `x2`: `i64`
- `imm`: `i64`

*Defined at line 7494*

---

## x64_vcmppd

vcmppd d, a, b, imm (VEX 66 0F C2 /r ib): d = (a cmp b) per f64 lane

```fides
pub fn x64_vcmppd(e: &mut NatEnc, d: i64, a: i64, b: i64, imm: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`
- `imm`: `i64`
- `l256`: `bool`

*Defined at line 7504*

---

## x64_cmppd_m

cmppd x, xmmword [base + disp], imm and vcmppd d, a, [base + disp],
imm: the compare with its second operand in memory

```fides
pub fn x64_cmppd_m(e: &mut NatEnc, x: i64, base: i64, disp: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `imm`: `i64`

*Defined at line 7513*

---

## x64_vcmppd_m

```fides
pub fn x64_vcmppd_m(e: &mut NatEnc, d: i64, a: i64, base: i64, disp: i64, imm: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `a`: `i64`
- `base`: `i64`
- `disp`: `i64`
- `imm`: `i64`
- `l256`: `bool`

*Defined at line 7522*

---

## x64_vpbroadcastq

vpbroadcastq d, xmm s (AVX2, VEX 66 0F38 59 /r W0): every 64-bit lane
of d = the low lane of s

```fides
pub fn x64_vpbroadcastq(e: &mut NatEnc, d: i64, s: i64, l256: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `s`: `i64`
- `l256`: `bool`

*Defined at line 7531*

---

## x64_vzeroupper

vzeroupper (C5 F8 77): the upper halves of every ymm register cleared,
after 256-bit code, so that the SSE code after it pays no transition

```fides
pub fn x64_vzeroupper(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7539*

---

## a64_fcmeq_zero_2d

fcmeq v d.2d, v n.2d, #0.0: each lane of d = all ones when n's lane is
+0.0 or -0.0 (NaN: zeros)

```fides
pub fn a64_fcmeq_zero_2d(e: &mut NatEnc, d: i64, n: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`

*Defined at line 7547*

---

## a64_bic_16b

bic v d.16b, v n.16b, v m.16b: d = n & ~m

```fides
pub fn a64_bic_16b(e: &mut NatEnc, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7552*

---

## a64_dup_elem_2d

dup v d.2d, v n.d[i] (both lanes = lane i) and dup d d, v n.d[i] (the
scalar form, `mov d0, v1.d[1]`)

```fides
pub fn a64_dup_elem_2d(e: &mut NatEnc, d: i64, n: i64, i: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `i`: `i64`

*Defined at line 7558*

---

## a64_dup_d_elem

```fides
pub fn a64_dup_d_elem(e: &mut NatEnc, d: i64, n: i64, i: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `i`: `i64`

*Defined at line 7562*

---

## rv_vsll_vi

vsll.vi vd, vs2, uimm: each element shifted left

```fides
pub fn rv_vsll_vi(e: &mut NatEnc, vd: i64, vs2: i64, uimm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `vs2`: `i64`
- `uimm`: `i64`

*Defined at line 7573*

---

## rv_vmseq_vi

vmseq.vi vd, vs2, simm: mask bit i of vd = (vs2[i] == simm)

```fides
pub fn rv_vmseq_vi(e: &mut NatEnc, vd: i64, vs2: i64, simm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `vs2`: `i64`
- `simm`: `i64`

*Defined at line 7578*

---

## rv_vmerge_vim

vmerge.vim vd, vs2, simm, v0: vd[i] = v0's mask bit i ? simm : vs2[i]

```fides
pub fn rv_vmerge_vim(e: &mut NatEnc, vd: i64, vs2: i64, simm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `vs2`: `i64`
- `simm`: `i64`

*Defined at line 7583*

---

## rv_vslidedown_vi

vslidedown.vi vd, vs2, uimm: vd[i] = vs2[i + uimm]

```fides
pub fn rv_vslidedown_vi(e: &mut NatEnc, vd: i64, vs2: i64, uimm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `vs2`: `i64`
- `uimm`: `i64`

*Defined at line 7588*

---

## rv_vfmv_f_s

vfmv.f.s fd, vs2 (element 0 to a float register) and vfmv.v.f vd, fs1
(every element = fs1)

```fides
pub fn rv_vfmv_f_s(e: &mut NatEnc, fd: i64, vs2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `fd`: `i64`
- `vs2`: `i64`

*Defined at line 7594*

---

## rv_vfmv_v_f

```fides
pub fn rv_vfmv_v_f(e: &mut NatEnc, vd: i64, fs1: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `vd`: `i64`
- `fs1`: `i64`

*Defined at line 7598*

---

## NatEnc_half

```fides
pub fn NatEnc_half(e: &mut NatEnc, h: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `h`: `i64`

*Defined at line 7610*

---

## t2_word

```fides
pub fn t2_word(e: &mut NatEnc, hw1: i64, hw2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `hw1`: `i64`
- `hw2`: `i64`

*Defined at line 7615*

---

## t2_movw

movw d, #imm16 (the low half, the high half cleared); movt d, #imm16
(the high half, the low kept): any 32-bit constant in two.

```fides
pub fn t2_movw(e: &mut NatEnc, d: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `imm`: `i64`

*Defined at line 7632*

---

## t2_movt

```fides
pub fn t2_movt(e: &mut NatEnc, d: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `imm`: `i64`

*Defined at line 7636*

---

## t2_addw

addw d, n, #imm12 and subw d, n, #imm12 (0..4095)

```fides
pub fn t2_addw(e: &mut NatEnc, d: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 7641*

---

## t2_subw

```fides
pub fn t2_subw(e: &mut NatEnc, d: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 7645*

---

## t2_alu

d = n op m, the wide register forms: kind NVK_ADD32 add.w, NVK_SUB32
sub.w, NVK_AND and.w, NVK_OR orr.w, NVK_XOR eor.w, NVK_MUL32 mul (T2).
Flags untouched (S = 0).

```fides
pub fn t2_alu(e: &mut NatEnc, kind: i64, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7652*

---

## t2_div

sdiv / udiv d, n, m (ARMv7-M: a division by zero gives 0, no trap)

```fides
pub fn t2_div(e: &mut NatEnc, d: i64, n: i64, m: i64, unsigned: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `unsigned`: `bool`

*Defined at line 7671*

---

## t2_shift

d = n shifted by register m: kind 0 lsl.w, 1 lsr.w, 2 asr.w

```fides
pub fn t2_shift(e: &mut NatEnc, kind: i64, d: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7680*

---

## t2_mov

mov.w d, m and cmp.w n, m

```fides
pub fn t2_mov(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 7685*

---

## t2_cmp

```fides
pub fn t2_cmp(e: &mut NatEnc, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7689*

---

## t2_ldst

ldr.w / str.w t, [n, #imm12] (word), ldrb.w / strb.w (byte): imm 0..4095

```fides
pub fn t2_ldst(e: &mut NatEnc, load: bool, byte: bool, t: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `load`: `bool`
- `byte`: `bool`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 7694*

---

## t2_push

push.w {mask} / pop.w {mask} (bit r per register; lr 14 on a push, pc
15 on a pop); at least two registers

```fides
pub fn t2_push(e: &mut NatEnc, mask: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `mask`: `i64`

*Defined at line 7707*

---

## t2_pop

```fides
pub fn t2_pop(e: &mut NatEnc, mask: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `mask`: `i64`

*Defined at line 7711*

---

## t2_b

```fides
pub fn t2_b(e: &mut NatEnc, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `off`: `i64`

*Defined at line 7728*

---

## t2_bl

```fides
pub fn t2_bl(e: &mut NatEnc, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `off`: `i64`

*Defined at line 7732*

---

## t2_bcc

b<cond>.w (T3), cond 0 eq .. 13 le: off from the address + 4, within
+-1 MB, even.

```fides
pub fn t2_bcc(e: &mut NatEnc, cond: i64, off: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `cond`: `i64`
- `off`: `i64`

*Defined at line 7738*

---

## t_bx

The 16-bit forms a function needs: bx lr, bx r, nop, udf #0 (a trap),
bkpt #imm8 (semihosting is bkpt 0xab on v7-M).

```fides
pub fn t_bx(e: &mut NatEnc, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `m`: `i64`

*Defined at line 7749*

---

## t_nop

```fides
pub fn t_nop(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7753*

---

## t_udf

```fides
pub fn t_udf(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7757*

---

## t_bkpt

```fides
pub fn t_bkpt(e: &mut NatEnc, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `imm`: `i64`

*Defined at line 7761*

---

## TDP_AND:

```fides
pub const TDP_AND: i64 = 0;
```

*Defined at line 7770*

---

## TDP_BIC:

```fides
pub const TDP_BIC: i64 = 1;
```

*Defined at line 7771*

---

## TDP_ORR:

```fides
pub const TDP_ORR: i64 = 2;
```

*Defined at line 7772*

---

## TDP_ORN:

```fides
pub const TDP_ORN: i64 = 3;
```

*Defined at line 7773*

---

## TDP_EOR:

```fides
pub const TDP_EOR: i64 = 4;
```

*Defined at line 7774*

---

## TDP_ADD:

```fides
pub const TDP_ADD: i64 = 8;
```

*Defined at line 7775*

---

## TDP_ADC:

```fides
pub const TDP_ADC: i64 = 10;
```

*Defined at line 7776*

---

## TDP_SBC:

```fides
pub const TDP_SBC: i64 = 11;
```

*Defined at line 7777*

---

## TDP_SUB:

```fides
pub const TDP_SUB: i64 = 13;
```

*Defined at line 7778*

---

## TDP_RSB:

```fides
pub const TDP_RSB: i64 = 14;
```

*Defined at line 7779*

---

## t2_dp

d = n op (m shifted): the wide register form; `s` sets the flags;
stype 0 lsl, 1 lsr, 2 asr, 3 ror by amt (0..31; 0 with lsl = no shift)

```fides
pub fn t2_dp(e: &mut NatEnc, op: i64, s: bool, d: i64, n: i64, m: i64, stype: i64, amt: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `s`: `bool`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `stype`: `i64`
- `amt`: `i64`

*Defined at line 7783*

---

## t2_dpi

d = n op #imm8 (0..255, the plain form of the modified immediate)

```fides
pub fn t2_dpi(e: &mut NatEnc, op: i64, s: bool, d: i64, n: i64, imm8: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `op`: `i64`
- `s`: `bool`
- `d`: `i64`
- `n`: `i64`
- `imm8`: `i64`

*Defined at line 7793*

---

## t2_shift_imm

lsl.w / lsr.w / asr.w d, m, #amt (1..31): mov.w with a shifted operand

```fides
pub fn t2_shift_imm(e: &mut NatEnc, stype: i64, d: i64, m: i64, amt: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `stype`: `i64`
- `d`: `i64`
- `m`: `i64`
- `amt`: `i64`

*Defined at line 7802*

---

## t2_umull

umull lo, hi, n, m (64-bit product of unsigned words)

```fides
pub fn t2_umull(e: &mut NatEnc, lo: i64, hi: i64, n: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `lo`: `i64`
- `hi`: `i64`
- `n`: `i64`
- `m`: `i64`

*Defined at line 7807*

---

## t2_mla

mla d, n, m, a (d = a + n * m) and mls d, n, m, a (d = a - n * m)

```fides
pub fn t2_mla(e: &mut NatEnc, d: i64, n: i64, m: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `a`: `i64`

*Defined at line 7812*

---

## t2_mls

```fides
pub fn t2_mls(e: &mut NatEnc, d: i64, n: i64, m: i64, a: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `n`: `i64`
- `m`: `i64`
- `a`: `i64`

*Defined at line 7816*

---

## t2_ext

sxtb.w (kind 0), sxth.w (1), uxtb.w (2), uxth.w (3) d, m

```fides
pub fn t2_ext(e: &mut NatEnc, kind: i64, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `d`: `i64`
- `m`: `i64`

*Defined at line 7821*

---

## t2_ldst_x

strh.w (kind 0), ldrh.w (1), ldrsb.w (2), ldrsh.w (3) t, [n, #imm12]

```fides
pub fn t2_ldst_x(e: &mut NatEnc, kind: i64, t: i64, n: i64, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `kind`: `i64`
- `t`: `i64`
- `n`: `i64`
- `imm`: `i64`

*Defined at line 7834*

---

## t_svc

svc #imm8 (a Linux system call is svc 0), blx m, and the 16-bit mov
of any two registers (mov sp, r12 among them)

```fides
pub fn t_svc(e: &mut NatEnc, imm: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `imm`: `i64`

*Defined at line 7848*

---

## t_blx

```fides
pub fn t_blx(e: &mut NatEnc, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `m`: `i64`

*Defined at line 7852*

---

## t_mov

```fides
pub fn t_mov(e: &mut NatEnc, d: i64, m: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `d`: `i64`
- `m`: `i64`

*Defined at line 7856*

---

## x64_movq_xr

xmm x = the 64 bits of r (movq), or the low 32 (movd)

```fides
pub fn x64_movq_xr(e: &mut NatEnc, x: i64, r: i64, w: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x`: `i64`
- `r`: `i64`
- `w`: `bool`

*Defined at line 7863*

---

## x64_movq_rx

```fides
pub fn x64_movq_rx(e: &mut NatEnc, r: i64, x: i64, w: bool) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `x`: `i64`
- `w`: `bool`

*Defined at line 7871*

---

## x64_sse

prefix 0F op /r between xmm registers (F2 58 addsd, 5C subsd, 59 mulsd,
5E divsd, 51 sqrtsd, F3 5A cvtss2sd, F2 5A cvtsd2ss, 66 2E ucomisd)

```fides
pub fn x64_sse(e: &mut NatEnc, prefix: i64, op: i64, x1: i64, x2: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `prefix`: `i64`
- `op`: `i64`
- `x1`: `i64`
- `x2`: `i64`

*Defined at line 7881*

---

## x64_cvtsi2sd

```fides
pub fn x64_cvtsi2sd(e: &mut NatEnc, x: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x`: `i64`
- `r`: `i64`

*Defined at line 7889*

---

## x64_cvttsd2si

```fides
pub fn x64_cvttsd2si(e: &mut NatEnc, r: i64, x: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `r`: `i64`
- `x`: `i64`

*Defined at line 7897*

---

## x64_roundsd

roundsd x1, x2, mode (1 floor, 2 ceil, 3 trunc)

```fides
pub fn x64_roundsd(e: &mut NatEnc, x1: i64, x2: i64, mode: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `x1`: `i64`
- `x2`: `i64`
- `mode`: `i64`

*Defined at line 7906*

---

## x64_vfmadd213sd_012

xmm0 = xmm1 * xmm0 + xmm2, rounded once

```fides
pub fn x64_vfmadd213sd_012(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 7917*

---

## x64_xchg_mem

xchg [base], r (an implicit lock)

```fides
pub fn x64_xchg_mem(e: &mut NatEnc, base: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `r`: `i64`

*Defined at line 7928*

---

## x64_lock_xadd

lock xadd [base], r

```fides
pub fn x64_lock_xadd(e: &mut NatEnc, base: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `r`: `i64`

*Defined at line 7935*

---

## x64_lock_cmpxchg

lock cmpxchg [base], r (compares with rax)

```fides
pub fn x64_lock_cmpxchg(e: &mut NatEnc, base: i64, r: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `base`: `i64`
- `r`: `i64`

*Defined at line 7944*

---

## NAT_MACHO_BASE:

```fides
pub const NAT_MACHO_BASE: i64 = 0x100000000;
```

*Defined at line 7969*

---

## NAT_MACHO_HDR:

The header + load commands take one page; the code follows.

```fides
pub const NAT_MACHO_HDR: i64 = 0x4000;
```

*Defined at line 7971*

---

## nat_macho_set_platform

XT77: set the platform of the Mach-O images that follow; platform 0 (or
1) puts back macOS 11.0, SDK 13.0, so a build after an iOS one in the
same process (fid serve, the LSP) writes a macOS image again.

```fides
pub fn nat_macho_set_platform(platform: i64, minos: i64, sdk: i64) {
```

**Parameters:**

- `platform`: `i64`
- `minos`: `i64`
- `sdk`: `i64`

*Defined at line 8202*

---

## nat_macho_dylib_path

TG15: the dylib dyld loads for `extern "C" link "<lib>"` on macOS ("",
"c", "m", "pthread", "dl", "System": libSystem, which holds them all).
A path ("/..." or "@rpath/...", "@executable_path/...") stays as it is;
"Name.framework" (or "framework:Name") is the system framework; a name
ending in ".dylib" is under /usr/lib ("z.1.dylib" and "libz.1.dylib":
/usr/lib/libz.1.dylib); anything else is /usr/lib/lib<name>.dylib
(sqlite3, z, c++, curl, ...).

```fides
pub fn nat_macho_dylib_path(lib: str) -> str {
```

**Parameters:**

- `lib`: `str`

**Returns:** `str`

*Defined at line 8232*

---

## nat_macho_import

TG15: one entry of nat_macho's `imp_names` for symbol `sym` (with its
leading underscore) of `link "<lib>"`: the bare symbol when it is
libSystem's (dylib ordinal 1), else "<dylib path>|<symbol>", which
nat_macho binds through an LC_LOAD_DYLIB of its own.

```fides
pub fn nat_macho_import(lib: str, sym: str) -> str {
```

**Parameters:**

- `lib`: `str`
- `sym`: `str`

**Returns:** `str`

*Defined at line 8260*

---

## nat_tbd_has

CB9 (2g.106): does the text-based stub `b` (its bytes) name `sym` (with
its leading underscore) as a whole word?

```fides
pub fn nat_tbd_has(b: &Vec<i64>, sym: str) -> bool {
```

**Parameters:**

- `b`: `&Vec<i64>`
- `sym`: `str`

**Returns:** `bool`

*Defined at line 8276*

---

## nat_macho_sdk

CB9 (2g.106): the macOS SDK whose text-based stubs say what each system
library exports: SDKROOT, else the Command Line Tools' or Xcode's (on a
Mac); "" when there is none (a build on another system).

```fides
pub fn nat_macho_sdk() -> str {
```

**Returns:** `str`

*Defined at line 8301*

---

## nat_macho_tbd_path

CB9 (2g.106): the text-based stub in `sdk` of the dylib at `path`
(nat_macho_dylib_path's result): the path without ".dylib" plus ".tbd";
"" for a path relative to the program (@rpath, ...).

```fides
pub fn nat_macho_tbd_path(sdk: str, path: str) -> str {
```

**Parameters:**

- `sdk`: `str`
- `path`: `str`

**Returns:** `str`

*Defined at line 8322*

---

## nat_macho_link_missing

CB9 (2g.106): the C functions of `names` that the dylib each is bound
from (nat_macho_dylib_path of its entry of `libs`; libSystem's stub
holds its re-exported libraries too, libm's among them) does not export,
as the SDK's text-based stubs say (nat_macho_sdk): a name is listed only
when its stub was found and read and does not name it. Without an SDK
(or the stub) nothing is listed: dyld reports the symbol by name.

```fides
pub fn nat_macho_link_missing(names: &Vec<str>, libs: &Vec<str>) -> Vec<str> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `libs`: `&Vec<str>`

**Returns:** `Vec<str>`

*Defined at line 8338*

---

## nat_macho_split_imports

TG15: `imp_names` (nat_macho) split into what dyld binds: the symbols,
each one's dylib ordinal (1: libSystem, 2.. the other dylibs in order of
first use) and those other dylibs' paths, through the out parameters.

```fides
pub fn nat_macho_split_imports(imp_names: &Vec<str>, syms: &mut Vec<str>, ords: &mut Vec<i64>, dylibs: &mut Vec<str>) {
```

**Parameters:**

- `imp_names`: `&Vec<str>`
- `syms`: `&mut Vec<str>`
- `ords`: `&mut Vec<i64>`
- `dylibs`: `&mut Vec<str>`

*Defined at line 8379*

---

## nat_macho_a64_stub

TG15: the body of a stub (its label already placed) that calls the C
function in GOT slot `slot` (bound by dyld) on Apple arm64, from the
machine layer's convention: arguments 0..7 in x0..x7, the rest at
[sp + 8 * (j - 8)] at entry, every float as its bits in a general
register (an f32 its 32-bit pattern: MI25); the result back in x0.
`sig` is ng_ldyn_sig's ("id>w"), with a `.` after the fixed arguments
of a variadic C function ("i.id>w": printf's format, then the rest).
The fixed arguments follow AAPCS64 (each integer to the next free x
register, never above its own, each float to the next d / s register);
Apple's variant puts every variadic argument in an 8-byte slot on the
stack, from sp up, in order (an f32 promoted to double, as C does),
where AAPCS64 proper would use registers. A frame record (x29, x30)
around the call; x16 / x17 / v16 are scratch (the intra-procedure-call
registers), x18 is never touched (Apple's platform register).

```fides
pub fn nat_macho_a64_stub(e: &mut NatEnc, sig: str, slot: i64) {
```

**Parameters:**

- `e`: `&mut NatEnc`
- `sig`: `str`
- `slot`: `i64`

*Defined at line 8438*

---

## nat_macho

A 64-bit Mach-O executable. `image` is the contiguous code+data blob
(code at [0, data_off), data at [data_off, end)); `entry` the entry
offset in the code; `rebase_offs` the image offsets of the NATFIX_ABS64
words (each >= data_off); `imp_names` the libSystem symbols and
`imp_offs` their GOT slot offsets in the image (each >= data_off).
g_nat_abs_base must already be NAT_MACHO_BASE + NAT_MACHO_HDR.
`names` / `starts`: the functions placed in the code and their image
offsets (DB2, DB10: local symbols of __text, so nm, lldb, Instruments
and atos name our functions; empty: no symbols).

```fides
pub fn nat_macho(arch: i64, image: Vec<i64>, entry: i64, data_off: i64, rebase_offs: Vec<i64>, imp_names: Vec<str>, imp_offs: Vec<i64>, names: Vec<str>, starts: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `rebase_offs`: `Vec<i64>`
- `imp_names`: `Vec<str>`
- `imp_offs`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 8571*

---

## nat_macho_img

TG13: nat_macho as a layout over image_writer (same arguments, same
bytes): __PAGEZERO, __TEXT (the headers and code), __DATA, __LINKEDIT
(rebase and bind opcodes, the symbols, their names, the signature).

```fides
pub fn nat_macho_img(arch: i64, image: &Vec<i64>, entry: i64, data_off: i64, rebase_offs: &Vec<i64>, imp_names: &Vec<str>, imp_offs: &Vec<i64>, names: &Vec<str>, starts: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `&Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `rebase_offs`: `&Vec<i64>`
- `imp_names`: `&Vec<str>`
- `imp_offs`: `&Vec<i64>`
- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 8582*

---

## nat_macho_debug

nat_macho with DWARF: `ends` the end offset of each function of
`names` / `starts`, `file` the input file, `lrows` the line rows
(nat_debug_line's [image offset, line, 2 * file + end?]). Without rows
or functions the image is nat_macho's.

```fides
pub fn nat_macho_debug(arch: i64, image: Vec<i64>, entry: i64, data_off: i64, rebase_offs: Vec<i64>, imp_names: Vec<str>, imp_offs: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `rebase_offs`: `Vec<i64>`
- `imp_names`: `Vec<str>`
- `imp_offs`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 8746*

---

## nat_macho_debug_cfi

CT9: nat_macho_debug with the frame rows `cfi` (DB6, nat_elf_debug's).
When the program's struct types (g_nat_dbg_types) or its variables
(g_nat_dbg_ext, g_nat_dbg_vars) are there, the __DWARF segment holds
the DWARF a Linux image has (nat_macho_dwarf_full): the names as Fides
writes them, the structs with their fields, each function's parameters
and variables with their types and locations, .debug_frame and
.debug_loc; so lldb's `breakpoint set -n 'Point::len2'`, `type lookup
Seg` and `frame variable` work. Without them, nat_macho_dwarf's
functions and lines (as before).

```fides
pub fn nat_macho_debug_cfi(arch: i64, image: Vec<i64>, entry: i64, data_off: i64, rebase_offs: Vec<i64>, imp_names: Vec<str>, imp_offs: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, cfi: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`
- `data_off`: `i64`
- `rebase_offs`: `Vec<i64>`
- `imp_names`: `Vec<str>`
- `imp_offs`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `cfi`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 8760*

---

## nat_macho_dwarf_full

CT9: the full DWARF of a Mach-O image (code offset 0 at NAT_MACHO_BASE
+ NAT_MACHO_HDR), the sections nat_dwarf_build writes for ELF and
`--nat` PE: [__debug_abbrev, __debug_info, __debug_line, __debug_frame]
and __debug_loc when a value has a location list.

```fides
pub fn nat_macho_dwarf_full(arch: i64, image: Vec<i64>, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, cfi: Vec<i64>) -> Vec<Vec<i64>> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `cfi`: `Vec<i64>`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 8794*

---

## nat_macho_dwarf

The DWARF 4 sections of a Mach-O image, [__debug_abbrev, __debug_info,
__debug_line], for the functions `names` at image offsets [starts,
ends) with line rows `lrows`; image offset 0 is at address `abase`. The
compile unit is named `file` and spans the functions; each function has
its declaration file and line (the file of its first row and the `fn`
line there, as nat_elf_debug's DB2 entries) and, when g_nat_dbg_shown names it otherwise (a method as
written, `Point::len`), that name with its symbol as the linkage name.

```fides
pub fn nat_macho_dwarf(names: &Vec<str>, starts: &Vec<i64>, ends: Vec<i64>, file: str, lrows: Vec<i64>, abase: i64) -> Vec<Vec<i64>> {
```

**Parameters:**

- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `Vec<i64>`
- `file`: `str`
- `lrows`: `Vec<i64>`
- `abase`: `i64`

**Returns:** `Vec<Vec<i64>>`

*Defined at line 8819*

---

## nat_macho_cpu

L2B: the Mach-O CPU type and subtype of a 64-bit Mach-O `m`
(mach_header_64: magic 0xFEEDFACF, cputype and cpusubtype little-endian
at 4 and 8), [-1, -1] when m is not one.

```fides
pub fn nat_macho_cpu(m: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `m`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 9067*

---

## nat_macho_fat

L2B: a macOS universal ("fat") binary holding the two 64-bit Mach-O
executables `first` and `second` (nat_macho's output, x86-64 and
AArch64): a big-endian fat_header (magic 0xCAFEBABE, 2 architectures),
one fat_arch each (cputype, cpusubtype, offset, size, align = 14, as
lipo writes for arm64), then each slice at a 16 KB boundary. Each
slice keeps its own code signature (the fat wrapper is not signed).
Empty when either is not a 64-bit Mach-O, or both have one CPU type.

```fides
pub fn nat_macho_fat(first: Vec<i64>, second: Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `first`: `Vec<i64>`
- `second`: `Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 9090*

---

## nat_hot_first

`idx` (positions of functions named `names`) with the ones the profile
counts first, by count (equal counts keep their order), then the rest
in their order. Without a profile, or when it names none of them, `idx`
comes back as it was.

```fides
pub fn nat_hot_first(idx: Vec<i64>, names: Vec<str>) -> Vec<i64> {
```

**Parameters:**

- `idx`: `Vec<i64>`
- `names`: `Vec<str>`

**Returns:** `Vec<i64>`

*Defined at line 9124*

---

## nat_line_files_reset

A new image: the file table holds only the input file.

```fides
pub fn nat_line_files_reset(main_file: str) {
```

**Parameters:**

- `main_file`: `str`

*Defined at line 9187*

---

## nat_line_file_index

The index of `path` in the file table (added when new).

```fides
pub fn nat_line_file_index(path: str) -> i64 {
```

**Parameters:**

- `path`: `str`

**Returns:** `i64`

*Defined at line 9197*

---

## nat_line_fn_set

Record the file index of function `name` (its rows are written later,
when the function is emitted).

```fides
pub fn nat_line_fn_set(name: str, ix: i64) {
```

**Parameters:**

- `name`: `str`
- `ix`: `i64`

*Defined at line 9212*

---

## nat_line_fn_file

The file index nat_line_fn_set recorded for `name` (0: the input file).

```fides
pub fn nat_line_fn_file(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 9220*

---

## nat_elf_object

```fides
pub fn nat_elf_object(arch: i64, image: Vec<i64>, entry: i64, names: Vec<str>, starts: Vec<i64>, ends: Vec<i64>, fixups: Vec<NatFix>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `Vec<i64>`
- `entry`: `i64`
- `names`: `Vec<str>`
- `starts`: `Vec<i64>`
- `ends`: `Vec<i64>`
- `fixups`: `Vec<NatFix>`

**Returns:** `Vec<i64>`

*Defined at line 9256*

---

## nat_elf_object_img

```fides
pub fn nat_elf_object_img(arch: i64, image: &Vec<i64>, entry: i64, names: &Vec<str>, starts: &Vec<i64>, ends: &Vec<i64>, fixups: &Vec<NatFix>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `image`: `&Vec<i64>`
- `entry`: `i64`
- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `&Vec<i64>`
- `fixups`: `&Vec<NatFix>`

**Returns:** `Vec<i64>`

*Defined at line 9268*

---

## nat_obj_export_syms

The global function symbols of g_nat_obj_exports, in section `sect`.

```fides
pub fn nat_obj_export_syms(sect: i64) -> Vec<ImgSym> {
```

**Parameters:**

- `sect`: `i64`

**Returns:** `Vec<ImgSym>`

*Defined at line 9395*

---

## nat_coff_dll_lib

TG12: the import library a DLL's functions come from (kernel32.dll ->
kernel32.lib; the API set of WaitOnAddress -> synchronization.lib).

```fides
pub fn nat_coff_dll_lib(dll: str) -> str {
```

**Parameters:**

- `dll`: `str`

**Returns:** `str`

*Defined at line 9418*

---

## nat_coff_imports

TG12: the imports of a `--nat` PE image, read from its import directory
(`imp_off`, `imp_size`: nat_ir.fd ng_gen_win_imports' tables, the image
at RVA 0x1000, each import address table slot still holding its
hint/name entry's RVA): each slot's image offset, the function's name
and its DLL, through the out parameters. "" or what is wrong.

```fides
pub fn nat_coff_imports(image: &Vec<i64>, imp_off: i64, imp_size: i64, slots: &mut Vec<i64>, names: &mut Vec<str>, dlls: &mut Vec<str>) -> str {
```

**Parameters:**

- `image`: `&Vec<i64>`
- `imp_off`: `i64`
- `imp_size`: `i64`
- `slots`: `&mut Vec<i64>`
- `names`: `&mut Vec<str>`
- `dlls`: `&mut Vec<str>`

**Returns:** `str`

*Defined at line 9437*

---

## nat_coff_object

```fides
pub fn nat_coff_object(e: &NatEnc, entry: i64, data_off: i64, imp_off: i64, imp_size: i64, iat_off: i64, iat_size: i64, names: &Vec<str>, starts: &Vec<i64>, ends: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `e`: `&NatEnc`
- `entry`: `i64`
- `data_off`: `i64`
- `imp_off`: `i64`
- `imp_size`: `i64`
- `iat_off`: `i64`
- `iat_size`: `i64`
- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 9513*

---

## nat_macho_object

```fides
pub fn nat_macho_object(arch: i64, e: &NatEnc, entry: i64, data_off: i64, imp_names: &Vec<str>, imp_offs: &Vec<i64>, names: &Vec<str>, starts: &Vec<i64>, ends: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `e`: `&NatEnc`
- `entry`: `i64`
- `data_off`: `i64`
- `imp_names`: `&Vec<str>`
- `imp_offs`: `&Vec<i64>`
- `names`: `&Vec<str>`
- `starts`: `&Vec<i64>`
- `ends`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 9686*

---

## NatAsmCode

```fides
pub struct NatAsmCode {
```

**Fields:**

| Name | Type |
|------|------|
| `bytes` | `Vec<i64>` |
| `error` | `str` |

*Defined at line 9896*

---

## nat_enc_image_start

The state of the writers here for a new image (nat_ir calls it first):
the line table's files (DB1) and the pending asm! thunks.

```fides
pub fn nat_enc_image_start(main_file: str) {
```

**Parameters:**

- `main_file`: `str`

*Defined at line 9903*

---

## nat_asm_add

Queue the thunk `code` at `label`.

```fides
pub fn nat_asm_add(label: i64, code: Vec<i64>) {
```

**Parameters:**

- `label`: `i64`
- `code`: `Vec<i64>`

*Defined at line 9909*

---

## nat_asm_emit

Place every queued thunk at the current position (after the code).

```fides
pub fn nat_asm_emit(e: &mut NatEnc) {
```

**Parameters:**

- `e`: `&mut NatEnc`

*Defined at line 9920*

---

## nat_asm_reg

The register number `s` names on `arch` (x86-64: rax..r15; AArch64:
x0..x30, fp, lr; RISC-V: x0..x31 and the ABI names), else -1. The stack
pointer is never an operand register here.

```fides
pub fn nat_asm_reg(arch: i64, s: str) -> i64 {
```

**Parameters:**

- `arch`: `i64`
- `s`: `str`

**Returns:** `i64`

*Defined at line 9940*

---

## NatAsmImm

An immediate operand: `#12`, `12`, `-3`, `0x1f`; ok false when it is not one.

```fides
pub struct NatAsmImm {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `v` | `i64` |

*Defined at line 10004*

---

## nat_asm_encode

Encode an asm! template for AArch64 or RISC-V: instructions separated
by `;` or new lines. AArch64: nop, yield, isb, dmb/dsb ish|sy, brk #n,
svc #n, mov, movz-style `mov xd, #imm`, add/sub (register or #imm),
and/orr/eor/mul/sdiv/udiv/lsl/lsr/asr (registers; lsl/lsr/asr also
#imm), neg, mvn, mrs xd, cntvct_el0|cntfrq_el0|tpidr_el0. RISC-V: nop,
fence, ecall, ebreak, mv, li, neg, not, add/sub/and/or/xor/sll/srl/sra/
slt/sltu/mul/mulh/mulhu/div/divu/rem/remu, addi/andi/ori/xori/slli/
srli/srai, rdcycle/rdtime/rdinstret. `ret` is refused (it would leave
the thunk without restoring what it saved).

```fides
pub fn nat_asm_encode(arch: i64, template: str) -> NatAsmCode {
```

**Parameters:**

- `arch`: `i64`
- `template`: `str`

**Returns:** `NatAsmCode`

*Defined at line 10083*

---

## nat_asm_thunk

The thunk of one asm! site (see above): `tpl` the template's bytes,
`ins` the register of each input (input i arrives in argument register
i: x86-64 rdi, rsi, rdx, rcx; AArch64 x0..x7; RISC-V a0..a7), `out`
the output register (-1: none, the result is 0).

```fides
pub fn nat_asm_thunk(arch: i64, tpl: Vec<i64>, ins: Vec<i64>, out: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`
- `tpl`: `Vec<i64>`
- `ins`: `Vec<i64>`
- `out`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 10491*

---

## nat_sel_ref

```fides
pub fn nat_sel_ref(op: i64, imm: i64, a: i64, b: i64) -> i64 {
```

**Parameters:**

- `op`: `i64`
- `imm`: `i64`
- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 10772*

---

## nat_sel_cases

The cases nat_sel_check selects: [op, d, a, b, imm] each; b = -2 means
the target's zero register (x86-64's 60, RISC-V's x0; nat_ir passes it).

```fides
pub fn nat_sel_cases(arch: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 10973*

---

## nsel_addi_imms

TG1: constants an MO_ADDI of `arch` may hold (nat_ir.fd ng_addi_fits:
x86-64 a signed 32-bit immediate, AArch64 -4095..4095, RISC-V
-2048..2047).

```fides
pub fn nsel_addi_imms(arch: i64) -> Vec<i64> {
```

**Parameters:**

- `arch`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 11071*

---

## nat_sel_eval_case

```fides
pub fn nat_sel_eval_case(arch: i64, code: Vec<i64>, op: i64, d: i64, a: i64, b: i64, imm: i64, zr: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `Vec<i64>`
- `op`: `i64`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`
- `imm`: `i64`
- `zr`: `i64`

**Returns:** `str`

*Defined at line 11107*

---

## nsel_exec_m

RL6: run `code` from byte 0 with memory, branches and calls (NselM).

```fides
pub fn nsel_exec_m(arch: i64, code: &Vec<i64>, regs: &mut Vec<i64>, m: &mut NselM) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `&Vec<i64>`
- `regs`: `&mut Vec<i64>`
- `m`: `&mut NselM`

**Returns:** `str`

*Defined at line 11190*

---

## NselM

```fides
pub struct NselM {
```

**Fields:**

| Name | Type |
|------|------|
| `wa` | `Vec<i64>` |
| `wv` | `Vec<i64>` |
| `stop` | `i64` |
| `pc` | `i64` |
| `call_at` | `i64` |
| `call_to` | `i64` |
| `call_regs` | `Vec<i64>` |
| `np` | `i64` |
| `ea` | `i64` |
| `fl` | `i64` |
| `fill` | `i64` |

*Defined at line 11227*

---

## NselM_new

```fides
pub fn NselM_new(stop: i64) -> NselM {
```

**Parameters:**

- `stop`: `i64`

**Returns:** `NselM`

*Defined at line 11245*

---

## nsel_mem_init

```fides
pub fn nsel_mem_init(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 11253*

---

## nsel_mem_rd

```fides
pub fn nsel_mem_rd(m: &NselM, addr: i64, size: i64) -> i64 {
```

**Parameters:**

- `m`: `&NselM`
- `addr`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 11275*

---

## nsel_ext_sz

```fides
pub fn nsel_ext_sz(v: i64, size: i64, signed: bool) -> i64 {
```

**Parameters:**

- `v`: `i64`
- `size`: `i64`
- `signed`: `bool`

**Returns:** `i64`

*Defined at line 11297*

---

## nsf_isnan

```fides
pub fn nsf_isnan(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 11326*

---

## nsf_isnan32

```fides
pub fn nsf_isnan32(w: i64) -> bool {
```

**Parameters:**

- `w`: `i64`

**Returns:** `bool`

*Defined at line 11330*

---

## nsf_widen

```fides
pub fn nsf_widen(w0: i64) -> i64 {
```

**Parameters:**

- `w0`: `i64`

**Returns:** `i64`

*Defined at line 11336*

---

## nsf_narrow

```fides
pub fn nsf_narrow(b: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`

**Returns:** `i64`

*Defined at line 11363*

---

## nsf_op

```fides
pub fn nsf_op(op: i64, a: i64, b: i64, is32: bool) -> i64 {
```

**Parameters:**

- `op`: `i64`
- `a`: `i64`
- `b`: `i64`
- `is32`: `bool`

**Returns:** `i64`

*Defined at line 11373*

---

## nsf_cmp

```fides
pub fn nsf_cmp(a: i64, b: i64) -> i64 {
```

**Parameters:**

- `a`: `i64`
- `b`: `i64`

**Returns:** `i64`

*Defined at line 11402*

---

## nsf_f2i_ok

```fides
pub fn nsf_f2i_ok(b: i64) -> bool {
```

**Parameters:**

- `b`: `i64`

**Returns:** `bool`

*Defined at line 11432*

---

## nsf_f2i

```fides
pub fn nsf_f2i(b: i64, bad: i64) -> i64 {
```

**Parameters:**

- `b`: `i64`
- `bad`: `i64`

**Returns:** `i64`

*Defined at line 11445*

---

## nsf_i2f

```fides
pub fn nsf_i2f(v: i64) -> i64 {
```

**Parameters:**

- `v`: `i64`

**Returns:** `i64`

*Defined at line 11455*

---

## NSEL_FBIN:

```fides
pub const NSEL_FBIN: i64 = 32;
```

*Defined at line 13168*

---

## NSEL_FCMP:

```fides
pub const NSEL_FCMP: i64 = 33;
```

*Defined at line 13169*

---

## NSEL_I2F:

```fides
pub const NSEL_I2F: i64 = 34;
```

*Defined at line 13170*

---

## NSEL_F2I:

```fides
pub const NSEL_F2I: i64 = 35;
```

*Defined at line 13171*

---

## NSEL_BNEZ:

```fides
pub const NSEL_BNEZ: i64 = 12;
```

*Defined at line 13172*

---

## NSEL_BEQZ:

```fides
pub const NSEL_BEQZ: i64 = 13;
```

*Defined at line 13173*

---

## NSEL_BRCC:

```fides
pub const NSEL_BRCC: i64 = 45;
```

*Defined at line 13174*

---

## NSEL_TBNZ:

```fides
pub const NSEL_TBNZ: i64 = 53;
```

*Defined at line 13175*

---

## nsf_values

```fides
pub fn nsf_values(is32: bool) -> Vec<i64> {
```

**Parameters:**

- `is32`: `bool`

**Returns:** `Vec<i64>`

*Defined at line 13182*

---

## nat_sel_fref

```fides
pub fn nat_sel_fref(op: i64, imm: i64, is32: bool, a: i64, b: i64, ok: &mut Vec<i64>) -> i64 {
```

**Parameters:**

- `op`: `i64`
- `imm`: `i64`
- `is32`: `bool`
- `a`: `i64`
- `b`: `i64`
- `ok`: `&mut Vec<i64>`

**Returns:** `i64`

*Defined at line 13334*

---

## nat_sel_eval_float

```fides
pub fn nat_sel_eval_float(arch: i64, code: Vec<i64>, op: i64, d: i64, a: i64, b: i64, imm: i64, w: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `Vec<i64>`
- `op`: `i64`
- `d`: `i64`
- `a`: `i64`
- `b`: `i64`
- `imm`: `i64`
- `w`: `i64`

**Returns:** `str`

*Defined at line 13387*

---

## nat_sel_eval_mem

```fides
pub fn nat_sel_eval_mem(arch: i64, code: Vec<i64>, store: bool, r: i64, base: i64, idx: i64, sh: i64, disp: i64, size: i64, signed: bool) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `Vec<i64>`
- `store`: `bool`
- `r`: `i64`
- `base`: `i64`
- `idx`: `i64`
- `sh`: `i64`
- `disp`: `i64`
- `size`: `i64`
- `signed`: `bool`

**Returns:** `str`

*Defined at line 13473*

---

## nat_sel_eval_branch

```fides
pub fn nat_sel_eval_branch(arch: i64, code: Vec<i64>, ft: i64, tk: i64, kind: i64, a: i64, b: i64, i1: i64, i2: i64, zr: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `Vec<i64>`
- `ft`: `i64`
- `tk`: `i64`
- `kind`: `i64`
- `a`: `i64`
- `b`: `i64`
- `i1`: `i64`
- `i2`: `i64`
- `zr`: `i64`

**Returns:** `str`

*Defined at line 13606*

---

## nat_sel_eval_call

```fides
pub fn nat_sel_eval_call(arch: i64, code: Vec<i64>, init_r: Vec<i64>, init_v: Vec<i64>, want_r: Vec<i64>, want_v: Vec<i64>, want_s: Vec<i64>, sp: i64, spv: i64, target: i64) -> str {
```

**Parameters:**

- `arch`: `i64`
- `code`: `Vec<i64>`
- `init_r`: `Vec<i64>`
- `init_v`: `Vec<i64>`
- `want_r`: `Vec<i64>`
- `want_v`: `Vec<i64>`
- `want_s`: `Vec<i64>`
- `sp`: `i64`
- `spv`: `i64`
- `target`: `i64`

**Returns:** `str`

*Defined at line 13683*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
