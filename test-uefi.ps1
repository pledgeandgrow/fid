#!/usr/bin/env pwsh
# test-uefi.ps1 — Build and test a Fides UEFI kernel under QEMU
# Usage: .\test-uefi.ps1 <file.fd> [expected_string]
# Example: .\test-uefi.ps1 tests\test_kernel_hello.fd "Hello from Fides OS!"

param(
    [Parameter(Mandatory=$true)]
    [string]$File,

    [string]$Expected = "Hello"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $RepoRoot

$QEMU = "C:\Program Files\qemu\qemu-system-x86_64.exe"
$OvmfCode = "release\ovmf_code.fd"
$OvmfVars = "release\ovmf_vars.fd"
$Stem = [System.IO.Path]::GetFileNameWithoutExtension($File)
$EfiPath = "release\$Stem.efi"

# 1. Ensure UEFI firmware is available
if (-not (Test-Path $OvmfCode)) {
    $src = "C:\Program Files\qemu\share\edk2-x86_64-code.fd"
    if (Test-Path $src) {
        Copy-Item $src $OvmfCode -Force
    } else {
        Write-Error "UEFI firmware not found. Install QEMU or copy OVMF to $OvmfCode"
        exit 1
    }
}
if (-not (Test-Path $OvmfVars)) {
    $src = "C:\Program Files\qemu\share\edk2-i386-vars.fd"
    if (Test-Path $src) {
        Copy-Item $src $OvmfVars -Force
    } else {
        Write-Error "UEFI vars firmware not found."
        exit 1
    }
}

# 2. Compile Fides source to UEFI PE32+
Write-Host "[1/3] Compiling $File -> $EfiPath ..."
& .\target\release\fidesc.exe emit $File --uefi -o $EfiPath
if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation failed"
    exit 1
}
Write-Host "  OK ($((Get-Item $EfiPath).Length) bytes)"

# 3. Set up EFI boot directory
$BootDir = "release\EFI\BOOT"
New-Item -ItemType Directory -Force -Path $BootDir | Out-Null
Copy-Item $EfiPath "$BootDir\BOOTX64.EFI" -Force

# 4. Run under QEMU with UEFI firmware
Write-Host "[2/3] Booting under QEMU UEFI ..."
$SerialOut = "release\uefi_test_serial.txt"
$StderrOut = "release\uefi_test_stderr.txt"

$proc = Start-Process -FilePath $QEMU `
    -ArgumentList '-pflash', $OvmfCode, '-pflash', $OvmfVars, `
                  '-serial', 'stdio', '-display', 'none', '-no-reboot', `
                  '-drive', 'format=raw,file=fat:rw:release' `
    -NoNewWindow -PassThru `
    -RedirectStandardOutput $SerialOut `
    -RedirectStandardError $StderrOut

Start-Sleep -Seconds 15
if (-not $proc.HasExited) { $proc.Kill() }

# 5. Check serial output
Write-Host "[3/3] Checking serial output ..."
$serialContent = Get-Content $SerialOut -Raw -ErrorAction SilentlyContinue
if ($serialContent -match [regex]::Escape($Expected)) {
    Write-Host "PASS: Found expected output '$Expected'" -ForegroundColor Green
    exit 0
} else {
    Write-Host "FAIL: Expected '$Expected' not found in serial output" -ForegroundColor Red
    Write-Host "Serial output (last 20 lines):"
    Get-Content $SerialOut -ErrorAction SilentlyContinue | Select-Object -Last 20
    exit 1
}
