$f = 'C:\Users\ADMIN\CascadeProjects\ZLANG\p-lang\compiler\flat_ast.fd'
$c = Get-Content $f -Raw
$c = $c -replace 'type_flat\(\*(\w+),', 'type_flat(box_type_unbox($1),'
$c = $c -replace 'expr_flat\(\*(\w+),', 'expr_flat(box_expr_unbox($1),'
$c = $c -replace 'pattern_flat\(\*(\w+),', 'pattern_flat(box_pattern_unbox($1),'
Set-Content $f -Value $c -NoNewline
Write-Output "done"
