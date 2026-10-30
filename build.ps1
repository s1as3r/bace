[CmdletBinding()]
param(
    [switch]$Release
)

$ErrorActionPreference = "Stop"

$ObjName = "bace.obj"
$CC = "cl"

$Config = if ($Release -or $env:BUILD_TYPE -eq "Release") { "Release" } else { "Debug" }

$BuildPath = if ($env:BUILD_PATH) { $env:BUILD_PATH }
             else { [System.IO.Path]::GetFullPath((Join-Path $PSScriptRoot "build")) }
$Src     = (Resolve-Path (Join-Path $PSScriptRoot "src")).Path
$Include = (Resolve-Path (Join-Path $PSScriptRoot "include")).Path

# extra arguments
$ExtraArgs = if ($env:CC_ARGS) {
    $env:CC_ARGS -split '\s+' | Where-Object { $_ }
} else {
    @()
}

$OptFlags = if ($Release -or $Config -eq "Release") { @("/O2", "/DNDEBUG") }
            else { @("/Od", "/Zi") }

$Defines = @()

$CompFlags = @(
    "/I$Include"
    "/W4"
    "/WX"
    "/std:c11"
    "/nologo"
) + $OptFlags

$ObjPath = Join-Path $BuildPath $ObjName
$ObjArgs = $Defines + $CompFlags + @(
    "/c"
    "/Fo$ObjPath"
    "$Src\bace.c"
) + $ExtraArgs


function Invoke-Native {
    param([Parameter(Mandatory)][scriptblock]$Command)
    $global:LASTEXITCODE = 0
    & $Command | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "command failed (exit $LASTEXITCODE): $Command"
    }
}


$ok = $false
try {
    if (-not (Test-Path $BuildPath)) {
        Write-Host "created build directory"
        New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
    }

    Write-Host "===== $ObjName ($Config) ====="
    Write-Host $CC @ObjArgs
    Invoke-Native { & $CC @ObjArgs }
    $ok = $true
} catch {
    Write-Host "build failed: $($_.Exception.Message)" -ForegroundColor Red
}

exit ([int](-not $ok))
