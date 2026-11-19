# UEFI applications (`fid emit --uefi`)

Roadmap Phase 16, "UEFI PE32+ emission in self-hosted PE emitter".

`fid emit <file.fd> --uefi -o app.efi` writes a UEFI application: a PE32+
image of subsystem 10 (EFI_APPLICATION) for x86-64 firmware. As with
`--bare`, the program is built without the stdlib (`build_program_no_std`);
the code is the bare build's (pe_ir), with a small runtime over the
firmware's boot services.

## Where it lives

| Piece | File | Function |
|---|---|---|
| Entry, `--uefi` image | `compiler/pe_ir.fd` | `pe_ir_generate_uefi` |
| PE32+ writer, `.reloc` | `compiler/pe_ir_enc.fd` | `pe_build_efi`, `pe_efi_relocs` |
| Runtime | `compiler/pe_ir_stubs.fd` | `PeirGen_gen_uefi_entry`, `_start`, `_io`, `_exit` |
| CLI flag | `compiler/main.fd` | `emit ... --uefi` |

## The image

- Headers (512 bytes), then `.text` at RVA 0x1000 (the flat binary's code;
  the entry point is its first byte, a jump to the UEFI entry), `.data`
  right after it (strings, globals, the allocator state; read/write), and
  `.reloc`.
- ImageBase 0x400000, but the firmware may load it anywhere: the code is
  RIP-relative, and the only absolute addresses (the slots of str-literal
  globals) have DIR64 base relocations. Relocations are never stripped; an
  image without any absolute address still has a `.reloc` block of padding.
- No imports: the firmware hands the entry point the system table.
- DllCharacteristics DYNAMIC_BASE | NX_COMPAT: nothing in `.data` runs, and
  `.text` is never written.

## The runtime

- Entry (`rcx` = ImageHandle, `rdx` = SystemTable): saves rbx, rbp, rdi,
  rsi, r12-r15 and xmm6-xmm15 (preserved in the UEFI calling convention),
  turns the watchdog off (`SetWatchdogTimer(0, ...)`), takes a heap of
  128 MB from `AllocatePages` (halved until it fits, at least 1 MB) and a
  1 MB stack of its own (native frames are large), then calls `main`.
- `print`/`eprint`: `ConOut->OutputString`, UTF-8 converted to UCS-2;
  `"\n"` becomes `"\r\n"`, characters outside the BMP and bad sequences
  become U+FFFD.
- Heap: the bare bump allocator, bounded: past the end, the panic
  "out of memory". `free` does nothing.
- Exit: main's result is the image's EFI_STATUS (a void main returns
  EFI_SUCCESS). The heap and stack pages are freed, the saved registers
  restored, and the entry returns to whoever started the image (the shell,
  the boot manager).
- Panic: `panic: <message>` (or the program's `@panic_handler`), then the
  image returns EFI_ABORTED. Not enough memory at start: a message and
  EFI_OUT_OF_RESOURCES.
- Any other runtime function traps (`ud2`): the firmware's exception
  handler reports it.

Allocator-state words of a bare build used by the UEFI runtime: +0 cursor,
+8 heap end, +16 SystemTable, +24 ImageHandle, +32 the entry's stack
pointer, +40 stack base, +48 exit status, +56 heap base, +64 heap pages.

## Testing

```bash
FID=release/fid.exe bash tests/uefi/run.sh
```

1. `tests/uefi/uefi_image_unit.fd`, compiled through C (`fid ccompile`;
   `UNIT=run` uses the interpreter, much slower): `pe_ir_generate_uefi` on
   a hand-built IR module; reads the headers back.
2. `fid emit tests/bootstrap/test_uefi_hello.fd --uefi`. With
   `FID_UEFI_DRIVER=<exe>`, a built `tests/uefi/uefi_emit_driver.fd` makes
   the image instead: the same steps as the `--uefi` branch of
   `compiler/main.fd` (no-stdlib build, lowering, `pe_ir_generate_uefi`),
   for checking the tree's emitter with a `fid` that predates the flag.
3. `tests/uefi/check_efi.py`: PE32+ structure, subsystem, sections and
   their permissions, entry, every relocation.
4. With QEMU and OVMF (here, or in WSL when run from Git Bash on Windows):
   `tests/uefi/boot.sh` boots each image as the removable-media boot file
   `EFI/BOOT/BOOTX64.EFI` of a FAT folder (OVMF builds without the UEFI
   shell, such as Ubuntu's `/usr/share/ovmf/OVMF.fd`, boot it directly; a
   `startup.nsh` covers firmware that starts the shell). The console must
   show the expected lines; QEMU stops at the last one.

By hand (WSL, Ubuntu's `qemu-system-x86` and `ovmf` packages):

```bash
mkdir -p esp/EFI/BOOT && cp app.efi esp/EFI/BOOT/BOOTX64.EFI
qemu-system-x86_64 -machine q35 -m 256 -nographic -net none \
  -bios /usr/share/ovmf/OVMF.fd -drive format=raw,file=fat:rw:esp
```

Nov 16: the unit image (`uefi_image_unit.fd` compiled by a stage-0 `fid`
against this tree's `compiler/pe_ir*.fd`) passed `check_efi.py` and booted
under QEMU 11.1 + OVMF (Debian's `/usr/share/ovmf/OVMF.fd`) in WSL, printing `hello from UEFI`, `Fides` (read
through the slot that has the DIR64 relocation) and `6765`, then returned
to the boot manager. `tests/bootstrap/test_uefi_hello.fd`, made into an
image by `tests/uefi/uefi_emit_driver.fd` (built the same way), passed
`check_efi.py` and printed all eight expected lines under the same QEMU:
`FID_UEFI_DRIVER=<driver.exe> UNIT=0 bash tests/uefi/run.sh` gave
`PASS emit --uefi`, `PASS boot hello.efi`, `PASS boot unit.efi`.
`fid emit --uefi` itself waits for the `--uefi` branch in
`compiler/main.fd`.

Nov 17: rerun against the tree after the day's pe_ir changes (`--cpu`, the
function cache, which bare and UEFI builds never use, and the nat_*
primitives). The unit test printed all 12 lines, and both images passed
`check_efi.py` and booted under the same QEMU + OVMF (`PASS emit --uefi`,
`PASS boot hello.efi`, `PASS boot unit.efi`). `run.sh` was also made
stricter. A `fid` without the flag builds an ordinary `hello.efi.exe`, and
Git Bash's `[ -f hello.efi ]` is true for that file, so step 2 used to
pass anyway. Step 2 now requires the `Emitted UEFI application` line. The
boot step also clears an earlier run's files out of the ESP folder first.

## Not done

- No access to UEFI protocols from Fides code (no `stdlib/uefi.fd` here:
  it is in the fides-os project); the program sees only the runtime above.
- `ExitBootServices` is not called: the image is an application, not an OS
  loader.
- AArch64 UEFI images (the machine layer, nat_ir) are not written.
