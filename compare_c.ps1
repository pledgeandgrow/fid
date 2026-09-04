# compare_c.ps1 — Compare two C files function-by-function
$ErrorActionPreference = 'Stop'

function Extract-Funcs([string[]]$lines) {
    $funcs = [ordered]@{}
    $i = 0
    while ($i -lt $lines.Length) {
        $l = $lines[$i]
        # Only match function definitions at column 0 (no leading whitespace)
        if ($l -match '^(int64_t|void|bool|double|void\*|char\*) \w+\(.*\) \{$' -and $l -notmatch 'bb\d' -and -not ($l -match '^\s')) {
            $name = ($l -split '\(')[0].Trim()
            $start = $i
            $depth = 1
            $i++
            while ($i -lt $lines.Length -and $depth -gt 0) {
                $cl = $lines[$i]
                # Count braces at end of line
                if ($cl -match '\{$') { $depth++ }
                if ($cl -eq '}') { $depth-- }
                $i++
            }
            $body = $lines[$start..($i-1)]
            $funcs[$name] = $body
        } else {
            $i++
        }
    }
    return $funcs
}

$boot = Get-Content pe_bootstrap.c
$self = Get-Content pe_selfhost.c

Write-Output "Bootstrap: $($boot.Length) lines"
Write-Output "SelfHost: $($self.Length) lines"

$bf = Extract-Funcs $boot
$sf = Extract-Funcs $self

Write-Output "Bootstrap funcs: $($bf.Count)"
Write-Output "SelfHost funcs: $($sf.Count)"

# Compare function sizes
$diffs = @()
foreach ($name in $bf.Keys) {
    $bl = $bf[$name].Length
    if ($sf.Contains($name)) {
        $sl = $sf[$name].Length
    } else {
        $sl = -1
    }
    if ($bl -ne $sl) {
        $diffs += [PSCustomObject]@{Func=$name; Boot=$bl; Self=$sl; Diff=$bl-$sl}
    }
}

Write-Output ""
Write-Output "Functions with size differences (all):"
$diffs | Sort-Object Diff -Descending | Select-Object -First 25 | Format-Table -AutoSize

# For top 5, show missing lines
$topN = $diffs | Sort-Object Diff -Descending | Select-Object -First 5
foreach ($d in $topN) {
    $name = $d.Func
    Write-Output ""
    Write-Output "=== $name (boot=$($d.Boot), self=$($d.Self), diff=$($d.Diff)) ==="
    if (-not $sf.Contains($name)) {
        Write-Output "  FUNCTION MISSING from selfhost!"
        continue
    }
    $bbody = $bf[$name]
    $sbody = $sf[$name]
    # Find lines in bootstrap but not in selfhost (using set difference)
    $sset = [System.Collections.Generic.HashSet[string]]::new()
    foreach ($l in $sbody) { [void]$sset.Add($l.Trim()) }
    $missing = @()
    foreach ($l in $bbody) {
        if (-not $sset.Contains($l.Trim())) {
            $missing += $l
        }
    }
    Write-Output "Lines in bootstrap but NOT in selfhost ($($missing.Count) lines, showing first 30):"
    $missing | Select-Object -First 30 | ForEach-Object { Write-Output "  $_" }
}
