param(
    [string]$ExePath = "$env:USERPROFILE\Desktop\sabbbb\Subnautica2Panel.exe"
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path $ExePath)) {
    Write-Error "Not found: $ExePath"
}

$dir = Split-Path $ExePath
$others = Get-ChildItem $dir -File | Where-Object { $_.Name -ne (Split-Path $ExePath -Leaf) }
if ($others.Count -gt 0) {
    Write-Warning "Extra files in output folder:"
    $others | ForEach-Object { Write-Host "  $($_.Name)" }
} else {
    Write-Host "OK: only one file in $dir"
}

$dumpbin = @(
    "${env:ProgramFiles(x86)}\Microsoft Visual Studio\18\Insiders\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe",
    "${env:ProgramFiles}\Microsoft Visual Studio\2022\*\VC\Tools\MSVC\*\bin\Hostx64\x64\dumpbin.exe"
) | Get-Item -ErrorAction SilentlyContinue | Select-Object -First 1

if ($dumpbin) {
    Write-Host "`nDependencies (must not list VCRUNTIME / MSVCP):"
    & $dumpbin.FullName /dependents $ExePath | Select-String "\.dll"
}

$sig = Get-AuthenticodeSignature $ExePath
Write-Host "`nSignature status: $($sig.Status)"
if ($sig.SignerCertificate) {
    Write-Host "Signer: $($sig.SignerCertificate.Subject)"
}

$ver = [System.Diagnostics.FileVersionInfo]::GetVersionInfo($ExePath)
Write-Host "`nFile version: $($ver.FileVersion)"
Write-Host "Product: $($ver.ProductName)"
Write-Host "Company: $($ver.CompanyName)"
