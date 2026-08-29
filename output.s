.section .text
.globl _start

_start:
    bl p_main
    mov x8, #93
    mov x0, #0
    svc #0

p_token_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    add x0, sp, #24
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_token_kind_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_1
    b .match_arm_1_2
    b .match_arm_2_3
    b .match_arm_3_4
    b .match_arm_4_5
    b .match_arm_5_6
    b .match_arm_6_7
    b .match_arm_7_8
    b .match_arm_8_9
    b .match_arm_9_10
    b .match_arm_10_11
    b .match_arm_11_12
    b .match_arm_12_13
    b .match_arm_13_14
    b .match_arm_14_15
    b .match_arm_15_16
    b .match_arm_16_17
    b .match_arm_17_18
    b .match_arm_18_19
    b .match_arm_19_20
    b .match_arm_20_21
    b .match_arm_21_22
    b .match_arm_22_23
    b .match_arm_23_24
    b .match_arm_24_25
    b .match_arm_25_26
    b .match_arm_26_27
    b .match_arm_27_28
    b .match_arm_28_29
    b .match_arm_29_30
    b .match_arm_30_31
    b .match_arm_31_32
    b .match_arm_32_33
    b .match_arm_33_34
    b .match_arm_34_35
    b .match_arm_35_36
    b .match_arm_36_37
    b .match_arm_37_38
    b .match_arm_38_39
    b .match_arm_39_40
    b .match_arm_40_41
    b .match_arm_41_42
    b .match_arm_42_43
    b .match_arm_43_44
    b .match_arm_44_45
    b .match_arm_45_46
    b .match_arm_46_47
    b .match_arm_47_48
    b .match_arm_48_49
    b .match_arm_49_50
    b .match_arm_50_51
    b .match_arm_51_52
    b .match_arm_52_53
    b .match_arm_53_54
    b .match_arm_54_55
    b .match_arm_55_56
    b .match_arm_56_57
    b .match_arm_57_58
    b .match_arm_58_59
    b .match_arm_59_60
    b .match_arm_60_61
    b .match_arm_61_62
    b .match_arm_62_63
    b .match_arm_63_64
    b .match_arm_64_65
    b .match_arm_65_66
    b .match_arm_66_67
    b .match_arm_67_68
    b .match_arm_68_69
    b .match_arm_69_70
    b .match_arm_70_71
    b .match_arm_71_72
    b .match_arm_72_73
    b .match_arm_73_74
    b .match_arm_74_75
    b .match_arm_75_76
    b .match_arm_76_77
    b .match_arm_77_78
    b .match_arm_78_79
    b .match_arm_79_80
    b .match_arm_80_81
    b .match_arm_81_82
    b .match_arm_82_83
    b .match_arm_83_84
    b .match_arm_84_85
    b .match_arm_85_86
    b .match_arm_86_87
    b .match_arm_87_88
    b .match_arm_88_89
    b .match_arm_89_90
    b .match_arm_90_91
    b .match_arm_91_92
    b .match_arm_92_93
    b .match_arm_93_94
    b .match_arm_94_95
    b .match_arm_95_96
    b .match_arm_96_97
    b .match_arm_97_98
    b .match_arm_98_99
    b .match_arm_99_100
    b .match_arm_100_101
    b .match_arm_101_102
    b .match_arm_102_103
    b .match_arm_103_104
    b .match_arm_104_105
    b .match_arm_105_106
    b .match_arm_106_107
    b .match_arm_107_108
    b .match_arm_108_109
    b .match_arm_109_110
    b .match_arm_110_111
    b .match_arm_111_112
    b .match_arm_112_113
    b .match_arm_113_114
    b .match_arm_114_115
    b .match_arm_115_116
    b .match_arm_116_117
    b .match_arm_117_118
    b .match_arm_118_119
    b .match_arm_119_120
    b .match_arm_120_121
    b .match_arm_121_122
    b .match_arm_122_123
    b .match_arm_123_124
    b .match_arm_124_125
    b .match_arm_125_126
    b .match_arm_126_127
    b .match_arm_127_128
    b .match_arm_128_129
    b .match_arm_129_130
    b .match_arm_130_131
    b .match_arm_131_132
    b .match_arm_132_133
    b .match_arm_133_134
    b .match_arm_134_135
    b .match_end_0
.match_arm_0_1:
    adr x0, .str_0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_1_2:
    adr x0, .str_1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_2_3:
    adr x0, .str_2
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_3_4:
    adr x0, .str_3
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_4_5:
    adr x0, .str_4
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_5_6:
    adr x0, .str_5
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_6_7:
    adr x0, .str_6
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_7_8:
    adr x0, .str_7
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_8_9:
    adr x0, .str_8
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_9_10:
    adr x0, .str_9
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_10_11:
    adr x0, .str_10
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_11_12:
    adr x0, .str_11
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_12_13:
    adr x0, .str_12
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_13_14:
    adr x0, .str_13
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_14_15:
    adr x0, .str_14
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_15_16:
    adr x0, .str_15
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_16_17:
    adr x0, .str_16
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_17_18:
    adr x0, .str_17
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_18_19:
    adr x0, .str_18
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_19_20:
    adr x0, .str_19
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_20_21:
    adr x0, .str_20
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_21_22:
    adr x0, .str_21
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_22_23:
    adr x0, .str_22
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_23_24:
    adr x0, .str_23
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_24_25:
    adr x0, .str_24
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_25_26:
    adr x0, .str_25
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_26_27:
    adr x0, .str_26
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_27_28:
    adr x0, .str_27
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_28_29:
    adr x0, .str_28
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_29_30:
    adr x0, .str_29
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_30_31:
    adr x0, .str_30
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_31_32:
    adr x0, .str_31
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_32_33:
    adr x0, .str_32
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_33_34:
    adr x0, .str_33
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_34_35:
    adr x0, .str_34
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_35_36:
    adr x0, .str_35
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_36_37:
    adr x0, .str_36
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_37_38:
    adr x0, .str_37
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_38_39:
    adr x0, .str_38
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_39_40:
    adr x0, .str_39
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_40_41:
    adr x0, .str_40
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_41_42:
    adr x0, .str_41
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_42_43:
    adr x0, .str_42
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_43_44:
    adr x0, .str_43
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_44_45:
    adr x0, .str_44
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_45_46:
    adr x0, .str_45
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_46_47:
    adr x0, .str_46
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_47_48:
    adr x0, .str_47
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_48_49:
    adr x0, .str_48
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_49_50:
    adr x0, .str_49
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_50_51:
    adr x0, .str_50
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_51_52:
    adr x0, .str_51
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_52_53:
    adr x0, .str_52
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_53_54:
    adr x0, .str_53
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_54_55:
    adr x0, .str_54
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_55_56:
    adr x0, .str_55
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_56_57:
    adr x0, .str_56
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_57_58:
    adr x0, .str_57
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_58_59:
    adr x0, .str_58
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_59_60:
    adr x0, .str_59
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_60_61:
    adr x0, .str_60
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_61_62:
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_int_to_str
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_62_63:
    adr x0, .str_61
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_float_to_str
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    adr x0, .str_62
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_63_64:
    adr x0, .str_63
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    adr x0, .str_63
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_64_65:
    adr x0, .str_64
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    adr x0, .str_65
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_65_66:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_66_67:
    adr x0, .str_66
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_67_68:
    adr x0, .str_67
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_68_69:
    adr x0, .str_68
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_69_70:
    adr x0, .str_69
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_70_71:
    adr x0, .str_70
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_71_72:
    adr x0, .str_71
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_72_73:
    adr x0, .str_72
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_73_74:
    adr x0, .str_73
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_74_75:
    adr x0, .str_74
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_75_76:
    adr x0, .str_75
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_76_77:
    adr x0, .str_76
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_77_78:
    adr x0, .str_77
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_78_79:
    adr x0, .str_78
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_79_80:
    adr x0, .str_79
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_80_81:
    adr x0, .str_80
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_81_82:
    adr x0, .str_81
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_82_83:
    adr x0, .str_82
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_83_84:
    adr x0, .str_83
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_84_85:
    adr x0, .str_84
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_85_86:
    adr x0, .str_85
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_86_87:
    adr x0, .str_86
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_87_88:
    adr x0, .str_87
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_88_89:
    adr x0, .str_88
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_89_90:
    adr x0, .str_89
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_90_91:
    adr x0, .str_90
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_91_92:
    adr x0, .str_91
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_92_93:
    adr x0, .str_92
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_93_94:
    adr x0, .str_93
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_94_95:
    adr x0, .str_94
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_95_96:
    adr x0, .str_95
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_96_97:
    adr x0, .str_96
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_97_98:
    adr x0, .str_97
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_98_99:
    adr x0, .str_98
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_99_100:
    adr x0, .str_99
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_100_101:
    adr x0, .str_100
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_101_102:
    adr x0, .str_101
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_102_103:
    adr x0, .str_102
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_103_104:
    adr x0, .str_103
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_104_105:
    adr x0, .str_104
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_105_106:
    adr x0, .str_105
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_106_107:
    adr x0, .str_106
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_107_108:
    adr x0, .str_107
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_108_109:
    adr x0, .str_108
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_109_110:
    adr x0, .str_109
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_110_111:
    adr x0, .str_110
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_111_112:
    adr x0, .str_111
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_112_113:
    adr x0, .str_112
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_113_114:
    adr x0, .str_113
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_114_115:
    adr x0, .str_114
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_115_116:
    adr x0, .str_115
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_116_117:
    adr x0, .str_116
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_117_118:
    adr x0, .str_117
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_118_119:
    adr x0, .str_118
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_119_120:
    adr x0, .str_119
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_120_121:
    adr x0, .str_120
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_121_122:
    adr x0, .str_121
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_122_123:
    adr x0, .str_122
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_123_124:
    adr x0, .str_123
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_124_125:
    adr x0, .str_124
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_125_126:
    adr x0, .str_125
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_126_127:
    adr x0, .str_126
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_127_128:
    adr x0, .str_127
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_128_129:
    adr x0, .str_128
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_129_130:
    adr x0, .str_129
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_130_131:
    adr x0, .str_130
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_131_132:
    adr x0, .str_131
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_132_133:
    adr x0, .str_132
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_133_134:
    adr x0, .str_133
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_arm_134_135:
    adr x0, .str_134
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_0
.match_end_0:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_is_word_keyword:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_137
    b .match_arm_1_138
    b .match_arm_2_139
    b .match_arm_3_140
    b .match_arm_4_141
    b .match_arm_5_142
    b .match_arm_6_143
    b .match_arm_7_144
    b .match_arm_8_145
    b .match_arm_9_146
    b .match_arm_10_147
    b .match_arm_11_148
    b .match_arm_12_149
    b .match_arm_13_150
    b .match_arm_14_151
    b .match_arm_15_152
    b .match_arm_16_153
    b .match_arm_17_154
    b .match_arm_18_155
    b .match_arm_19_156
    b .match_arm_20_157
    b .match_arm_21_158
    b .match_arm_22_159
    b .match_arm_23_160
    b .match_arm_24_161
    b .match_arm_25_162
    b .match_arm_26_163
    b .match_arm_27_164
    b .match_arm_28_165
    b .match_arm_29_166
    b .match_arm_30_167
    b .match_arm_31_168
    b .match_arm_32_169
    b .match_arm_33_170
    b .match_arm_34_171
    b .match_arm_35_172
    b .match_arm_36_173
    b .match_arm_37_174
    b .match_arm_38_175
    b .match_arm_39_176
    b .match_arm_40_177
    b .match_arm_41_178
    b .match_arm_42_179
    b .match_arm_43_180
    b .match_arm_44_181
    b .match_arm_45_182
    b .match_arm_46_183
    b .match_arm_47_184
    b .match_arm_48_185
    b .match_arm_49_186
    b .match_arm_50_187
    b .match_arm_51_188
    b .match_arm_52_189
    b .match_arm_53_190
    b .match_arm_54_191
    b .match_arm_55_192
    b .match_arm_56_193
    b .match_arm_57_194
    b .match_arm_58_195
    b .match_arm_59_196
    b .match_arm_60_197
    b .match_arm_61_198
    b .match_arm_62_199
    b .match_arm_63_200
    b .match_arm_64_201
    b .match_arm_65_202
    b .match_arm_66_203
    b .match_arm_67_204
    b .match_arm_68_205
    b .match_arm_69_206
    b .match_arm_70_207
    b .match_arm_71_208
    b .match_arm_72_209
    b .match_arm_73_210
    b .match_arm_74_211
    b .match_arm_75_212
    b .match_arm_76_213
    b .match_arm_77_214
    b .match_arm_78_215
    b .match_arm_79_216
    b .match_arm_80_217
    b .match_arm_81_218
    b .match_arm_82_219
    b .match_arm_83_220
    b .match_arm_84_221
    b .match_arm_85_222
    b .match_arm_86_223
    b .match_arm_87_224
    b .match_arm_88_225
    b .match_arm_89_226
    b .match_arm_90_227
    b .match_arm_91_228
    b .match_arm_92_229
    b .match_arm_93_230
    b .match_arm_94_231
    b .match_arm_95_232
    b .match_arm_96_233
    b .match_arm_97_234
    b .match_arm_98_235
    b .match_arm_99_236
    b .match_arm_100_237
    b .match_arm_101_238
    b .match_arm_102_239
    b .match_arm_103_240
    b .match_arm_104_241
    b .match_arm_105_242
    b .match_arm_106_243
    b .match_arm_107_244
    b .match_arm_108_245
    b .match_arm_109_246
    b .match_arm_110_247
    b .match_arm_111_248
    b .match_arm_112_249
    b .match_arm_113_250
    b .match_arm_114_251
    b .match_arm_115_252
    b .match_arm_116_253
    b .match_arm_117_254
    b .match_arm_118_255
    b .match_arm_119_256
    b .match_arm_120_257
    b .match_arm_121_258
    b .match_arm_122_259
    b .match_arm_123_260
    b .match_arm_124_261
    b .match_arm_125_262
    b .match_arm_126_263
    b .match_arm_127_264
    b .match_arm_128_265
    b .match_arm_129_266
    b .match_arm_130_267
    b .match_arm_131_268
    b .match_arm_132_269
    b .match_arm_133_270
    b .match_arm_134_271
    b .match_end_136
.match_arm_0_137:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_1_138:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_2_139:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_3_140:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_4_141:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_5_142:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_6_143:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_7_144:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_8_145:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_9_146:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_10_147:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_11_148:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_12_149:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_13_150:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_14_151:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_15_152:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_16_153:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_17_154:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_18_155:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_19_156:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_20_157:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_21_158:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_22_159:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_23_160:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_24_161:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_25_162:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_26_163:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_27_164:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_28_165:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_29_166:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_30_167:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_31_168:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_32_169:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_33_170:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_34_171:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_35_172:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_36_173:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_37_174:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_38_175:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_39_176:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_40_177:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_41_178:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_42_179:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_43_180:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_44_181:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_45_182:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_46_183:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_47_184:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_48_185:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_49_186:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_50_187:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_51_188:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_52_189:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_53_190:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_54_191:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_55_192:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_56_193:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_57_194:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_58_195:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_59_196:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_60_197:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_61_198:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_62_199:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_63_200:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_64_201:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_65_202:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_66_203:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_67_204:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_68_205:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_69_206:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_70_207:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_71_208:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_72_209:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_73_210:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_74_211:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_75_212:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_76_213:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_77_214:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_78_215:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_79_216:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_80_217:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_81_218:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_82_219:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_83_220:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_84_221:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_85_222:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_86_223:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_87_224:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_88_225:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_89_226:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_90_227:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_91_228:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_92_229:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_93_230:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_94_231:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_95_232:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_96_233:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_97_234:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_98_235:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_99_236:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_100_237:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_101_238:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_102_239:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_103_240:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_104_241:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_105_242:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_106_243:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_107_244:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_108_245:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_109_246:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_110_247:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_111_248:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_112_249:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_113_250:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_114_251:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_115_252:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_116_253:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_117_254:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_118_255:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_119_256:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_120_257:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_121_258:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_122_259:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_123_260:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_124_261:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_125_262:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_126_263:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_127_264:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_128_265:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_129_266:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_130_267:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_131_268:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_132_269:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_133_270:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_arm_134_271:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_136
.match_end_136:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_token_kind_tag:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_273
    b .match_arm_1_274
    b .match_arm_2_275
    b .match_arm_3_276
    b .match_arm_4_277
    b .match_arm_5_278
    b .match_arm_6_279
    b .match_arm_7_280
    b .match_arm_8_281
    b .match_arm_9_282
    b .match_arm_10_283
    b .match_arm_11_284
    b .match_arm_12_285
    b .match_arm_13_286
    b .match_arm_14_287
    b .match_arm_15_288
    b .match_arm_16_289
    b .match_arm_17_290
    b .match_arm_18_291
    b .match_arm_19_292
    b .match_arm_20_293
    b .match_arm_21_294
    b .match_arm_22_295
    b .match_arm_23_296
    b .match_arm_24_297
    b .match_arm_25_298
    b .match_arm_26_299
    b .match_arm_27_300
    b .match_arm_28_301
    b .match_arm_29_302
    b .match_arm_30_303
    b .match_arm_31_304
    b .match_arm_32_305
    b .match_arm_33_306
    b .match_arm_34_307
    b .match_arm_35_308
    b .match_arm_36_309
    b .match_arm_37_310
    b .match_arm_38_311
    b .match_arm_39_312
    b .match_arm_40_313
    b .match_arm_41_314
    b .match_arm_42_315
    b .match_arm_43_316
    b .match_arm_44_317
    b .match_arm_45_318
    b .match_arm_46_319
    b .match_arm_47_320
    b .match_arm_48_321
    b .match_arm_49_322
    b .match_arm_50_323
    b .match_arm_51_324
    b .match_arm_52_325
    b .match_arm_53_326
    b .match_arm_54_327
    b .match_arm_55_328
    b .match_arm_56_329
    b .match_arm_57_330
    b .match_arm_58_331
    b .match_arm_59_332
    b .match_arm_60_333
    b .match_arm_61_334
    b .match_arm_62_335
    b .match_arm_63_336
    b .match_arm_64_337
    b .match_arm_65_338
    b .match_arm_66_339
    b .match_arm_67_340
    b .match_arm_68_341
    b .match_arm_69_342
    b .match_arm_70_343
    b .match_arm_71_344
    b .match_arm_72_345
    b .match_arm_73_346
    b .match_arm_74_347
    b .match_arm_75_348
    b .match_arm_76_349
    b .match_arm_77_350
    b .match_arm_78_351
    b .match_arm_79_352
    b .match_arm_80_353
    b .match_arm_81_354
    b .match_arm_82_355
    b .match_arm_83_356
    b .match_arm_84_357
    b .match_arm_85_358
    b .match_arm_86_359
    b .match_arm_87_360
    b .match_arm_88_361
    b .match_arm_89_362
    b .match_arm_90_363
    b .match_arm_91_364
    b .match_arm_92_365
    b .match_arm_93_366
    b .match_arm_94_367
    b .match_arm_95_368
    b .match_arm_96_369
    b .match_arm_97_370
    b .match_arm_98_371
    b .match_arm_99_372
    b .match_arm_100_373
    b .match_arm_101_374
    b .match_arm_102_375
    b .match_arm_103_376
    b .match_arm_104_377
    b .match_arm_105_378
    b .match_arm_106_379
    b .match_arm_107_380
    b .match_arm_108_381
    b .match_arm_109_382
    b .match_arm_110_383
    b .match_arm_111_384
    b .match_arm_112_385
    b .match_arm_113_386
    b .match_arm_114_387
    b .match_arm_115_388
    b .match_arm_116_389
    b .match_arm_117_390
    b .match_arm_118_391
    b .match_arm_119_392
    b .match_arm_120_393
    b .match_arm_121_394
    b .match_arm_122_395
    b .match_arm_123_396
    b .match_arm_124_397
    b .match_arm_125_398
    b .match_arm_126_399
    b .match_arm_127_400
    b .match_arm_128_401
    b .match_arm_129_402
    b .match_arm_130_403
    b .match_arm_131_404
    b .match_arm_132_405
    b .match_arm_133_406
    b .match_arm_134_407
    b .match_end_272
.match_arm_0_273:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_1_274:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_2_275:
    mov x0, #2
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_3_276:
    mov x0, #3
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_4_277:
    mov x0, #4
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_5_278:
    mov x0, #5
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_6_279:
    mov x0, #6
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_7_280:
    mov x0, #7
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_8_281:
    mov x0, #8
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_9_282:
    mov x0, #9
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_10_283:
    mov x0, #10
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_11_284:
    mov x0, #11
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_12_285:
    mov x0, #12
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_13_286:
    mov x0, #13
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_14_287:
    mov x0, #14
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_15_288:
    mov x0, #15
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_16_289:
    mov x0, #16
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_17_290:
    mov x0, #17
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_18_291:
    mov x0, #18
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_19_292:
    mov x0, #19
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_20_293:
    mov x0, #20
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_21_294:
    mov x0, #21
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_22_295:
    mov x0, #22
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_23_296:
    mov x0, #23
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_24_297:
    mov x0, #24
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_25_298:
    mov x0, #25
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_26_299:
    mov x0, #26
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_27_300:
    mov x0, #27
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_28_301:
    mov x0, #28
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_29_302:
    mov x0, #29
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_30_303:
    mov x0, #30
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_31_304:
    mov x0, #31
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_32_305:
    mov x0, #32
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_33_306:
    mov x0, #33
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_34_307:
    mov x0, #34
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_35_308:
    mov x0, #35
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_36_309:
    mov x0, #36
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_37_310:
    mov x0, #37
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_38_311:
    mov x0, #38
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_39_312:
    mov x0, #39
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_40_313:
    mov x0, #40
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_41_314:
    mov x0, #41
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_42_315:
    mov x0, #42
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_43_316:
    mov x0, #43
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_44_317:
    mov x0, #44
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_45_318:
    mov x0, #45
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_46_319:
    mov x0, #46
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_47_320:
    mov x0, #47
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_48_321:
    mov x0, #48
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_49_322:
    mov x0, #49
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_50_323:
    mov x0, #50
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_51_324:
    mov x0, #51
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_52_325:
    mov x0, #52
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_53_326:
    mov x0, #53
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_54_327:
    mov x0, #54
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_55_328:
    mov x0, #55
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_56_329:
    mov x0, #56
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_57_330:
    mov x0, #57
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_58_331:
    mov x0, #58
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_59_332:
    mov x0, #59
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_60_333:
    mov x0, #60
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_61_334:
    mov x0, #61
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_62_335:
    mov x0, #62
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_63_336:
    mov x0, #63
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_64_337:
    mov x0, #64
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_65_338:
    mov x0, #65
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_66_339:
    mov x0, #66
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_67_340:
    mov x0, #67
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_68_341:
    mov x0, #68
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_69_342:
    mov x0, #69
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_70_343:
    mov x0, #70
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_71_344:
    mov x0, #71
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_72_345:
    mov x0, #72
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_73_346:
    mov x0, #73
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_74_347:
    mov x0, #74
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_75_348:
    mov x0, #75
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_76_349:
    mov x0, #76
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_77_350:
    mov x0, #77
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_78_351:
    mov x0, #78
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_79_352:
    mov x0, #79
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_80_353:
    mov x0, #80
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_81_354:
    mov x0, #81
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_82_355:
    mov x0, #82
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_83_356:
    mov x0, #83
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_84_357:
    mov x0, #84
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_85_358:
    mov x0, #85
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_86_359:
    mov x0, #86
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_87_360:
    mov x0, #87
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_88_361:
    mov x0, #88
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_89_362:
    mov x0, #89
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_90_363:
    mov x0, #90
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_91_364:
    mov x0, #91
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_92_365:
    mov x0, #92
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_93_366:
    mov x0, #93
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_94_367:
    mov x0, #94
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_95_368:
    mov x0, #95
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_96_369:
    mov x0, #96
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_97_370:
    mov x0, #97
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_98_371:
    mov x0, #98
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_99_372:
    mov x0, #99
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_100_373:
    mov x0, #100
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_101_374:
    mov x0, #101
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_102_375:
    mov x0, #102
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_103_376:
    mov x0, #103
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_104_377:
    mov x0, #104
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_105_378:
    mov x0, #105
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_106_379:
    mov x0, #106
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_107_380:
    mov x0, #107
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_108_381:
    mov x0, #108
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_109_382:
    mov x0, #109
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_110_383:
    mov x0, #110
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_111_384:
    mov x0, #111
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_112_385:
    mov x0, #112
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_113_386:
    mov x0, #113
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_114_387:
    mov x0, #114
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_115_388:
    mov x0, #115
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_116_389:
    mov x0, #116
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_117_390:
    mov x0, #117
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_118_391:
    mov x0, #118
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_119_392:
    mov x0, #119
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_120_393:
    mov x0, #120
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_121_394:
    mov x0, #121
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_122_395:
    mov x0, #122
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_123_396:
    mov x0, #123
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_124_397:
    mov x0, #124
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_125_398:
    mov x0, #125
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_126_399:
    mov x0, #126
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_127_400:
    mov x0, #127
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_128_401:
    mov x0, #128
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_129_402:
    mov x0, #129
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_130_403:
    mov x0, #130
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_131_404:
    mov x0, #131
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_132_405:
    mov x0, #132
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_133_406:
    mov x0, #133
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_arm_134_407:
    mov x0, #134
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_272
.match_end_272:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_token_kind_eq:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_token_kind_tag
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_token_kind_tag
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_println:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    adr x0, .str_135
    mov x1, x0
    mov x2, #64
    mov x0, #1
    mov x8, #64
    svc #0
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_assert:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_408
    adr x0, .str_136
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_panic
    b .endif_409
.else_408:
.endif_409:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_assert_msg:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_410
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_panic
    b .endif_411
.else_410:
.endif_411:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_max:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_412
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_413
.else_412:
.endif_413:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_min:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_414
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_415
.else_414:
.endif_415:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_abs:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_416
    ldr x0, [sp, #0]
    neg x0, x0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_417
.else_416:
.endif_417:
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_clamp:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_418
    ldr x0, [sp, #8]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_419
.else_418:
.endif_419:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_420
    ldr x0, [sp, #16]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_421
.else_420:
.endif_421:
    ldr x0, [sp, #0]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_create_allocator:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_allocator_create
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_allocator_alloc:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_allocator_alloc
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_allocator_free:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_allocator_free
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_allocator_bytes_used:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_allocator_stats
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Vec_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    mov x0, #24
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    bl p_p_malloc
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_Vec_push:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #192
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_422
    mov x0, #8
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_424
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #2
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    str x0, [sp, #64]
    b .endif_425
.else_424:
.endif_425:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    bl p_p_malloc
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
.while_start_426:
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_427
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #96]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #72]
    str x0, [sp, #104]
    ldr x0, [sp, #80]
    str x0, [sp, #112]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #112]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    str x0, [sp, #112]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #120]
    ldr x0, [sp, #80]
    str x0, [sp, #128]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #128]
    add x0, x0, x1
    str x0, [sp, #80]
    b .while_start_426
.while_end_427:
    ldr x0, [sp, #56]
    str x0, [sp, #128]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #128]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_428
    ldr x0, [sp, #56]
    str x0, [sp, #128]
    ldr x0, [sp, #128]
    bl p_p_free
    b .endif_429
.else_428:
.endif_429:
    b .endif_423
.else_422:
.endif_423:
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #128]
    ldr x0, [sp, #40]
    str x0, [sp, #136]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #136]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #128]
    add x0, x0, x1
    str x0, [sp, #128]
    ldr x0, [sp, #128]
    str x0, [sp, #136]
    ldr x0, [sp, #40]
    str x0, [sp, #144]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return

p_Vec_pop:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_430
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_431
.else_430:
.endif_431:
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    str x0, [sp, #48]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #48]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_Vec_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Vec_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Vec_get:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_432
    adr x0, .str_137
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_panic
    b .endif_433
.else_432:
.endif_433:
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_434
    adr x0, .str_137
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_panic
    b .endif_435
.else_434:
.endif_435:
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #56]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_set:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_436
    adr x0, .str_137
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_panic
    b .endif_437
.else_436:
.endif_437:
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_438
    adr x0, .str_137
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_panic
    b .endif_439
.else_438:
.endif_439:
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_clear:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Vec_capacity:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Vec_contains:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_440:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_441
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_442
    mov x0, #1
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    b .endif_443
.else_442:
.endif_443:
    ldr x0, [sp, #48]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_440
.while_end_441:
    mov x0, #0
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_first:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_444
    adr x0, .str_138
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_panic
    b .endif_445
.else_444:
.endif_445:
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_Vec_last:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_446
    adr x0, .str_138
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_panic
    b .endif_447
.else_446:
.endif_447:
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    str x0, [sp, #48]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #48]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_Vec_truncate:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_448
    b .endif_449
.else_448:
.endif_449:
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_Vec_reverse:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_450:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #2
    mov x1, x0
    ldr x0, [sp, #56]
    sdiv x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_451
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #56]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #40]
    str x0, [sp, #96]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_450
.while_end_451:
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_extend:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_452:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_453
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #0]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_Vec_push
    ldr x0, [sp, #48]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_452
.while_end_453:
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_insert:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_454
    adr x0, .str_139
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_panic
    b .endif_455
.else_454:
.endif_455:
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_456
    adr x0, .str_139
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_panic
    b .endif_457
.else_456:
.endif_457:
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_Vec_push
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    sub x0, x0, x1
    str x0, [sp, #56]
.while_start_458:
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .while_end_459
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    ldr x0, [sp, #56]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    sub x0, x0, x1
    str x0, [sp, #80]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #80]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #56]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    sub x0, x0, x1
    str x0, [sp, #56]
    b .while_start_458
.while_end_459:
    ldr x0, [sp, #48]
    str x0, [sp, #104]
    ldr x0, [sp, #8]
    str x0, [sp, #112]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #112]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    str x0, [sp, #112]
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_Vec_remove:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_460
    adr x0, .str_140
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_panic
    b .endif_461
.else_460:
.endif_461:
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_462
    adr x0, .str_140
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_panic
    b .endif_463
.else_462:
.endif_463:
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #56]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
    ldr x0, [sp, #8]
    str x0, [sp, #72]
.while_start_464:
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    ldr x0, [sp, #32]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_465
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #88]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #40]
    str x0, [sp, #88]
    ldr x0, [sp, #72]
    str x0, [sp, #96]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #96]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #72]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #72]
    b .while_start_464
.while_end_465:
    ldr x0, [sp, #32]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    sub x0, x0, x1
    str x0, [sp, #120]
    ldr x0, [sp, #64]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_Vec_clone:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    bl p_Vec_new
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_466:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_467
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_Vec_push
    ldr x0, [sp, #48]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_466
.while_end_467:
    ldr x0, [sp, #40]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    adr x0, .str_141
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_468:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_469
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_470
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    adr x0, .str_142
    str x0, [sp, #64]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_p_str_concat
    str x0, [sp, #40]
    b .endif_471
.else_470:
.endif_471:
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    bl p_p_to_str
    str x0, [sp, #88]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_p_str_concat
    str x0, [sp, #40]
    ldr x0, [sp, #48]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_468
.while_end_469:
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    adr x0, .str_143
    str x0, [sp, #88]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_p_str_concat
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_Vec_sort:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .else_472
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_473
.else_472:
.endif_473:
    mov x0, #1
    str x0, [sp, #40]
.while_start_474:
    ldr x0, [sp, #40]
    cmp x0, #0
    beq .while_end_475
    mov x0, #0
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_476:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_477
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_478
    mov x0, #1
    str x0, [sp, #40]
    b .endif_479
.else_478:
.endif_479:
    ldr x0, [sp, #48]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_476
.while_end_477:
    b .while_start_474
.while_end_475:
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_p_str_hash:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    mov x0, #3750763034362895579
    neg x0, x0
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #32]
.while_start_480:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .while_end_481
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #40]
    eor x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #1099511628211
    mov x1, x0
    ldr x0, [sp, #40]
    mul x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #32]
    b .while_start_480
.while_end_481:
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_482
    ldr x0, [sp, #8]
    neg x0, x0
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_483
.else_482:
.endif_483:
    ldr x0, [sp, #8]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_p_str_equal:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_484
    mov x0, #1
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_485
.else_484:
.endif_485:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_486
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_487
.else_486:
.endif_487:
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_488
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_489
.else_488:
.endif_489:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #56]
.while_start_490:
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #64]
    and x0, x0, x1
    cmp x0, #0
    beq .while_end_491
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_492
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_493
.else_492:
.endif_493:
    ldr x0, [sp, #16]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #56]
    b .while_start_490
.while_end_491:
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_p_str_compare:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_494
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_495
.else_494:
.endif_495:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_496
    mov x0, #1
    neg x0, x0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_497
.else_496:
.endif_497:
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_498
    mov x0, #1
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_499
.else_498:
.endif_499:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #56]
.while_start_500:
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #64]
    and x0, x0, x1
    cmp x0, #0
    beq .while_end_501
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_502
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_503
.else_502:
.endif_503:
    ldr x0, [sp, #16]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #56]
    b .while_start_500
.while_end_501:
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_HashMap_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    mov x0, #32
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    bl p_p_malloc
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_HashMap_find_slot:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_504
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_505
.else_504:
.endif_505:
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_str_hash
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    sdiv x2, x0, x1
    msub x0, x2, x1, x0
    str x0, [sp, #56]
    mov x0, #1
    neg x0, x0
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
.while_start_506:
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_507
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #88]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #0
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_508
    ldr x0, [sp, #64]
    str x0, [sp, #104]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_510
    ldr x0, [sp, #64]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_511
.else_510:
.endif_511:
    ldr x0, [sp, #56]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_509
.else_508:
.endif_509:
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_512
    ldr x0, [sp, #64]
    str x0, [sp, #104]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_514
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    b .endif_515
.else_514:
.endif_515:
    b .endif_513
.else_512:
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    ldr x0, [sp, #8]
    str x0, [sp, #112]
    ldr x1, [sp, #112]
    ldr x0, [sp, #104]
    bl p_p_str_equal
    cmp x0, #0
    beq .else_516
    ldr x0, [sp, #56]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_517
.else_516:
.endif_517:
.endif_513:
    ldr x0, [sp, #56]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #104]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_518
    mov x0, #0
    str x0, [sp, #56]
    b .endif_519
.else_518:
.endif_519:
    ldr x0, [sp, #72]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #72]
    b .while_start_506
.while_end_507:
    ldr x0, [sp, #64]
    str x0, [sp, #104]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_520
    ldr x0, [sp, #64]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_521
.else_520:
.endif_521:
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_HashMap_grow:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #320
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #56]
    mov x0, #8
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_522
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #2
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    str x0, [sp, #64]
    b .endif_523
.else_522:
.endif_523:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    bl p_p_malloc
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #80]
    mul x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    bl p_p_malloc
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
.while_start_524:
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_525
    ldr x0, [sp, #72]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #104]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    ldr x0, [sp, #80]
    str x0, [sp, #112]
    ldr x0, [sp, #88]
    str x0, [sp, #120]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #120]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #112]
    add x0, x0, x1
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    str x0, [sp, #120]
    ldr x0, [sp, #88]
    str x0, [sp, #128]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #128]
    add x0, x0, x1
    str x0, [sp, #88]
    b .while_start_524
.while_end_525:
    mov x0, #0
    str x0, [sp, #128]
    mov x0, #0
    str x0, [sp, #136]
.while_start_526:
    ldr x0, [sp, #136]
    str x0, [sp, #144]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #144]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_527
    ldr x0, [sp, #48]
    str x0, [sp, #144]
    ldr x0, [sp, #136]
    str x0, [sp, #152]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #152]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    ldr x0, [sp, #144]
    str x0, [sp, #152]
    mov x0, #0
    str x0, [sp, #160]
    ldr x0, [sp, #152]
    ldr x0, [x0]
    str x0, [sp, #160]
    ldr x0, [sp, #160]
    str x0, [sp, #168]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #168]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #168]
    ldr x0, [sp, #160]
    str x0, [sp, #176]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #176]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #168]
    and x0, x0, x1
    cmp x0, #0
    beq .else_528
    ldr x0, [sp, #56]
    str x0, [sp, #168]
    ldr x0, [sp, #136]
    str x0, [sp, #176]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #176]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #168]
    add x0, x0, x1
    str x0, [sp, #168]
    ldr x0, [sp, #168]
    str x0, [sp, #176]
    mov x0, #0
    str x0, [sp, #184]
    ldr x0, [sp, #176]
    ldr x0, [x0]
    str x0, [sp, #184]
    ldr x0, [sp, #160]
    str x0, [sp, #192]
    ldr x0, [sp, #192]
    bl p_p_str_hash
    str x0, [sp, #192]
    ldr x0, [sp, #192]
    str x0, [sp, #200]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #200]
    sdiv x2, x0, x1
    msub x0, x2, x1, x0
    str x0, [sp, #200]
    mov x0, #0
    str x0, [sp, #208]
.while_start_530:
    ldr x0, [sp, #208]
    str x0, [sp, #216]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #216]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_531
    ldr x0, [sp, #72]
    str x0, [sp, #216]
    ldr x0, [sp, #200]
    str x0, [sp, #224]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #224]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #216]
    add x0, x0, x1
    str x0, [sp, #216]
    ldr x0, [sp, #216]
    str x0, [sp, #224]
    mov x0, #0
    str x0, [sp, #232]
    ldr x0, [sp, #224]
    ldr x0, [x0]
    str x0, [sp, #232]
    ldr x0, [sp, #232]
    str x0, [sp, #240]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #240]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_532
    ldr x0, [sp, #80]
    str x0, [sp, #240]
    ldr x0, [sp, #200]
    str x0, [sp, #248]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #248]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #240]
    add x0, x0, x1
    str x0, [sp, #240]
    ldr x0, [sp, #240]
    str x0, [sp, #248]
    b .loop_end_534
    b .endif_533
.else_532:
.endif_533:
    ldr x0, [sp, #200]
    str x0, [sp, #256]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #256]
    add x0, x0, x1
    str x0, [sp, #200]
    ldr x0, [sp, #200]
    str x0, [sp, #256]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #256]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_534
    mov x0, #0
    str x0, [sp, #200]
    b .endif_535
.else_534:
.endif_535:
    ldr x0, [sp, #208]
    str x0, [sp, #256]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #256]
    add x0, x0, x1
    str x0, [sp, #208]
    b .while_start_530
.while_end_531:
    ldr x0, [sp, #128]
    str x0, [sp, #256]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #256]
    add x0, x0, x1
    str x0, [sp, #128]
    b .endif_529
.else_528:
.endif_529:
    ldr x0, [sp, #136]
    str x0, [sp, #256]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #256]
    add x0, x0, x1
    str x0, [sp, #136]
    b .while_start_526
.while_end_527:
    ldr x0, [sp, #48]
    str x0, [sp, #256]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #256]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_536
    ldr x0, [sp, #48]
    str x0, [sp, #256]
    ldr x0, [sp, #256]
    bl p_p_free
    b .endif_537
.else_536:
.endif_537:
    ldr x0, [sp, #56]
    str x0, [sp, #256]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #256]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_538
    ldr x0, [sp, #56]
    str x0, [sp, #256]
    ldr x0, [sp, #256]
    bl p_p_free
    b .endif_539
.else_538:
.endif_539:
    add sp, sp, #320
    ldp x29, x30, [sp], #16
    return

p_HashMap_insert:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #224
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    str x0, [sp, #64]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #7
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ge
    mov x1, x0
    ldr x0, [sp, #56]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_540
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    bl p_HashMap_grow
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    b .endif_541
.else_540:
.endif_541:
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_HashMap_find_slot
    str x0, [sp, #56]
    ldr x0, [sp, #0]
    str x0, [sp, #64]
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    ldr x0, [sp, #56]
    str x0, [sp, #104]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #104]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    mov x0, #0
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    str x0, [sp, #120]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #120]
    ldr x0, [sp, #112]
    str x0, [sp, #128]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #128]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #120]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_542
    ldr x0, [sp, #88]
    str x0, [sp, #120]
    ldr x0, [sp, #56]
    str x0, [sp, #128]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #128]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #120]
    ldr x0, [sp, #120]
    str x0, [sp, #128]
    ldr x0, [sp, #40]
    str x0, [sp, #136]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #136]
    add x0, x0, x1
    str x0, [sp, #136]
    b .endif_543
.else_542:
    ldr x0, [sp, #88]
    str x0, [sp, #144]
    ldr x0, [sp, #56]
    str x0, [sp, #152]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #152]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    ldr x0, [sp, #144]
    str x0, [sp, #152]
.endif_543:
    add sp, sp, #224
    ldp x29, x30, [sp], #16
    return

p_HashMap_get:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #192
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_544
    mov x0, #0
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_545
.else_544:
.endif_545:
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x1, [sp, #40]
    ldr x0, [sp, #32]
    bl p_HashMap_find_slot
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #80]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_546
    mov x0, #0
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_547
.else_546:
.endif_547:
    ldr x0, [sp, #0]
    str x0, [sp, #80]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #32]
    str x0, [sp, #104]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #104]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    mov x0, #0
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return

p_HashMap_contains:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_548
    mov x0, #0
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_549
.else_548:
.endif_549:
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x1, [sp, #40]
    ldr x0, [sp, #32]
    bl p_HashMap_find_slot
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #80]
    and x0, x0, x1
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_HashMap_remove:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_550
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_551
.else_550:
.endif_551:
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_HashMap_find_slot
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #88]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_552
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_553
.else_552:
.endif_553:
    mov x0, #0
    str x0, [sp, #88]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #96]
    sub x0, x0, x1
    str x0, [sp, #96]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_HashMap_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_HashMap_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_HashMap_clear:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, gt
    mov x1, x0
    ldr x0, [sp, #48]
    and x0, x0, x1
    cmp x0, #0
    beq .else_554
    mov x0, #0
    str x0, [sp, #48]
.while_start_556:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_557
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_556
.while_end_557:
    b .endif_555
.else_554:
.endif_555:
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_HashMap_keys:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    bl p_Vec_new
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #48]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_558
    ldr x0, [sp, #40]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_559
.else_558:
.endif_559:
    mov x0, #0
    str x0, [sp, #48]
.while_start_560:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_561
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #80]
    and x0, x0, x1
    cmp x0, #0
    beq .else_562
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_Vec_push
    b .endif_563
.else_562:
.endif_563:
    ldr x0, [sp, #48]
    str x0, [sp, #80]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_560
.while_end_561:
    ldr x0, [sp, #40]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    bl p_Vec_sort
    ldr x0, [sp, #40]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_HashMap_values:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_HashMap_keys
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_Vec_len
    str x0, [sp, #16]
    bl p_Vec_new
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
.while_start_564:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_565
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_Vec_get
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_HashMap_get
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_Vec_push
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #32]
    b .while_start_564
.while_end_565:
    ldr x0, [sp, #24]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_HashMap_clone:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #192
    str x0, [sp, #0]
    bl p_HashMap_new
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #64]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_566
    ldr x0, [sp, #8]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_567
.else_566:
.endif_567:
    mov x0, #0
    str x0, [sp, #64]
.while_start_568:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_569
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #80]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, ne
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ne
    mov x1, x0
    ldr x0, [sp, #96]
    and x0, x0, x1
    cmp x0, #0
    beq .else_570
    ldr x0, [sp, #56]
    str x0, [sp, #96]
    ldr x0, [sp, #64]
    str x0, [sp, #104]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #104]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    mov x0, #0
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #8]
    str x0, [sp, #120]
    ldr x0, [sp, #88]
    str x0, [sp, #128]
    ldr x0, [sp, #112]
    str x0, [sp, #136]
    ldr x2, [sp, #136]
    ldr x1, [sp, #128]
    ldr x0, [sp, #120]
    bl p_HashMap_insert
    b .endif_571
.else_570:
.endif_571:
    ldr x0, [sp, #64]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #64]
    b .while_start_568
.while_end_569:
    ldr x0, [sp, #8]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return

p_HashMap_get_or:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #192
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #24
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_572
    ldr x0, [sp, #16]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_573
.else_572:
.endif_573:
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_HashMap_find_slot
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #72]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #1
    neg x0, x0
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #88]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_574
    ldr x0, [sp, #16]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_575
.else_574:
.endif_575:
    ldr x0, [sp, #0]
    str x0, [sp, #88]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    mov x0, #0
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    ldr x0, [sp, #40]
    str x0, [sp, #112]
    mov x0, #8
    mov x1, x0
    ldr x0, [sp, #112]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    str x0, [sp, #112]
    mov x0, #0
    str x0, [sp, #120]
    ldr x0, [sp, #112]
    ldr x0, [x0]
    str x0, [sp, #120]
    ldr x0, [sp, #120]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return

p_HashMap_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_HashMap_keys
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_Vec_len
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
.while_start_576:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_577
    mov x0, #0
    str x0, [sp, #32]
.while_start_578:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_579
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_Vec_get
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_Vec_get
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_p_str_compare
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_580
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    ldr x2, [sp, #72]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_Vec_set
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    ldr x2, [sp, #72]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_Vec_set
    b .endif_581
.else_580:
.endif_581:
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #32]
    b .while_start_578
.while_end_579:
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #24]
    b .while_start_576
.while_end_577:
    adr x0, .str_144
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #24]
.while_start_582:
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_583
    ldr x0, [sp, #24]
    str x0, [sp, #64]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_584
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    adr x0, .str_142
    str x0, [sp, #72]
    ldr x1, [sp, #72]
    ldr x0, [sp, #64]
    bl p_p_str_concat
    str x0, [sp, #56]
    b .endif_585
.else_584:
.endif_585:
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x0, [sp, #24]
    str x0, [sp, #72]
    ldr x1, [sp, #72]
    ldr x0, [sp, #64]
    bl p_Vec_get
    str x0, [sp, #64]
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    ldr x1, [sp, #80]
    ldr x0, [sp, #72]
    bl p_HashMap_get
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    bl p_p_str_from_ptr
    str x0, [sp, #80]
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    ldr x1, [sp, #96]
    ldr x0, [sp, #88]
    bl p_p_str_concat
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    adr x0, .str_145
    str x0, [sp, #96]
    ldr x1, [sp, #96]
    ldr x0, [sp, #88]
    bl p_p_str_concat
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    ldr x0, [sp, #72]
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    bl p_p_to_str
    str x0, [sp, #96]
    ldr x1, [sp, #96]
    ldr x0, [sp, #88]
    bl p_p_str_concat
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #24]
    b .while_start_582
.while_end_583:
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    adr x0, .str_146
    str x0, [sp, #96]
    ldr x1, [sp, #96]
    ldr x0, [sp, #88]
    bl p_p_str_concat
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_p_str_from_ptr:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_586
    mov x0, #1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_malloc
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_587
.else_586:
.endif_587:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
.while_start_588:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .while_end_589
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #48]
    b .while_start_588
.while_end_589:
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    bl p_p_malloc
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
.while_start_590:
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_591
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #72]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    ldr x0, [sp, #64]
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    str x0, [sp, #112]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #112]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #104]
    str x0, [sp, #64]
    b .while_start_590
.while_end_591:
    ldr x0, [sp, #56]
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_p_str_length:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_592
    mov x0, #0
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_593
.else_592:
.endif_593:
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
.while_start_594:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .while_end_595
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    ldr x0, [x0]
    str x0, [sp, #32]
    b .while_start_594
.while_end_595:
    ldr x0, [sp, #24]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_p_str_copy_n:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_596:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_597
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
    ldr x0, [sp, #24]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #32]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_596
.while_end_597:
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_String_from:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_598
    mov x0, #1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_malloc
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    b .endif_599
.else_598:
.endif_599:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_malloc
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    ldr x2, [sp, #56]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_p_str_copy_n
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_String_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_str_length
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_String_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_600
    mov x0, #1
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_601
.else_600:
.endif_601:
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x0, [x0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_String_push_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_malloc
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #16]
    str x0, [sp, #56]
    ldr x2, [sp, #56]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_p_str_copy_n
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    ldr x2, [sp, #56]
    ldr x1, [sp, #48]
    ldr x0, [sp, #40]
    bl p_p_str_copy_n
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_String_contains:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_602
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_603
.else_602:
.endif_603:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_604
    mov x0, #1
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_605
.else_604:
.endif_605:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_606
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_607
.else_606:
.endif_607:
    mov x0, #0
    str x0, [sp, #32]
.while_start_608:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .while_end_609
    mov x0, #1
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_610:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_611
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    mov x0, #0
    str x0, [sp, #96]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_612
    mov x0, #0
    str x0, [sp, #40]
    b .loop_end_614
    b .endif_613
.else_612:
.endif_613:
    ldr x0, [sp, #48]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_610
.while_end_611:
    ldr x0, [sp, #40]
    cmp x0, #0
    beq .else_614
    mov x0, #1
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_615
.else_614:
.endif_615:
    ldr x0, [sp, #32]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #32]
    b .while_start_608
.while_end_609:
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_starts_with:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_616
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_617
.else_616:
.endif_617:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_618
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_619
.else_618:
.endif_619:
    mov x0, #0
    str x0, [sp, #32]
.while_start_620:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_621
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #0
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_622
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_623
.else_622:
.endif_623:
    ldr x0, [sp, #32]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #32]
    b .while_start_620
.while_end_621:
    mov x0, #1
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_String_ends_with:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_624
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_625
.else_624:
.endif_625:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_626
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_627
.else_626:
.endif_627:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_628
    mov x0, #1
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_629
.else_628:
.endif_629:
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    sub x0, x0, x1
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_630:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_631
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_632
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_633
.else_632:
.endif_633:
    ldr x0, [sp, #40]
    str x0, [sp, #96]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_630
.while_end_631:
    mov x0, #1
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_String_trim:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #208
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_634
    mov x0, #1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_malloc
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    add sp, sp, #208
    ldp x29, x30, [sp], #16
    return
    b .endif_635
.else_634:
.endif_635:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
.while_start_636:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #9
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #72]
    orr x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #72]
    orr x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #13
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #72]
    orr x0, x0, x1
    cmp x0, #0
    beq .while_end_637
    ldr x0, [sp, #32]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #48]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
    b .while_start_636
.while_end_637:
    ldr x0, [sp, #32]
    str x0, [sp, #72]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_638
    mov x0, #1
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    bl p_p_malloc
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    add sp, sp, #208
    ldp x29, x30, [sp], #16
    return
    b .endif_639
.else_638:
.endif_639:
    ldr x0, [sp, #0]
    str x0, [sp, #88]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    sub x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #104]
.while_start_640:
    ldr x0, [sp, #104]
    str x0, [sp, #112]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #112]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    mov x0, #9
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #112]
    orr x0, x0, x1
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #112]
    orr x0, x0, x1
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    mov x0, #13
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #112]
    orr x0, x0, x1
    cmp x0, #0
    beq .while_end_641
    ldr x0, [sp, #40]
    str x0, [sp, #112]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #112]
    sub x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #112]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #112]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .else_642
    b .loop_end_644
    b .endif_643
.else_642:
.endif_643:
    ldr x0, [sp, #0]
    str x0, [sp, #112]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #112]
    add x0, x0, x1
    str x0, [sp, #112]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #112]
    sub x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #104]
    b .while_start_640
.while_end_641:
    ldr x0, [sp, #40]
    str x0, [sp, #112]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #112]
    sub x0, x0, x1
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    str x0, [sp, #120]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .else_644
    mov x0, #1
    str x0, [sp, #120]
    ldr x0, [sp, #120]
    bl p_p_malloc
    str x0, [sp, #120]
    ldr x0, [sp, #120]
    str x0, [sp, #128]
    ldr x0, [sp, #120]
    add sp, sp, #208
    ldp x29, x30, [sp], #16
    return
    b .endif_645
.else_644:
.endif_645:
    ldr x0, [sp, #112]
    str x0, [sp, #136]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #136]
    add x0, x0, x1
    str x0, [sp, #136]
    ldr x0, [sp, #136]
    bl p_p_malloc
    str x0, [sp, #136]
    ldr x0, [sp, #136]
    str x0, [sp, #144]
    ldr x0, [sp, #0]
    str x0, [sp, #152]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #152]
    add x0, x0, x1
    str x0, [sp, #152]
    ldr x0, [sp, #112]
    str x0, [sp, #160]
    ldr x2, [sp, #160]
    ldr x1, [sp, #152]
    ldr x0, [sp, #144]
    bl p_p_str_copy_n
    ldr x0, [sp, #136]
    str x0, [sp, #144]
    ldr x0, [sp, #112]
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    ldr x0, [sp, #144]
    str x0, [sp, #152]
    ldr x0, [sp, #136]
    add sp, sp, #208
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #208
    ldp x29, x30, [sp], #16
    return

p_String_to_uppercase:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_646
    mov x0, #1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_malloc
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_647
.else_646:
.endif_647:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_malloc
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_648:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_649
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #97
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #122
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #88]
    and x0, x0, x1
    cmp x0, #0
    beq .else_650
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #88]
    sub x0, x0, x1
    str x0, [sp, #80]
    b .endif_651
.else_650:
.endif_651:
    ldr x0, [sp, #40]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_648
.while_end_649:
    ldr x0, [sp, #32]
    str x0, [sp, #88]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #32]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_to_lowercase:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_652
    mov x0, #1
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_malloc
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_653
.else_652:
.endif_653:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_malloc
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_654:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_655
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #64]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #65
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #90
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #88]
    and x0, x0, x1
    cmp x0, #0
    beq .else_656
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #80]
    b .endif_657
.else_656:
.endif_657:
    ldr x0, [sp, #40]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_654
.while_end_655:
    ldr x0, [sp, #32]
    str x0, [sp, #88]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #32]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_slice:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_658
    mov x0, #1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_malloc
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_659
.else_658:
.endif_659:
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_str_length
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #16]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_660
    mov x0, #0
    str x0, [sp, #48]
    b .endif_661
.else_660:
.endif_661:
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_662
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    b .endif_663
.else_662:
.endif_663:
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_664
    mov x0, #1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    bl p_p_malloc
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_665
.else_664:
.endif_665:
    ldr x0, [sp, #56]
    str x0, [sp, #80]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #80]
    sub x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    bl p_p_malloc
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #0]
    str x0, [sp, #104]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #80]
    str x0, [sp, #112]
    ldr x2, [sp, #112]
    ldr x1, [sp, #104]
    ldr x0, [sp, #96]
    bl p_p_str_copy_n
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #80]
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #96]
    str x0, [sp, #104]
    ldr x0, [sp, #88]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_find:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_666
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_667
.else_666:
.endif_667:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_668
    mov x0, #0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_669
.else_668:
.endif_669:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_670
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_671
.else_670:
.endif_671:
    mov x0, #0
    str x0, [sp, #32]
.while_start_672:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .while_end_673
    mov x0, #1
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_674:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_675
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    mov x0, #0
    str x0, [sp, #96]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_676
    mov x0, #0
    str x0, [sp, #40]
    b .loop_end_678
    b .endif_677
.else_676:
.endif_677:
    ldr x0, [sp, #48]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_674
.while_end_675:
    ldr x0, [sp, #40]
    cmp x0, #0
    beq .else_678
    ldr x0, [sp, #32]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_679
.else_678:
.endif_679:
    ldr x0, [sp, #32]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #32]
    b .while_start_672
.while_end_673:
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_rfind:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #176
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_680
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_681
.else_680:
.endif_681:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_682
    ldr x0, [sp, #16]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_683
.else_682:
.endif_683:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_684
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_685
.else_684:
.endif_685:
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    sub x0, x0, x1
    str x0, [sp, #32]
.while_start_686:
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .while_end_687
    mov x0, #1
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_688:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_689
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
    mov x0, #0
    str x0, [sp, #96]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    ldr x0, [x0]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    mov x1, x0
    ldr x0, [sp, #104]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_690
    mov x0, #0
    str x0, [sp, #40]
    b .loop_end_692
    b .endif_691
.else_690:
.endif_691:
    ldr x0, [sp, #48]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_688
.while_end_689:
    ldr x0, [sp, #40]
    cmp x0, #0
    beq .else_692
    ldr x0, [sp, #32]
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    b .endif_693
.else_692:
.endif_693:
    ldr x0, [sp, #32]
    str x0, [sp, #104]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #104]
    sub x0, x0, x1
    str x0, [sp, #32]
    b .while_start_686
.while_end_687:
    mov x0, #1
    neg x0, x0
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #176
    ldp x29, x30, [sp], #16
    return

p_String_replace:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #272
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_694
    mov x0, #1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_malloc
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    add sp, sp, #272
    ldp x29, x30, [sp], #16
    return
    b .endif_695
.else_694:
.endif_695:
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #40]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_696
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_String_from
    add sp, sp, #272
    ldp x29, x30, [sp], #16
    return
    b .endif_697
.else_696:
.endif_697:
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_str_length
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_698
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_String_from
    add sp, sp, #272
    ldp x29, x30, [sp], #16
    return
    b .endif_699
.else_698:
.endif_699:
    mov x0, #1
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_malloc
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    ldr x0, [sp, #72]
    bl p_p_str_length
    str x0, [sp, #72]
.while_start_700:
    ldr x0, [sp, #64]
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    mov x1, x0
    ldr x0, [sp, #80]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_701
    mov x0, #1
    str x0, [sp, #80]
    mov x0, #0
    str x0, [sp, #88]
.while_start_702:
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_703
    ldr x0, [sp, #64]
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #72]
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_704
    mov x0, #0
    str x0, [sp, #80]
    b .loop_end_706
    b .endif_705
.else_704:
.endif_705:
    ldr x0, [sp, #0]
    str x0, [sp, #96]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #88]
    mov x1, x0
    ldr x0, [sp, #96]
    add x0, x0, x1
    str x0, [sp, #96]
    ldr x0, [sp, #8]
    str x0, [sp, #104]
    ldr x0, [sp, #88]
    mov x1, x0
    ldr x0, [sp, #104]
    add x0, x0, x1
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    mov x0, #0
    str x0, [sp, #128]
    mov x0, #0
    str x0, [sp, #136]
    ldr x0, [sp, #112]
    ldr x0, [x0]
    str x0, [sp, #128]
    ldr x0, [sp, #120]
    ldr x0, [x0]
    str x0, [sp, #136]
    ldr x0, [sp, #128]
    str x0, [sp, #144]
    ldr x0, [sp, #136]
    mov x1, x0
    ldr x0, [sp, #144]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_706
    mov x0, #0
    str x0, [sp, #80]
    b .loop_end_708
    b .endif_707
.else_706:
.endif_707:
    ldr x0, [sp, #88]
    str x0, [sp, #144]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #88]
    b .while_start_702
.while_end_703:
    ldr x0, [sp, #80]
    cmp x0, #0
    beq .else_708
    ldr x0, [sp, #48]
    str x0, [sp, #144]
    ldr x0, [sp, #16]
    str x0, [sp, #152]
    ldr x1, [sp, #152]
    ldr x0, [sp, #144]
    bl p_String_push_str
    str x0, [sp, #48]
    ldr x0, [sp, #64]
    str x0, [sp, #144]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #64]
    b .endif_709
.else_708:
    ldr x0, [sp, #0]
    str x0, [sp, #144]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    ldr x0, [sp, #144]
    str x0, [sp, #152]
    mov x0, #0
    str x0, [sp, #160]
    ldr x0, [sp, #152]
    ldr x0, [x0]
    str x0, [sp, #160]
    mov x0, #2
    str x0, [sp, #168]
    ldr x0, [sp, #168]
    bl p_p_malloc
    str x0, [sp, #168]
    ldr x0, [sp, #168]
    str x0, [sp, #176]
    ldr x0, [sp, #168]
    str x0, [sp, #184]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #184]
    add x0, x0, x1
    str x0, [sp, #184]
    ldr x0, [sp, #184]
    str x0, [sp, #192]
    ldr x0, [sp, #48]
    str x0, [sp, #200]
    ldr x0, [sp, #168]
    str x0, [sp, #208]
    ldr x1, [sp, #208]
    ldr x0, [sp, #200]
    bl p_String_push_str
    str x0, [sp, #48]
    ldr x0, [sp, #168]
    str x0, [sp, #200]
    ldr x0, [sp, #200]
    bl p_p_free
    ldr x0, [sp, #64]
    str x0, [sp, #200]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #200]
    add x0, x0, x1
    str x0, [sp, #64]
.endif_709:
    b .while_start_700
.while_end_701:
    ldr x0, [sp, #48]
    add sp, sp, #272
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #272
    ldp x29, x30, [sp], #16
    return

p_String_split:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #256
    str x0, [sp, #0]
    str x1, [sp, #8]
    bl p_Vec_new
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #24]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_710
    ldr x0, [sp, #16]
    add sp, sp, #256
    ldp x29, x30, [sp], #16
    return
    b .endif_711
.else_710:
.endif_711:
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_str_length
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_712
    ldr x0, [sp, #16]
    add sp, sp, #256
    ldp x29, x30, [sp], #16
    return
    b .endif_713
.else_712:
.endif_713:
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_str_length
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_714:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    str x0, [sp, #64]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .while_end_715
    mov x0, #1
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
.while_start_716:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_717
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #8]
    str x0, [sp, #80]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #0
    str x0, [sp, #104]
    mov x0, #0
    str x0, [sp, #112]
    ldr x0, [sp, #88]
    ldr x0, [x0]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    ldr x0, [sp, #112]
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_718
    mov x0, #0
    str x0, [sp, #56]
    b .loop_end_720
    b .endif_719
.else_718:
.endif_719:
    ldr x0, [sp, #64]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #64]
    b .while_start_716
.while_end_717:
    ldr x0, [sp, #56]
    cmp x0, #0
    beq .else_720
    ldr x0, [sp, #48]
    str x0, [sp, #120]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #120]
    sub x0, x0, x1
    str x0, [sp, #120]
    ldr x0, [sp, #120]
    str x0, [sp, #128]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #128]
    add x0, x0, x1
    str x0, [sp, #128]
    ldr x0, [sp, #128]
    bl p_p_malloc
    str x0, [sp, #128]
    ldr x0, [sp, #128]
    str x0, [sp, #136]
    ldr x0, [sp, #0]
    str x0, [sp, #144]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #144]
    add x0, x0, x1
    str x0, [sp, #144]
    ldr x0, [sp, #120]
    str x0, [sp, #152]
    ldr x2, [sp, #152]
    ldr x1, [sp, #144]
    ldr x0, [sp, #136]
    bl p_p_str_copy_n
    ldr x0, [sp, #128]
    str x0, [sp, #136]
    ldr x0, [sp, #120]
    mov x1, x0
    ldr x0, [sp, #136]
    add x0, x0, x1
    str x0, [sp, #136]
    ldr x0, [sp, #136]
    str x0, [sp, #144]
    ldr x0, [sp, #16]
    str x0, [sp, #152]
    ldr x0, [sp, #128]
    str x0, [sp, #160]
    ldr x1, [sp, #160]
    ldr x0, [sp, #152]
    bl p_Vec_push
    ldr x0, [sp, #48]
    str x0, [sp, #152]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #152]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    b .endif_721
.else_720:
    ldr x0, [sp, #48]
    str x0, [sp, #152]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #152]
    add x0, x0, x1
    str x0, [sp, #48]
.endif_721:
    b .while_start_714
.while_end_715:
    ldr x0, [sp, #40]
    str x0, [sp, #152]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #152]
    sub x0, x0, x1
    str x0, [sp, #152]
    ldr x0, [sp, #152]
    str x0, [sp, #160]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #160]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_722
    ldr x0, [sp, #152]
    str x0, [sp, #160]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #160]
    add x0, x0, x1
    str x0, [sp, #160]
    ldr x0, [sp, #160]
    bl p_p_malloc
    str x0, [sp, #160]
    ldr x0, [sp, #160]
    str x0, [sp, #168]
    ldr x0, [sp, #0]
    str x0, [sp, #176]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #176]
    add x0, x0, x1
    str x0, [sp, #176]
    ldr x0, [sp, #152]
    str x0, [sp, #184]
    ldr x2, [sp, #184]
    ldr x1, [sp, #176]
    ldr x0, [sp, #168]
    bl p_p_str_copy_n
    ldr x0, [sp, #160]
    str x0, [sp, #168]
    ldr x0, [sp, #152]
    mov x1, x0
    ldr x0, [sp, #168]
    add x0, x0, x1
    str x0, [sp, #168]
    ldr x0, [sp, #168]
    str x0, [sp, #176]
    ldr x0, [sp, #16]
    str x0, [sp, #184]
    ldr x0, [sp, #160]
    str x0, [sp, #192]
    ldr x1, [sp, #192]
    ldr x0, [sp, #184]
    bl p_Vec_push
    b .endif_723
.else_722:
.endif_723:
    ldr x0, [sp, #16]
    add sp, sp, #256
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #256
    ldp x29, x30, [sp], #16
    return

p_String_parse_int:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_724
    mov x0, #0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_725
.else_724:
.endif_725:
    mov x0, #0
    str x0, [sp, #8]
    mov x0, #1
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
.while_start_726:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #9
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #48]
    orr x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #48]
    orr x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #13
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #48]
    orr x0, x0, x1
    cmp x0, #0
    beq .while_end_727
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    b .while_start_726
.while_end_727:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #45
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_728
    mov x0, #1
    neg x0, x0
    str x0, [sp, #16]
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    b .endif_729
.else_728:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #43
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_730
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    b .endif_731
.else_730:
.endif_731:
.endif_729:
.while_start_732:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #57
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #48]
    and x0, x0, x1
    cmp x0, #0
    beq .while_end_733
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #48]
    mul x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #56]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    b .while_start_732
.while_end_733:
    ldr x0, [sp, #8]
    str x0, [sp, #48]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #48]
    mul x0, x0, x1
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_String_char_at:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_734
    mov x0, #0
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_735
.else_734:
.endif_735:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_str_length
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, ge
    mov x1, x0
    ldr x0, [sp, #24]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_736
    mov x0, #0
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_737
.else_736:
.endif_737:
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    mov x0, #2
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_p_malloc
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #48]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_String_repeat:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_738
    mov x0, #1
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_malloc
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    b .endif_739
.else_738:
.endif_739:
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_p_str_length
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #40]
    mul x0, x0, x1
    str x0, [sp, #40]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_malloc
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_740:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_741
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #0]
    str x0, [sp, #64]
    ldr x0, [sp, #32]
    str x0, [sp, #72]
    ldr x2, [sp, #72]
    ldr x1, [sp, #64]
    ldr x0, [sp, #56]
    bl p_p_str_copy_n
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #48]
    b .while_start_740
.while_end_741:
    ldr x0, [sp, #40]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #64]
    mul x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_String_to_string:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_from
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_String_clone:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_from
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_String_bytes:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_str_length
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_String_byte_at:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_742
    mov x0, #1
    neg x0, x0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_743
.else_742:
.endif_743:
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_744
    mov x0, #1
    neg x0, x0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_745
.else_744:
.endif_745:
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #16]
    add x0, x0, x1
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x0, [x0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_746
    mov x0, #1
    neg x0, x0
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_747
.else_746:
.endif_747:
    ldr x0, [sp, #32]
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_String_char_count:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_str_length
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_char_to_str
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_is_digit:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #57
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_is_alpha:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #65
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #90
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #97
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    mov x0, #122
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #16]
    and x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #8]
    orr x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_is_whitespace:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #9
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #8]
    orr x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #8]
    orr x0, x0, x1
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #13
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #8]
    orr x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_is_upper:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #65
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #90
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_is_lower:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #97
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #122
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_to_upper:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #97
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #122
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    cmp x0, #0
    beq .else_748
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #8]
    sub x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_749
.else_748:
.endif_749:
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_char_to_lower:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #65
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    mov x0, #90
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #8]
    and x0, x0, x1
    cmp x0, #0
    beq .else_750
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    mov x0, #32
    mov x1, x0
    ldr x0, [sp, #8]
    add x0, x0, x1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_751
.else_750:
.endif_751:
    ldr x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_bytes:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    mov x0, #0
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_?_new
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_String_len
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
.while_start_752:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_753
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x1, [sp, #64]
    bl p_?_push
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #24]
    b .while_start_752
.while_end_753:
    ldr x0, [sp, #8]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_str_from_bytes:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    adr x0, .str_147
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_Vec_len
    str x0, [sp, #24]
.while_start_754:
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_755
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x1, [sp, #48]
    bl p_Vec_get
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_char_to_str
    str x0, [sp, #40]
    ldr x1, [sp, #40]
    ldr x0, [sp, #32]
    bl p_p_str_concat
    str x0, [sp, #8]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #16]
    b .while_start_754
.while_end_755:
    ldr x0, [sp, #8]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_str_slice:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_String_len
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_756
    mov x0, #0
    str x0, [sp, #32]
    b .endif_757
.else_756:
.endif_757:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_758
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    b .endif_759
.else_758:
.endif_759:
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_760
    adr x0, .str_147
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_761
.else_760:
.endif_761:
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
    ldr x0, [sp, #40]
    str x0, [sp, #64]
    ldr x2, [sp, #64]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_String_slice
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_str_starts_with:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_starts_with
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_ends_with:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_ends_with
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_contains:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_contains
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_find:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_find
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_rfind:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_rfind
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_trim_start:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_len
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
.while_start_762:
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_763
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_char_is_whitespace
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_764
    b .loop_end_766
    b .endif_765
.else_764:
.endif_765:
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #16]
    b .while_start_762
.while_end_763:
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_766
    adr x0, .str_147
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_767
.else_766:
.endif_767:
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #16]
    str x0, [sp, #56]
    ldr x0, [sp, #8]
    str x0, [sp, #64]
    ldr x2, [sp, #64]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_String_slice
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_str_trim_end:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #112
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_len
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
.while_start_768:
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .while_end_769
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #24]
    add x0, x0, x1
    str x0, [sp, #24]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #24]
    sub x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    ldr x0, [x0]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    bl p_char_is_whitespace
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_770
    b .loop_end_772
    b .endif_771
.else_770:
.endif_771:
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #48]
    sub x0, x0, x1
    str x0, [sp, #16]
    b .while_start_768
.while_end_769:
    ldr x0, [sp, #16]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .else_772
    adr x0, .str_147
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    b .endif_773
.else_772:
.endif_773:
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #16]
    str x0, [sp, #64]
    ldr x2, [sp, #64]
    ldr x1, [sp, #56]
    ldr x0, [sp, #48]
    bl p_String_slice
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #112
    ldp x29, x30, [sp], #16
    return

p_str_trim:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_trim
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_split:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #192
    str x0, [sp, #0]
    str x1, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_?_new
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_String_len
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_774
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x1, [sp, #32]
    bl p_?_push
    ldr x0, [sp, #16]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    b .endif_775
.else_774:
.endif_775:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_String_len
    str x0, [sp, #32]
    ldr x0, [sp, #8]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_String_len
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
.while_start_776:
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    ldr x0, [sp, #32]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #64]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #56]
    cmp x0, x1
    cset x0, le
    cmp x0, #0
    beq .while_end_777
    mov x0, #1
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
.while_start_778:
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_779
    ldr x0, [sp, #0]
    str x0, [sp, #72]
    ldr x0, [sp, #48]
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #72]
    ldr x0, [sp, #8]
    str x0, [sp, #80]
    ldr x0, [sp, #64]
    mov x1, x0
    ldr x0, [sp, #80]
    add x0, x0, x1
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #0
    str x0, [sp, #104]
    mov x0, #0
    str x0, [sp, #112]
    ldr x0, [sp, #88]
    ldr x0, [x0]
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #112]
    ldr x0, [sp, #104]
    str x0, [sp, #120]
    ldr x0, [sp, #112]
    mov x1, x0
    ldr x0, [sp, #120]
    cmp x0, x1
    cset x0, ne
    cmp x0, #0
    beq .else_780
    mov x0, #0
    str x0, [sp, #56]
    b .loop_end_782
    b .endif_781
.else_780:
.endif_781:
    ldr x0, [sp, #64]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #64]
    b .while_start_778
.while_end_779:
    ldr x0, [sp, #56]
    cmp x0, #0
    beq .else_782
    ldr x0, [sp, #16]
    str x0, [sp, #120]
    ldr x0, [sp, #0]
    str x0, [sp, #128]
    ldr x0, [sp, #24]
    str x0, [sp, #136]
    ldr x0, [sp, #48]
    str x0, [sp, #144]
    ldr x2, [sp, #144]
    ldr x1, [sp, #136]
    ldr x0, [sp, #128]
    bl p_String_slice
    str x0, [sp, #128]
    ldr x0, [sp, #120]
    ldr x1, [sp, #128]
    bl p_?_push
    ldr x0, [sp, #48]
    str x0, [sp, #120]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    str x0, [sp, #48]
    b .endif_783
.else_782:
    ldr x0, [sp, #48]
    str x0, [sp, #120]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #120]
    add x0, x0, x1
    str x0, [sp, #48]
.endif_783:
    b .while_start_776
.while_end_777:
    ldr x0, [sp, #16]
    str x0, [sp, #120]
    ldr x0, [sp, #0]
    str x0, [sp, #128]
    ldr x0, [sp, #24]
    str x0, [sp, #136]
    ldr x0, [sp, #32]
    str x0, [sp, #144]
    ldr x2, [sp, #144]
    ldr x1, [sp, #136]
    ldr x0, [sp, #128]
    bl p_String_slice
    str x0, [sp, #128]
    ldr x0, [sp, #120]
    ldr x1, [sp, #128]
    bl p_?_push
    ldr x0, [sp, #16]
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #192
    ldp x29, x30, [sp], #16
    return

p_str_split_char:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #144
    str x0, [sp, #0]
    str x1, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_?_new
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_String_len
    str x0, [sp, #32]
    mov x0, #0
    str x0, [sp, #40]
.while_start_784:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_785
    ldr x0, [sp, #0]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #48]
    str x0, [sp, #56]
    mov x0, #0
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x0, [x0]
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #72]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_786
    ldr x0, [sp, #16]
    str x0, [sp, #72]
    ldr x0, [sp, #0]
    str x0, [sp, #80]
    ldr x0, [sp, #24]
    str x0, [sp, #88]
    ldr x0, [sp, #40]
    str x0, [sp, #96]
    ldr x2, [sp, #96]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_String_slice
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x1, [sp, #80]
    bl p_?_push
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #24]
    b .endif_787
.else_786:
.endif_787:
    ldr x0, [sp, #40]
    str x0, [sp, #72]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #72]
    add x0, x0, x1
    str x0, [sp, #40]
    b .while_start_784
.while_end_785:
    ldr x0, [sp, #16]
    str x0, [sp, #72]
    ldr x0, [sp, #0]
    str x0, [sp, #80]
    ldr x0, [sp, #24]
    str x0, [sp, #88]
    ldr x0, [sp, #32]
    str x0, [sp, #96]
    ldr x2, [sp, #96]
    ldr x1, [sp, #88]
    ldr x0, [sp, #80]
    bl p_String_slice
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x1, [sp, #80]
    bl p_?_push
    ldr x0, [sp, #16]
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #144
    ldp x29, x30, [sp], #16
    return

p_str_replace:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    ldr x2, [sp, #40]
    ldr x1, [sp, #32]
    ldr x0, [sp, #24]
    bl p_String_replace
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_str_repeat:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_repeat
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_to_upper:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_to_uppercase
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_to_lower:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_to_lowercase
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_parse_int:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_parse_int
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_parse_hex:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #240
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_len
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_788
    mov x0, #0
    add sp, sp, #240
    ldp x29, x30, [sp], #16
    return
    b .endif_789
.else_788:
.endif_789:
    mov x0, #0
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
    mov x0, #0
    str x0, [sp, #32]
.while_start_790:
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #40]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_791
    ldr x0, [sp, #0]
    str x0, [sp, #40]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #40]
    add x0, x0, x1
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    mov x0, #0
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    ldr x0, [x0]
    str x0, [sp, #56]
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    bl p_char_is_whitespace
    cmp x0, #0
    cset x0, eq
    cmp x0, #0
    beq .else_792
    b .loop_end_794
    b .endif_793
.else_792:
.endif_793:
    ldr x0, [sp, #16]
    str x0, [sp, #64]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #16]
    b .while_start_790
.while_end_791:
    ldr x0, [sp, #16]
    str x0, [sp, #64]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_794
    mov x0, #0
    add sp, sp, #240
    ldp x29, x30, [sp], #16
    return
    b .endif_795
.else_794:
.endif_795:
    ldr x0, [sp, #0]
    str x0, [sp, #64]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #45
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_796
    mov x0, #1
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #16]
    b .endif_797
.else_796:
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #43
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, eq
    cmp x0, #0
    beq .else_798
    ldr x0, [sp, #16]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #16]
    b .endif_799
.else_798:
.endif_799:
.endif_797:
    ldr x0, [sp, #16]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_800
    ldr x0, [sp, #0]
    str x0, [sp, #88]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #88]
    str x0, [sp, #96]
    mov x0, #0
    str x0, [sp, #104]
    ldr x0, [sp, #96]
    ldr x0, [x0]
    str x0, [sp, #104]
    ldr x0, [sp, #0]
    str x0, [sp, #112]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #112]
    add x0, x0, x1
    str x0, [sp, #112]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #112]
    add x0, x0, x1
    str x0, [sp, #112]
    ldr x0, [sp, #112]
    str x0, [sp, #120]
    mov x0, #0
    str x0, [sp, #128]
    ldr x0, [sp, #120]
    ldr x0, [x0]
    str x0, [sp, #128]
    ldr x0, [sp, #104]
    str x0, [sp, #136]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #136]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #136]
    ldr x0, [sp, #128]
    str x0, [sp, #144]
    mov x0, #120
    mov x1, x0
    ldr x0, [sp, #144]
    cmp x0, x1
    cset x0, eq
    str x0, [sp, #144]
    ldr x0, [sp, #128]
    str x0, [sp, #152]
    mov x0, #88
    mov x1, x0
    ldr x0, [sp, #152]
    cmp x0, x1
    cset x0, eq
    mov x1, x0
    ldr x0, [sp, #144]
    orr x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #136]
    and x0, x0, x1
    cmp x0, #0
    beq .else_802
    ldr x0, [sp, #16]
    str x0, [sp, #136]
    mov x0, #2
    mov x1, x0
    ldr x0, [sp, #136]
    add x0, x0, x1
    str x0, [sp, #16]
    b .endif_803
.else_802:
.endif_803:
    b .endif_801
.else_800:
.endif_801:
.while_start_804:
    ldr x0, [sp, #16]
    str x0, [sp, #136]
    ldr x0, [sp, #8]
    mov x1, x0
    ldr x0, [sp, #136]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_805
    ldr x0, [sp, #0]
    str x0, [sp, #136]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #136]
    add x0, x0, x1
    str x0, [sp, #136]
    ldr x0, [sp, #136]
    str x0, [sp, #144]
    mov x0, #0
    str x0, [sp, #152]
    ldr x0, [sp, #144]
    ldr x0, [x0]
    str x0, [sp, #152]
    mov x0, #1
    neg x0, x0
    str x0, [sp, #160]
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #168]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #168]
    ldr x0, [sp, #152]
    str x0, [sp, #176]
    mov x0, #57
    mov x1, x0
    ldr x0, [sp, #176]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #168]
    and x0, x0, x1
    cmp x0, #0
    beq .else_806
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #168]
    sub x0, x0, x1
    str x0, [sp, #160]
    b .endif_807
.else_806:
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #65
    mov x1, x0
    ldr x0, [sp, #168]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #168]
    ldr x0, [sp, #152]
    str x0, [sp, #176]
    mov x0, #70
    mov x1, x0
    ldr x0, [sp, #176]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #168]
    and x0, x0, x1
    cmp x0, #0
    beq .else_808
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #55
    mov x1, x0
    ldr x0, [sp, #168]
    sub x0, x0, x1
    str x0, [sp, #160]
    b .endif_809
.else_808:
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #97
    mov x1, x0
    ldr x0, [sp, #168]
    cmp x0, x1
    cset x0, ge
    str x0, [sp, #168]
    ldr x0, [sp, #152]
    str x0, [sp, #176]
    mov x0, #102
    mov x1, x0
    ldr x0, [sp, #176]
    cmp x0, x1
    cset x0, le
    mov x1, x0
    ldr x0, [sp, #168]
    and x0, x0, x1
    cmp x0, #0
    beq .else_810
    ldr x0, [sp, #152]
    str x0, [sp, #168]
    mov x0, #87
    mov x1, x0
    ldr x0, [sp, #168]
    sub x0, x0, x1
    str x0, [sp, #160]
    b .endif_811
.else_810:
    b .loop_end_812
.endif_811:
.endif_809:
.endif_807:
    ldr x0, [sp, #24]
    str x0, [sp, #168]
    mov x0, #16
    mov x1, x0
    ldr x0, [sp, #168]
    mul x0, x0, x1
    str x0, [sp, #168]
    ldr x0, [sp, #160]
    mov x1, x0
    ldr x0, [sp, #168]
    add x0, x0, x1
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #168]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #168]
    add x0, x0, x1
    str x0, [sp, #16]
    b .while_start_804
.while_end_805:
    ldr x0, [sp, #32]
    cmp x0, #0
    beq .else_812
    mov x0, #0
    str x0, [sp, #168]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #168]
    sub x0, x0, x1
    str x0, [sp, #24]
    b .endif_813
.else_812:
.endif_813:
    ldr x0, [sp, #24]
    add sp, sp, #240
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #240
    ldp x29, x30, [sp], #16
    return

p_str_parse_int_range:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #160
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_String_len
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .else_814
    mov x0, #0
    str x0, [sp, #32]
    b .endif_815
.else_814:
.endif_815:
    ldr x0, [sp, #40]
    str x0, [sp, #48]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, gt
    cmp x0, #0
    beq .else_816
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    b .endif_817
.else_816:
.endif_817:
    ldr x0, [sp, #32]
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #48]
    cmp x0, x1
    cset x0, ge
    cmp x0, #0
    beq .else_818
    mov x0, #0
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    b .endif_819
.else_818:
.endif_819:
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #32]
    str x0, [sp, #56]
.while_start_820:
    ldr x0, [sp, #56]
    str x0, [sp, #64]
    ldr x0, [sp, #40]
    mov x1, x0
    ldr x0, [sp, #64]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_821
    ldr x0, [sp, #0]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    mov x1, x0
    ldr x0, [sp, #64]
    add x0, x0, x1
    str x0, [sp, #64]
    ldr x0, [sp, #64]
    str x0, [sp, #72]
    mov x0, #0
    str x0, [sp, #80]
    ldr x0, [sp, #72]
    ldr x0, [x0]
    str x0, [sp, #80]
    ldr x0, [sp, #80]
    str x0, [sp, #88]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #88]
    cmp x0, x1
    cset x0, lt
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #57
    mov x1, x0
    ldr x0, [sp, #96]
    cmp x0, x1
    cset x0, gt
    mov x1, x0
    ldr x0, [sp, #88]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_822
    b .loop_end_824
    b .endif_823
.else_822:
.endif_823:
    ldr x0, [sp, #48]
    str x0, [sp, #88]
    mov x0, #10
    mov x1, x0
    ldr x0, [sp, #88]
    mul x0, x0, x1
    str x0, [sp, #88]
    ldr x0, [sp, #80]
    str x0, [sp, #96]
    mov x0, #48
    mov x1, x0
    ldr x0, [sp, #96]
    sub x0, x0, x1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #48]
    ldr x0, [sp, #56]
    str x0, [sp, #88]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #88]
    add x0, x0, x1
    str x0, [sp, #56]
    b .while_start_820
.while_end_821:
    ldr x0, [sp, #48]
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #160
    ldp x29, x30, [sp], #16
    return

p_str_byte_at:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_String_byte_at
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_len
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_String_is_empty
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_concat:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_str_concat
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_compare:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_str_compare
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_equal:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x1, [sp, #24]
    ldr x0, [sp, #16]
    bl p_p_str_equal
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_int_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_to_str
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_float_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_float_to_str
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_str_parse_float:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_str_to_float
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_byte_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_p_char_to_str
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_is_some:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_825
    b .match_arm_1_826
    b .match_end_824
.match_arm_0_825:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_824
.match_arm_1_826:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_824
.match_end_824:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_is_none:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_828
    b .match_arm_1_829
    b .match_end_827
.match_arm_0_828:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_827
.match_arm_1_829:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_827
.match_end_827:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_unwrap:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_831
    b .match_arm_1_832
    b .match_end_830
.match_arm_0_831:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_830
.match_arm_1_832:
    adr x0, .str_148
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_panic
    b .match_end_830
.match_end_830:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_unwrap_or:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_834
    b .match_arm_1_835
    b .match_end_833
.match_arm_0_834:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_833
.match_arm_1_835:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_833
.match_end_833:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_map:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_837
    b .match_arm_1_838
    b .match_end_836
.match_arm_0_837:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_f
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_836
.match_arm_1_838:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_836
.match_end_836:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_and_then:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_840
    b .match_arm_1_841
    b .match_end_839
.match_arm_0_840:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_f
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_839
.match_arm_1_841:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_839
.match_end_839:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_or_else:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_843
    b .match_arm_1_844
    b .match_end_842
.match_arm_0_843:
    mov x0, #0
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_842
.match_arm_1_844:
    bl p_default
    str x0, [sp, #32]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_842
.match_end_842:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_unwrap_or_else:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_846
    b .match_arm_1_847
    b .match_end_845
.match_arm_0_846:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_845
.match_arm_1_847:
    bl p_default
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_845
.match_end_845:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Option_expect:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_849
    b .match_arm_1_850
    b .match_end_848
.match_arm_0_849:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_848
.match_arm_1_850:
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_panic
    b .match_end_848
.match_end_848:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_is_ok:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_852
    b .match_arm_1_853
    b .match_end_851
.match_arm_0_852:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_851
.match_arm_1_853:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_851
.match_end_851:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_is_err:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_855
    b .match_arm_1_856
    b .match_end_854
.match_arm_0_855:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_854
.match_arm_1_856:
    mov x0, #1
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_854
.match_end_854:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_unwrap:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_858
    b .match_arm_1_859
    b .match_end_857
.match_arm_0_858:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_857
.match_arm_1_859:
    adr x0, .str_149
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_p_panic
    b .match_end_857
.match_end_857:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_unwrap_or:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_861
    b .match_arm_1_862
    b .match_end_860
.match_arm_0_861:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_860
.match_arm_1_862:
    ldr x0, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_860
.match_end_860:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_ok:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_864
    b .match_arm_1_865
    b .match_end_863
.match_arm_0_864:
    mov x0, #0
    str x0, [sp, #16]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_863
.match_arm_1_865:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_863
.match_end_863:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_err:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    str x0, [sp, #8]
    b .match_arm_0_867
    b .match_arm_1_868
    b .match_end_866
.match_arm_0_867:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_866
.match_arm_1_868:
    mov x0, #0
    str x0, [sp, #16]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_866
.match_end_866:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_map:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_870
    b .match_arm_1_871
    b .match_end_869
.match_arm_0_870:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_f
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_869
.match_arm_1_871:
    mov x0, #0
    str x0, [sp, #32]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_869
.match_end_869:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_map_err:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_873
    b .match_arm_1_874
    b .match_end_872
.match_arm_0_873:
    mov x0, #0
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_872
.match_arm_1_874:
    mov x0, #0
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    bl p_f
    str x0, [sp, #32]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_872
.match_end_872:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_and_then:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_876
    b .match_arm_1_877
    b .match_end_875
.match_arm_0_876:
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_f
    str x0, [sp, #24]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_875
.match_arm_1_877:
    mov x0, #0
    str x0, [sp, #32]
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_875
.match_end_875:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_unwrap_or_else:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_879
    b .match_arm_1_880
    b .match_end_878
.match_arm_0_879:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_878
.match_arm_1_880:
    bl p_default
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_878
.match_end_878:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_Result_expect:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    b .match_arm_0_882
    b .match_arm_1_883
    b .match_end_881
.match_arm_0_882:
    mov x0, #0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .match_end_881
.match_arm_1_883:
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_p_panic
    b .match_end_881
.match_end_881:
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #64
    adr x0, .str_147
    str x0, [sp, #0]
    mov x0, #0
    str x0, [sp, #8]
    add x0, sp, #0
    add sp, sp, #64
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #64
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_append_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_append_char:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_append_int:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_StringBuilder_clear:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_new:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    mov x0, #0
    str x0, [sp, #24]
    ldr x0, [sp, #24]
    bl p_?_new
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    add x0, sp, #8
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_from_string:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    mov x0, #0
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    ldr x1, [sp, #16]
    bl p_?_new
    str x0, [sp, #8]
    ldr x0, [sp, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #16]
    bl p_String_len
    str x0, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
.while_start_884:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_885
    ldr x0, [sp, #0]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #8]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x1, [sp, #64]
    bl p_?_write_byte
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #24]
    b .while_start_884
.while_end_885:
    ldr x0, [sp, #8]
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_write_byte:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x1, [sp, #24]
    bl p_?_push
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_write_bytes:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #128
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    mov x0, #0
    str x0, [sp, #24]
.while_start_886:
    ldr x0, [sp, #24]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_887
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #32]
    ldr x0, [sp, #32]
    str x0, [sp, #40]
    mov x0, #0
    str x0, [sp, #48]
    ldr x0, [sp, #40]
    ldr x0, [x0]
    str x0, [sp, #48]
    ldr x0, [sp, #0]
    str x0, [sp, #56]
    ldr x0, [sp, #48]
    str x0, [sp, #64]
    ldr x0, [sp, #56]
    ldr x1, [sp, #64]
    bl p_ByteBuffer_write_byte
    ldr x0, [sp, #24]
    str x0, [sp, #56]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #56]
    add x0, x0, x1
    str x0, [sp, #24]
    b .while_start_886
.while_end_887:
    add sp, sp, #128
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_read_byte:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    str x1, [sp, #8]
    ldr x0, [sp, #8]
    str x0, [sp, #16]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #16]
    cmp x0, x1
    cset x0, lt
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, ge
    mov x1, x0
    ldr x0, [sp, #16]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_888
    mov x0, #1
    neg x0, x0
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    b .endif_889
.else_888:
.endif_889:
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    str x0, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    ldr x1, [sp, #24]
    bl p_?_get
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_set_byte:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    str x1, [sp, #8]
    str x2, [sp, #16]
    ldr x0, [sp, #8]
    str x0, [sp, #24]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    mov x1, x0
    ldr x0, [sp, #32]
    cmp x0, x1
    cset x0, ge
    mov x1, x0
    ldr x0, [sp, #24]
    orr x0, x0, x1
    cmp x0, #0
    beq .else_890
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    b .endif_891
.else_890:
.endif_891:
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #16]
    str x0, [sp, #40]
    ldr x0, [sp, #24]
    ldr x1, [sp, #32]
    ldr x2, [sp, #40]
    bl p_?_set
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_len:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_is_empty:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    str x0, [sp, #8]
    mov x0, #0
    mov x1, x0
    ldr x0, [sp, #8]
    cmp x0, x1
    cset x0, eq
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_clear:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #80
    str x0, [sp, #0]
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    str x0, [sp, #8]
    ldr x0, [sp, #8]
    bl p_?_clear
    add sp, sp, #80
    ldp x29, x30, [sp], #16
    return

p_ByteBuffer_to_str:
    stp x29, x30, [sp, #-16]!
    mov x29, sp
    sub sp, sp, #96
    str x0, [sp, #0]
    adr x0, .str_147
    str x0, [sp, #8]
    mov x0, #0
    str x0, [sp, #16]
.while_start_892:
    ldr x0, [sp, #16]
    str x0, [sp, #24]
    ldr x0, [sp, #0]
    ldr x0, [x0, #8]
    mov x1, x0
    ldr x0, [sp, #24]
    cmp x0, x1
    cset x0, lt
    cmp x0, #0
    beq .while_end_893
    ldr x0, [sp, #0]
    ldr x0, [x0, #0]
    str x0, [sp, #24]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    ldr x1, [sp, #32]
    bl p_?_get
    str x0, [sp, #24]
    ldr x0, [sp, #8]
    str x0, [sp, #32]
    ldr x0, [sp, #24]
    str x0, [sp, #40]
    ldr x0, [sp, #40]
    bl p_p_char_to_str
    str x0, [sp, #40]
    ldr x1, [sp, #40]
    ldr x0, [sp, #32]
    bl p_p_str_concat
    str x0, [sp, #8]
    ldr x0, [sp, #16]
    str x0, [sp, #32]
    mov x0, #1
    mov x1, x0
    ldr x0, [sp, #32]
    add x0, x0, x1
    str x0, [sp, #16]
    b .while_start_892
.while_end_893:
    ldr x0, [sp, #8]
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return
    add sp, sp, #96
    ldp x29, x30, [sp], #16
    return


.section .rodata
QuestionQuestion:
    .asciz ".str_125"
Assertion failed:
    .asciz ".str_136"
PipeArrow:
    .asciz ".str_99"
Vec is empty:
    .asciz ".str_138"
F64:
    .asciz ".str_78"
Star:
    .asciz ".str_84"
val:
    .asciz ".str_1"
ensure:
    .asciz ".str_6"
if:
    .asciz ".str_10"
extern:
    .asciz ".str_46"
component:
    .asciz ".str_56"
state:
    .asciz ".str_59"
Tilde:
    .asciz ".str_101"
PercentEq:
    .asciz ".str_111"
And:
    .asciz ".str_95"
F32:
    .asciz ".str_77"
QuestionDot:
    .asciz ".str_126"
I128:
    .asciz ".str_71"
type:
    .asciz ".str_8"
with:
    .asciz ".str_20"
comptime:
    .asciz ".str_23"
Shl:
    .asciz ".str_102"
PlusEq:
    .asciz ".str_107"
FatArrow:
    .asciz ".str_118"
Void:
    .asciz ".str_81"
Comma:
    .asciz ".str_120"
Vec insert index out of bounds:
    .asciz ".str_139"
U128:
    .asciz ".str_76"
in:
    .asciz ".str_29"
Or:
    .asciz ".str_97"
system:
    .asciz ".str_57"
Bool:
    .asciz ".str_79"
Arrow:
    .asciz ".str_117"
}:
    .asciz ".str_146"
RParen:
    .asciz ".str_132"
Bang:
    .asciz ".str_94"
LParen:
    .asciz ".str_131"
match:
    .asciz ".str_12"

:
    .asciz ".str_135"
PipeEq:
    .asciz ".str_113"
dyn:
    .asciz ".str_31"
enum:
    .asciz ".str_15"
CaretEq:
    .asciz ".str_114"
await:
    .asciz ".str_22"
var:
    .asciz ".str_2"
interrupt:
    .asciz ".str_45"
Caret:
    .asciz ".str_100"
LBrace:
    .asciz ".str_133"
]:
    .asciz ".str_143"
persist:
    .asciz ".str_37"
ShrEq:
    .asciz ".str_116"
":
    .asciz ".str_63"
Ampersand:
    .asciz ".str_96"
[:
    .asciz ".str_141"
Str:
    .asciz ".str_80"
use:
    .asciz ".str_18"
region:
    .asciz ".str_33"
struct:
    .asciz ".str_14"
sandbox:
    .asciz ".str_34"
for:
    .asciz ".str_38"
Pipe:
    .asciz ".str_98"
I8:
    .asciz ".str_67"
kernel:
    .asciz ".str_55"
I16:
    .asciz ".str_68"
RBracket:
    .asciz ".str_130"
, :
    .asciz ".str_142"
Slash:
    .asciz ".str_85"
Shr:
    .asciz ".str_103"
fn:
    .asciz ".str_0"
Question:
    .asciz ".str_124"
: :
    .asciz ".str_145"
EqEq:
    .asciz ".str_88"
Semicolon:
    .asciz ".str_122"
backward:
    .asciz ".str_54"
FloatLit(:
    .asciz ".str_61"
Eq:
    .asciz ".str_87"
impl:
    .asciz ".str_16"
where:
    .asciz ".str_32"
Gt:
    .asciz ".str_91"
const:
    .asciz ".str_4"
loop:
    .asciz ".str_40"
Hash:
    .asciz ".str_127"
At:
    .asciz ".str_128"
true:
    .asciz ".str_49"
model:
    .asciz ".str_52"
RBrace:
    .asciz ".str_134"
GtEq:
    .asciz ".str_93"
DoubleColon:
    .asciz ".str_123"
EOF:
    .asciz ".str_66"
guard:
    .asciz ".str_47"
module:
    .asciz ".str_19"
do:
    .asciz ".str_44"
MinusEq:
    .asciz ".str_108"
layer:
    .asciz ".str_51"
else:
    .asciz ".str_11"
given:
    .asciz ".str_5"
static:
    .asciz ".str_27"
mut:
    .asciz ".str_3"
called unwrap on None:
    .asciz ".str_148"
):
    .asciz ".str_62"
select:
    .asciz ".str_13"
called unwrap on Err:
    .asciz ".str_149"
U8:
    .asciz ".str_72"
unsafe:
    .asciz ".str_35"
async:
    .asciz ".str_21"
FString(":
    .asciz ".str_64"
return:
    .asciz ".str_9"
U32:
    .asciz ".str_74"
I32:
    .asciz ".str_69"
U64:
    .asciz ".str_75"
SlashEq:
    .asciz ".str_110"
Vec remove index out of bounds:
    .asciz ".str_140"
StarEq:
    .asciz ".str_109"
LBracket:
    .asciz ".str_129"
grad:
    .asciz ".str_53"
BangEq:
    .asciz ".str_89"
Percent:
    .asciz ".str_86"
DotDot:
    .asciz ".str_104"
break:
    .asciz ".str_41"
on:
    .asciz ".str_60"
txn:
    .asciz ".str_36"
Lt:
    .asciz ".str_90"
"):
    .asciz ".str_65"
while:
    .asciz ".str_39"
move:
    .asciz ".str_26"
cap:
    .asciz ".str_7"
continue:
    .asciz ".str_42"
query:
    .asciz ".str_58"
DotDotEq:
    .asciz ".str_105"
pub:
    .asciz ".str_17"
Dot:
    .asciz ".str_119"
Plus:
    .asciz ".str_82"
try:
    .asciz ".str_28"
LtEq:
    .asciz ".str_92"
is:
    .asciz ".str_48"
Colon:
    .asciz ".str_121"
Vec index out of bounds:
    .asciz ".str_137"
TildeArrow:
    .asciz ".str_106"
:
    .asciz ".str_147"
spawn:
    .asciz ".str_25"
handle:
    .asciz ".str_43"
{:
    .asciz ".str_144"
false:
    .asciz ".str_50"
U16:
    .asciz ".str_73"
Minus:
    .asciz ".str_83"
I64:
    .asciz ".str_70"
AmpEq:
    .asciz ".str_112"
as:
    .asciz ".str_30"
ShlEq:
    .asciz ".str_115"
defer:
    .asciz ".str_24"
