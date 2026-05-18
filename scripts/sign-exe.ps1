param(
    [Parameter(Mandatory = $true)]
    [string]$ExePath,
    [string]$PfxPath = "",
    [string]$Password = "Subnautica2Trainer",
    [string]$TimestampServer = "http://timestamp.digicert.com"
)

$ErrorActionPreference = "Stop"
if (-not (Test-Path $ExePath)) {
    Write-Error "EXE not found: $ExePath"
}

$root = Split-Path -Parent $PSScriptRoot
if (-not $PfxPath) {
    $PfxPath = Join-Path $root "signing\Subnautica2Trainer.pfx"
}
if (-not (Test-Path $PfxPath)) {
    Write-Warning "Signing skipped: no PFX at $PfxPath"
    exit 0
}

$secure = ConvertTo-SecureString -String $Password -Force -AsPlainText
$cert = Import-PfxCertificate -FilePath $PfxPath -Password $secure -CertStoreLocation Cert:\CurrentUser\My -Exportable

try {
    $sig = Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert -HashAlgorithm SHA256 `
        -TimestampServer $TimestampServer
} catch {
    Write-Warning "Timestamp failed, signing without timestamp: $_"
    $sig = Set-AuthenticodeSignature -FilePath $ExePath -Certificate $cert -HashAlgorithm SHA256
}

Write-Host "Sign result: $($sig.Status)"
if ($sig.Status -ne "Valid" -and $sig.Status -ne "UnknownError") {
    Write-Warning "Signature status: $($sig.Status)"
}
exit 0
