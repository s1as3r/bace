# test.ps1 - MSVC (cl.exe) equivalent of test.sh
$ExePrefix = "test_"
$CC = "cl"

$BuildPath = if ($env:BUILD_PATH) { $env:BUILD_PATH }
else { [System.IO.Path]::GetFullPath((Join-Path (Get-Location) "build/tests")) }

$Tests = (Resolve-Path "./tests/").Path
$Includes = (Resolve-Path "./include/").Path
$AcutestPath = (Resolve-Path "./external/acutest/include/").Path

$BaceLib = Join-Path $BuildPath "bace.obj"
$BaceBuildPs1 = (Resolve-Path "./build.ps1").Path

# debug flags
$Debug = @(
    "/Zi"
)

# compile time defines
$Defines = @()

# windows platform libraries
$Libs = @(
    "`"$BaceLib`""
)

# compiler flags
$CompFlags = @(
    "/I$AcutestPath"
    "/I$Includes"
    "/W4"
    "/WX"
    "/Od"
    "/std:c11"
    "/nologo"
)

$NoBuild = $false
$NoRun = $false
$BuildBaceFlag = $false
foreach ($arg in $args) {
    switch ($arg) {
        { $_ -cin "--no-build", "-B" } { $NoBuild = $true; break }
        { $_ -cin "--no-run", "-R" } { $NoRun = $true; break }
        { $_ -cin "--build-bace", "-b" } { $BuildBaceFlag = $true; break }
    }
}

# create build directory if it doesn't exist
if (-not (Test-Path $BuildPath)) {
    Write-Output "created build directory"
    New-Item -ItemType Directory -Path $BuildPath -Force | Out-Null
}

if ((-not (Test-Path $BaceLib)) -or $BuildBaceFlag) {
    Write-Output "=== building bace ==="

    $old_bpath = $env:BUILD_PATH
    $env:BUILD_PATH = $BuildPath
    try {
        & $BaceBuildPs1
        $baceExitCode = $LASTEXITCODE
    } finally {
        $env:BUILD_PATH = $old_bpath
    }

    if ($baceExitCode -ne 0) {
        Write-Host "building bace failed"
        exit 1
    }
}

Push-Location $BuildPath

Get-ChildItem -Path $Tests -Filter "*.c" | ForEach-Object {
    $exeName = "$ExePrefix$($_.BaseName).exe"

    $exeCmd = @($CC) + $Defines + $Debug + $CompFlags + @(
        "/Fe`"$exeName`""
        "`"$($_.FullName)`""
    ) + $Libs

    $exeCmdStr = $exeCmd -join " "

    Write-Output "===== $exeName ====="
    if (-not $NoBuild) {
        Write-Output $exeCmdStr
        Invoke-Expression $exeCmdStr
        if ($LASTEXITCODE -ne 0) {
            Write-Output "failed building $exeName"
        }
    }
    if (-not $NoRun) {
        Write-Output ""
        & "./$exeName"
    }
}

Pop-Location
