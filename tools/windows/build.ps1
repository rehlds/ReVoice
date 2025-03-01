# Stop on first error
$ErrorActionPreference = "Stop"

# Default values
$JOBS = 0
$BUILD_TYPE = "Release"
$PRESERVE_BUILD = 0
$LINK_STATIC_MSVC_RT = "OFF"
$OPTIMIZE_NATIVE = "OFF"
$ADDITIONAL_CMAKE_ARGS = @()

# Display help message
function Show-Help {
    Write-Host @"
Usage: tools\windows\build.ps1 [options]

Options:
  -h, --help
        Show this help message and exit.

  -b, --build-type <type>
        Set the build type (Debug, Release, RelWithDebInfo) (default: Release).

  -c, --compiler <name>
        Specify the compiler to use (msvc or clang) (default: auto-detected).

  -g, --generator <name>
        Specify the build system generator (ninja or vs2022) (default: auto-detected).

  -j, --jobs <n>
        Number of parallel jobs for the build (default: number of CPU cores).

  -k, --keep-build
        Preserve the build directory; does not delete it before building.

  -o, --optimize
        Enable optimizations for the current CPU
        (equivalent to compiler flag -march=native) (clang only).

  -s, --static-runtime
        Enable static linking of runtime libraries (e.g., msvcrt).

  -- <args>
        Pass remaining arguments directly to CMake.

Examples:
  tools\windows\build.ps1
        Automatically detect the compiler and build with default settings.

  tools\windows\build.ps1 -c msvc -b debug
        Build using the MSVC compiler with the Debug build type.

  tools\windows\build.ps1 --compiler clang --optimize
        Build using the Clang compiler with optimizations enabled for the current CPU.

  tools\windows\build.ps1 -- -DENABLE_SPEEX_VORBIS_PSY=ON
        Automatically detect the compiler and pass additional options to CMake.
"@
}

# Parse command-line arguments
$i = 0
while ($i -lt $args.Count) {
    switch ($args[$i]) {
        {$_ -in "-h", "--help"} {
            Show-Help;
            exit 0
        }
        {$_ -in "-b", "--build-type"} {
            $BUILD_TYPE = $args[$i + 1]
            $i += 2
        }
        {$_ -in "-c", "--compiler"} {
            $COMPILER = $args[$i + 1].ToLower()
            $i += 2
        }
        {$_ -in "-g", "--generator"} {
            $GENERATOR = $args[$i + 1].ToLower()
            $i += 2
        }
        {$_ -in "-j", "--jobs"} {
            $JOBS = $args[$i + 1]
            $i += 2
        }
        {$_ -in "-k", "--keep-build"} {
            $PRESERVE_BUILD = 1
            $i += 1
        }
        {$_ -in "-o", "--optimize"} {
            $OPTIMIZE_NATIVE = "ON"
            $i += 1
        }
        {$_ -in "-s", "--static-runtime"} {
            $LINK_STATIC_MSVC_RT = "ON"
            $i += 1
        }
        default {
            $ADDITIONAL_CMAKE_ARGS += $args[$i]
            $i += 1
        }
    }
}

# Initialize Visual Studio development environment
function Initialize-VsEnvironment {
    $vsToolsPaths = @(
        "C:\Program Files\Microsoft Visual Studio\*\*\*\Tools\Launch-VsDevShell.ps1"
        "C:\Program Files (x86)\Microsoft Visual Studio\*\*\*\Tools\Launch-VsDevShell.ps1"
    )

    $launchVsDevShell = $vsToolsPaths | ForEach-Object {
        Get-Item $_ -ErrorAction SilentlyContinue
    } | Select-Object -First 1 -ExpandProperty FullName

    if (Test-Path $launchVsDevShell) {
        & "$launchVsDevShell" -Arch x86 -HostArch amd64 -SkipAutomaticLocation

        if ($LASTEXITCODE -ne 0) {
            throw "Failed to initialize Visual Studio environment, exit code: $LASTEXITCODE"
        }
    }
}

# Check availability of Ninja build system
function Check-Ninja {
    if (Get-Command ninja -ErrorAction SilentlyContinue) {
        $ninjaVersion = ninja --version
        Write-Host "-- Found Ninja $ninjaVersion"
    }
}

# Check CMake availability and minimum version
function Check-CMake {
    $requiredMajor = 3
    $requiredMinor = 21
    $cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue

    if (-not $cmakeCommand) {
        Write-Host "Error: CMake not found. Please install CMake."
        exit 1
    }

    $cmakeVersionOutput = cmake --version
    $cmakeVersion = ($cmakeVersionOutput -split "\n")[0] -replace "cmake version ", ""
    Write-Host "-- Found CMake version $cmakeVersion"

    $versionParts = $cmakeVersion -split "\."
    $cmakeMajor = [int]$versionParts[0]
    $cmakeMinor = [int]$versionParts[1]

    if (($cmakeMajor -lt $requiredMajor) -or (($cmakeMajor -eq $requiredMajor) -and ($cmakeMinor -lt $requiredMinor))) {
        Write-Host "Error: CMake version $requiredMajor.$requiredMinor or higher is required. Please update CMake."
        exit 1
    }
}

# Select build system generator
function Select-Generator {
    if ($GENERATOR) {
        return
    }

    if (Get-Command ninja -ErrorAction SilentlyContinue) {
        $script:GENERATOR = "ninja"
    }
    else {
        $script:GENERATOR = "vs2022"
    }
}

# Select a compiler to use
function Select-Compiler {
    if ($GENERATOR -eq "vs2022") {
        if (-not $COMPILER) {
            $script:COMPILER = "msvc"
        }
    }
    else {
        Initialize-VsEnvironment
        $presetList = cmake --list-presets
        $isClangPresetAvailable = [bool]($presetList | Where-Object { $_ -match '"[^"]*ninja-clang-[^"]*"' })

        if (-not $COMPILER) {
            if (($isClangPresetAvailable) -and (Get-Command clang -ErrorAction SilentlyContinue)) {
                $script:COMPILER = "clang"
            }
            elseif (Get-Command cl -ErrorAction SilentlyContinue) {
                $script:COMPILER = "msvc"
            }
            else {
                Write-Host "Error: No suitable compiler found. Please install MSVC or Clang."
                exit 1
            }
        }
    }
}

# Select toolchain based on compiler
function Select-Toolchain {
    switch ($COMPILER) {
        "msvc" {
            $script:TOOLCHAIN = "msvc-windows"
        }
        "clang" {
            $script:TOOLCHAIN = "clang-windows"
        }
        default {
            Write-Host "Error: Unsupported compiler '$COMPILER'. Use msvc or clang."
            exit 1
        }
    }
}

# Select a CMake preset based on build type
function Select-Preset {
    $script:CONFIGURE_PRESET = "$GENERATOR-$TOOLCHAIN"

    switch ($BUILD_TYPE.ToLower()) {
        "debug" {
            $script:BUILD_PRESET = "$CONFIGURE_PRESET-debug"
        }
        "release" {
            $script:BUILD_PRESET = "$CONFIGURE_PRESET-release"
        }
        "relwithdebinfo" {
            $script:BUILD_PRESET = "$CONFIGURE_PRESET-reldebinfo"
        }
        default {
            Write-Host "Error: Unsupported build type: $BUILD_TYPE"
            exit 1
        }
    }

    Write-Host "-- CMake build preset: $BUILD_PRESET"
}

# Remove build directories if necessary
function Perform-Cleanup {
    if ($PRESERVE_BUILD -eq 0) {
        if (Test-Path "build") {
            Write-Host "-- Removing build directory"
            Start-Process -FilePath "powershell.exe" `
                -ArgumentList "-Command", "Remove-Item -Recurse -Force 'build'" `
                -NoNewWindow -Wait
        }
    }
    else {
        Write-Host "-- Preserving build directory"
    }
}

# Configure the build with CMake
function Configure-Build {
    Write-Host ""

    $cmakeArgs = @(
        "--preset", "$CONFIGURE_PRESET",
        "-DLINK_STATIC_MSVC_RT=$LINK_STATIC_MSVC_RT",
        "-DOPTIMIZE_FOR_CURRENT_CPU=$OPTIMIZE_NATIVE"
    )

    if ($ADDITIONAL_CMAKE_ARGS) {
        $cmakeArgs += $ADDITIONAL_CMAKE_ARGS
    }

    & cmake $cmakeArgs

    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed with exit code: $LASTEXITCODE"
    }
}

# Build the project using CMake
function Build-Project {
    $buildArgs = @(
        "--build",
        "--preset", "$BUILD_PRESET"
    )

    if ($JOBS -ne 0) {
        $buildArgs += "--parallel", "$JOBS"
    }

    if ($PRESERVE_BUILD -ne 0) {
        $buildArgs += "--clean-first"
    }

    & cmake $buildArgs

    if ($LASTEXITCODE -ne 0) {
        throw "CMake build failed with exit code: $LASTEXITCODE"
    }
}

Write-Host ""

# Build process
Check-Ninja
Check-CMake
Select-Generator
Select-Compiler
Select-Toolchain
Select-Preset
Perform-Cleanup
Configure-Build
Build-Project

Write-Host ""
Write-Host "Build completed successfully!"
Write-Host "Artifacts can be found in the 'bin' directory."
Write-Host ""
