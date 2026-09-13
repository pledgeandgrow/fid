$bytes = [System.IO.File]::ReadAllBytes("release/fides-c.exe")
$peOffset = [BitConverter]::ToInt32($bytes, 0x3C)
$stackOffset = $peOffset + 0x18 + 0x48
[Array]::Copy([BitConverter]::GetBytes([int64]268435456), 0, $bytes, $stackOffset, 8)
[Array]::Copy([BitConverter]::GetBytes([int64]268435456), 0, $bytes, $stackOffset + 8, 8)
[System.IO.File]::WriteAllBytes("release/fides-c.exe", $bytes)
Write-Output "Stack patched to 256MB at offset $stackOffset (PE offset $peOffset)"
