# Creates a self-signed code-signing certificate for local builds.
# For production, replace signing/Subnautica2Trainer.pfx with your EV/OV certificate.
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$signDir = Join-Path $root "signing"
$pfxPath = Join-Path $signDir "Subnautica2Trainer.pfx"
$password = "Subnautica2Trainer"

New-Item -ItemType Directory -Force -Path $signDir | Out-Null

if (Test-Path $pfxPath) {
    Write-Host "Certificate already exists: $pfxPath"
    exit 0
}

$subject = "CN=SabNavr Code Signing, O=SabNavr, C=US"
$cert = New-SelfSignedCertificate `
    -Type CodeSigningCert `
    -Subject $subject `
    -KeyAlgorithm RSA `
    -KeyLength 2048 `
    -HashAlgorithm SHA256 `
    -NotAfter (Get-Date).AddYears(5) `
    -CertStoreLocation "Cert:\CurrentUser\My" `
    -KeyExportPolicy Exportable `
    -KeyUsage DigitalSignature

$secure = ConvertTo-SecureString -String $password -Force -AsPlainText
Export-PfxCertificate -Cert $cert -FilePath $pfxPath -Password $secure | Out-Null

Write-Host "Created: $pfxPath"
Write-Host "Password (S2_SIGN_PFX_PASSWORD): $password"
Write-Host "Trust this cert in Windows if SmartScreen should show your publisher name."
