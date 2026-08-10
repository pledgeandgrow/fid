#!/usr/bin/env python3
"""Structural validator for Pledge-emitted WASM binaries.

Verifies the binary is well-formed without needing a WASM runtime:
  - magic + version
  - section ordering and sizes are consistent
  - type section parses (functype entries)
  - import section parses (function imports with valid type indices)
  - function section parses (type indices in range)
  - memory / global / export sections parse
  - code section: function count matches function section, each body parses
    (locals declarations, instruction stream ends with `end` 0x0B)
  - data section parses

This is NOT a full WASM validator (no type-checking of instruction stacks),
but it catches structural corruption which is the common bug surface for a
hand-written encoder.
"""

import sys
import struct

def read_uleb(data, i):
    result = 0
    shift = 0
    while True:
        b = data[i]; i += 1
        result |= (b & 0x7F) << shift
        if not (b & 0x80):
            break
        shift += 7
    return result, i

def read_sleb(data, i):
    result = 0
    shift = 0
    while True:
        b = data[i]; i += 1
        result |= (b & 0x7F) << shift
        if not (b & 0x80):
            if b & 0x40:
                result |= -(1 << (shift + 7))
            break
        shift += 7
    return result, i

SEC_NAMES = {
    0: "custom", 1: "type", 2: "import", 3: "function", 4: "table",
    5: "memory", 6: "global", 7: "export", 8: "start", 9: "element",
    10: "code", 11: "data", 12: "datacount",
}

def validate(path):
    with open(path, "rb") as f:
        data = f.read()

    errors = []
    warnings = []
    info = {}

    if len(data) < 8:
        errors.append("file too short (< 8 bytes)")
        return errors, warnings, info

    if data[0:4] != b"\x00asm":
        errors.append(f"bad magic: {data[0:4]!r}")
    version = struct.unpack_from("<I", data, 4)[0]
    if version != 1:
        errors.append(f"unsupported version {version}")
    info["version"] = version

    i = 8
    sections = {}
    prev_id = 0
    while i < len(data):
        sec_id = data[i]; i += 1
        size, i = read_uleb(data, i)
        body = data[i:i+size]
        if len(body) != size:
            errors.append(f"section {sec_id} ({SEC_NAMES.get(sec_id,'?')}) truncated: declared {size} bytes, got {len(body)}")
            break
        sections[sec_id] = body
        if sec_id != 0 and sec_id < prev_id:
            errors.append(f"section {sec_id} out of order (after {prev_id})")
        if sec_id != 0:
            prev_id = sec_id
        i += size

    info["sections"] = sorted(sections.keys())

    # ─── Type section ────────────────────────────────────────────────
    type_sigs = []
    if 1 in sections:
        b = sections[1]; j = 0
        count, j = read_uleb(b, j)
        info["type_count"] = count
        for k in range(count):
            if b[j] != 0x60:
                errors.append(f"type {k}: expected functype 0x60, got {b[j]:#x}")
                break
            j += 1
            pcount, j = read_uleb(b, j)
            params = b[j:j+pcount]; j += pcount
            rcount, j = read_uleb(b, j)
            results = b[j:j+rcount]; j += rcount
            type_sigs.append((params, results))
        if j != len(b):
            errors.append(f"type section trailing bytes: {len(b)-j}")

    # ─── Import section ──────────────────────────────────────────────
    import_count = 0
    import_func_count = 0
    if 2 in sections:
        b = sections[2]; j = 0
        count, j = read_uleb(b, j)
        import_count = count
        for k in range(count):
            mlen, j = read_uleb(b, j)
            mod_name = b[j:j+mlen].decode("utf-8", "replace"); j += mlen
            flen, j = read_uleb(b, j)
            field_name = b[j:j+flen].decode("utf-8", "replace"); j += flen
            kind = b[j]; j += 1
            if kind == 0x00:  # function
                tidx, j = read_uleb(b, j)
                import_func_count += 1
                if tidx >= len(type_sigs):
                    errors.append(f"import {k} ({mod_name}.{field_name}): type idx {tidx} out of range ({len(type_sigs)})")
            elif kind == 0x01:  # table
                j += 1  # elemtype
                # limits
                flag = b[j]; j += 1
                _, j = read_uleb(b, j)
                if flag == 1:
                    _, j = read_uleb(b, j)
            elif kind == 0x02:  # memory
                flag = b[j]; j += 1
                _, j = read_uleb(b, j)
                if flag == 1:
                    _, j = read_uleb(b, j)
            elif kind == 0x03:  # global
                j += 1  # valtype
                j += 1  # mutability
            else:
                errors.append(f"import {k}: unknown kind {kind:#x}")
                break
        if j != len(b):
            errors.append(f"import section trailing bytes: {len(b)-j}")
    info["import_count"] = import_count
    info["import_func_count"] = import_func_count

    # ─── Function section ────────────────────────────────────────────
    func_type_idxs = []
    if 3 in sections:
        b = sections[3]; j = 0
        count, j = read_uleb(b, j)
        info["function_count"] = count
        for k in range(count):
            tidx, j = read_uleb(b, j)
            if tidx >= len(type_sigs):
                errors.append(f"function {k}: type idx {tidx} out of range")
            func_type_idxs.append(tidx)
        if j != len(b):
            errors.append(f"function section trailing bytes: {len(b)-j}")

    total_funcs = import_func_count + len(func_type_idxs)

    # ─── Memory section ──────────────────────────────────────────────
    if 5 in sections:
        b = sections[5]; j = 0
        count, j = read_uleb(b, j)
        info["memory_count"] = count
        for k in range(count):
            flag = b[j]; j += 1
            _, j = read_uleb(b, j)
            if flag == 1:
                _, j = read_uleb(b, j)
        if j != len(b):
            errors.append(f"memory section trailing bytes: {len(b)-j}")

    # ─── Global section ──────────────────────────────────────────────
    if 6 in sections:
        b = sections[6]; j = 0
        count, j = read_uleb(b, j)
        info["global_count"] = count
        for k in range(count):
            j += 1  # valtype
            j += 1  # mutability
            # init expression: scan until 0x0B end
            depth = 0
            while j < len(b):
                op = b[j]; j += 1
                if op == 0x0B:
                    break
                # skip common init opcodes
                if op in (0x41, 0x42):  # i32/i64.const
                    _, j = read_sleb(b, j)
                elif op in (0x43,):  # f32.const
                    j += 4
                elif op in (0x44,):  # f64.const
                    j += 8
                elif op in (0x23,):  # global.get
                    _, j = read_uleb(b, j)
                else:
                    pass
        if j != len(b):
            errors.append(f"global section trailing bytes: {len(b)-j}")

    # ─── Export section ──────────────────────────────────────────────
    if 7 in sections:
        b = sections[7]; j = 0
        count, j = read_uleb(b, j)
        info["export_count"] = count
        exports = []
        for k in range(count):
            nlen, j = read_uleb(b, j)
            name = b[j:j+nlen].decode("utf-8", "replace"); j += nlen
            kind = b[j]; j += 1
            idx, j = read_uleb(b, j)
            exports.append((name, kind, idx))
            if kind == 0x00 and idx >= total_funcs:
                errors.append(f"export {name}: func idx {idx} out of range ({total_funcs})")
            if kind == 0x02 and idx >= info.get("memory_count", 0):
                errors.append(f"export {name}: memory idx {idx} out of range")
        info["exports"] = exports
        if j != len(b):
            errors.append(f"export section trailing bytes: {len(b)-j}")

    # ─── Code section ────────────────────────────────────────────────
    if 10 in sections:
        b = sections[10]; j = 0
        count, j = read_uleb(b, j)
        info["code_count"] = count
        if count != len(func_type_idxs):
            errors.append(f"code section count {count} != function section count {len(func_type_idxs)}")
        for k in range(count):
            body_size, j = read_uleb(b, j)
            body = b[j:j+body_size]; j += body_size
            m = 0
            # locals
            lcount, m = read_uleb(body, m)
            for _ in range(lcount):
                rep, m = read_uleb(body, m)
                m += 1  # valtype
            # instruction stream: scan to end (0x0B at top level)
            # We don't fully decode; just check it ends with 0x0B and has
            # balanced block/if/loop nesting.
            depth = 0
            end_seen = False
            pos = m
            while pos < len(body):
                op = body[pos]; pos += 1
                if op in (0x02, 0x03, 0x04):  # block, loop, if
                    depth += 1
                    pos += 1  # blocktype
                elif op == 0x05:  # else
                    pass
                elif op == 0x0B:  # end
                    if depth == 0:
                        end_seen = True
                        break
                    depth -= 1
                elif op == 0x0C or op == 0x0D:  # br, br_if
                    _, pos = read_uleb(body, pos)
                elif op == 0x0E:  # br_table
                    n, pos = read_uleb(body, pos)
                    for _ in range(n + 1):
                        _, pos = read_uleb(body, pos)
                elif op == 0x10:  # call
                    _, pos = read_uleb(body, pos)
                elif op == 0x11:  # call_indirect
                    _, pos = read_uleb(body, pos)
                    _, pos = read_uleb(body, pos)
                elif op in (0x20, 0x21, 0x22):  # local.get/set/tee
                    _, pos = read_uleb(body, pos)
                elif op in (0x23, 0x24):  # global.get/set
                    _, pos = read_uleb(body, pos)
                elif op in (0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
                            0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                            0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E):
                    _, pos = read_uleb(body, pos)  # align
                    _, pos = read_uleb(body, pos)  # offset
                elif op == 0x40:  # memory.grow etc — actually 0x40 is blocktype void
                    pass
                elif op == 0x3F or op == 0x40:  # memory.size / memory.grow (single byte memarg 0x00)
                    pos += 1
                elif op in (0x41, 0x42):  # i32/i64.const
                    _, pos = read_sleb(body, pos)
                elif op == 0x43:  # f32.const
                    pos += 4
                elif op == 0x44:  # f64.const
                    pos += 8
                elif op == 0x0F:  # return
                    pass
                elif op == 0x00:  # unreachable
                    pass
                elif op == 0x01:  # nop
                    pass
                elif op == 0x1A:  # drop
                    pass
                # All other numeric/comparison ops are single-byte, no immediates.
            if not end_seen:
                errors.append(f"code {k}: body did not end with 0x0B (depth={depth}, pos={pos}/{len(body)})")
            if pos != len(body):
                errors.append(f"code {k}: trailing bytes after end: {len(body)-pos}")
        if j != len(b):
            errors.append(f"code section trailing bytes: {len(b)-j}")

    # ─── Data section ────────────────────────────────────────────────
    if 11 in sections:
        b = sections[11]; j = 0
        count, j = read_uleb(b, j)
        info["data_count"] = count
        for k in range(count):
            memidx, j = read_uleb(b, j)
            # offset init expr: scan to 0x0B
            while j < len(b):
                op = b[j]; j += 1
                if op == 0x0B:
                    break
                if op in (0x41, 0x42):
                    _, j = read_sleb(b, j)
                elif op in (0x23,):
                    _, j = read_uleb(b, j)
            seg_size, j = read_uleb(b, j)
            j += seg_size
        if j != len(b):
            errors.append(f"data section trailing bytes: {len(b)-j}")

    return errors, warnings, info

def main():
    if len(sys.argv) < 2:
        print("usage: wasm_validate.py <file.wasm>", file=sys.stderr)
        sys.exit(2)
    path = sys.argv[1]
    errors, warnings, info = validate(path)
    print(f"File: {path} ({__import__('os').path.getsize(path)} bytes)")
    print(f"Sections: {info.get('sections', [])}")
    print(f"  types={info.get('type_count',0)} imports={info.get('import_count',0)} "
          f"import_funcs={info.get('import_func_count',0)} functions={info.get('function_count',0)} "
          f"code={info.get('code_count',0)}")
    print(f"  memories={info.get('memory_count',0)} globals={info.get('global_count',0)} "
          f"exports={info.get('export_count',0)} data={info.get('data_count',0)}")
    if "exports" in info:
        for name, kind, idx in info["exports"]:
            kindname = {0:"func",1:"table",2:"memory",3:"global"}.get(kind, str(kind))
            print(f"    export {name!r} {kindname}#{idx}")
    if warnings:
        print(f"Warnings ({len(warnings)}):")
        for w in warnings:
            print(f"  {w}")
    if errors:
        print(f"ERRORS ({len(errors)}):")
        for e in errors:
            print(f"  {e}")
        sys.exit(1)
    print("OK — structurally valid")

if __name__ == "__main__":
    main()
