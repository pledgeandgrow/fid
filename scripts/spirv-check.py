#!/usr/bin/env python3
"""Structural check of a SPIR-V module written by `fid gpu --target spirv`.

    fid gpu kernel.fd --target spirv -o kernel.spv
    python3 scripts/spirv-check.py kernel.spv

Checks (no SPIR-V tools needed; `spirv-val` checks much more):
  - magic, version 1.0, bound > every id defined
  - every instruction's word count walks exactly to the end of the module
  - module layout order: OpCapability, OpMemoryModel, OpEntryPoint,
    OpExecutionMode, types/constants, then functions
  - ids are defined once; branch targets are OpLabel ids
  - each block ends with a terminator, each OpFunction with OpFunctionEnd,
    and OpVariable only at the start of a function's first block

Exit status 0 when every check passes, 1 otherwise (each problem printed).
"""

import struct
import sys

CAPABILITY, MEMORY_MODEL, ENTRY_POINT, EXECUTION_MODE = 17, 14, 15, 16
TYPES = {19, 20, 21, 22, 23, 32, 33}
CONSTANTS = {41, 42, 43}
FUNCTION, FUNCTION_END, VARIABLE, LABEL = 54, 56, 59, 248
BRANCH, BRANCH_COND = 249, 250
TERMINATORS = {249, 250, 251, 252, 253, 254, 255}
# Opcodes whose result id is operand 0 (types) or operand 1 (typed results)
RESULT_AT_0 = TYPES | {LABEL}
RESULT_AT_1 = CONSTANTS | {FUNCTION, VARIABLE, 61, 126, 128, 130, 132, 135,
                           138, 169, 170, 171, 173, 175, 177, 179, 195, 196,
                           197, 198, 199, 200}


def main(path):
    data = open(path, "rb").read()
    problems = []
    if len(data) % 4 != 0 or len(data) < 20:
        print("not a whole number of words, or shorter than the header")
        return 1
    words = list(struct.unpack("<%dI" % (len(data) // 4), data))
    if words[0] != 0x07230203:
        problems.append("bad magic %#x" % words[0])
    if words[1] != 0x00010000:
        problems.append("version %#x, expected 1.0" % words[1])
    bound = words[3]

    defined = {}
    labels = set()
    branch_targets = []
    # layout section: 0 caps, 1 memory model, 2 entry points, 3 modes,
    # 4 types/constants, 5 functions
    section = 0
    in_function = False
    in_block = False
    first_block = False
    block_has_code = False
    i = 5
    while i < len(words):
        count = words[i] >> 16
        op = words[i] & 0xFFFF
        if count == 0 or i + count > len(words):
            problems.append("word %d: word count %d runs past the end" % (i, count))
            break
        ops = words[i + 1:i + count]
        where = "word %d (op %d)" % (i, op)

        order = None
        if op == CAPABILITY:
            order = 0
        elif op == MEMORY_MODEL:
            order = 1
        elif op == ENTRY_POINT:
            order = 2
        elif op == EXECUTION_MODE:
            order = 3
        elif op in TYPES or op in CONSTANTS:
            order = 4
        elif op == FUNCTION:
            order = 5
        if order is not None and not in_function:
            if order < section:
                problems.append("%s: out of the module's layout order" % where)
            section = max(section, order)
        elif order is not None and order < 5 and in_function:
            problems.append("%s: a type or constant inside a function" % where)

        rid = None
        if op in RESULT_AT_0 and ops:
            rid = ops[0]
        elif op in RESULT_AT_1 and len(ops) > 1:
            rid = ops[1]
        if rid is not None:
            if rid == 0 or rid >= bound:
                problems.append("%s: id %d outside the bound %d" % (where, rid, bound))
            if rid in defined:
                problems.append("%s: id %d defined twice" % (where, rid))
            defined[rid] = op
        if op == LABEL:
            labels.add(ops[0])

        if op == FUNCTION:
            if in_function:
                problems.append("%s: OpFunction inside a function" % where)
            in_function, in_block, first_block = True, False, True
        elif op == FUNCTION_END:
            if in_block:
                problems.append("%s: the last block has no terminator" % where)
            in_function, in_block = False, False
        elif op == LABEL:
            if not in_function:
                problems.append("%s: OpLabel outside a function" % where)
            if in_block:
                problems.append("%s: a block with no terminator" % where)
            in_block, block_has_code = True, False
        elif in_function:
            if not in_block:
                problems.append("%s: an instruction outside a block" % where)
            if op == VARIABLE:
                if not first_block or block_has_code:
                    problems.append("%s: OpVariable not at the start of the first block" % where)
            else:
                block_has_code = True
            if op == BRANCH and ops:
                branch_targets.append((where, ops[0]))
            if op == BRANCH_COND and len(ops) >= 3:
                branch_targets.append((where, ops[1]))
                branch_targets.append((where, ops[2]))
            if op in TERMINATORS:
                in_block, first_block = False, False
        i += count

    if in_function:
        problems.append("the module ends inside a function")
    for where, target in branch_targets:
        if target not in labels:
            problems.append("%s: branch to %d, which no OpLabel defines" % (where, target))

    for p in problems:
        print(p)
    if problems:
        print("%s: %d problem(s)" % (path, len(problems)))
        return 1
    print("%s: %d words, bound %d, %d ids, structure OK" % (path, len(words), bound, len(defined)))
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: spirv-check.py <module.spv>")
        sys.exit(2)
    sys.exit(main(sys.argv[1]))
