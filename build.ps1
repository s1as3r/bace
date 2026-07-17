# build.ps1 - MSVC (cl.exe) equivalent of build.sh
$ErrorActionPreference = "Stop"

$ObjName = "bace.obj"
$CC = "cl"

$BuildPath = if ($env:BUILD_PATH) { $env:BUILD_PATH }
else { [System.IO.Path]::GetFullPath((Join-Path (Get-Location) "build")) }
$Src = (Resolve-Path "./src/").Path
$Include = (Resolve-Path "./include/").Path

# debug flags
$Debug = @(
    "/Zi"
)

# compile time defines
$Defines = @()

# windows platform libraries
$Libs = @(
    # "/LIBPATH:$BuildPath"
)

# compiler flags
$CompFlags = @(
    "/I$Include"
    "/W4"
    "/WX"
    "/Od"
    "/std:c11"
    "/nologo"
)

# build commands
$ObjCmd = @($CC) + $Defines + $Debug + $CompFlags + @(
    "/c"
    "/Fo`"$ObjName`""
    "`"$Src\bace.c`""
) + $Libs

$ObjCmdStr = $ObjCmd -join " "

# create build directory if it doesn't exist
if (-not (Test-Path $BuildPath)) {
    Write-Output "created build directory"
    New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
}

Push-Location $BuildPath
Write-Output "===== $ObjName ====="
Write-Output $ObjCmdStr
Invoke-Expression $ObjCmdStr
$exitCode = $LASTEXITCODE
Pop-Location

exit $exitCode
