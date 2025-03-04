# Import the assembly for using Windows Forms dialog
Add-Type -AssemblyName System.Windows.Forms

# Formats JSON in a nicer format than the built-in ConvertTo-Json does
function Format-Json([Parameter(Mandatory, ValueFromPipeline)][String] $json) {
    $indent = 0
    ($json -Split '\n' |
        % {
          if ($_ -match '[\}\]]') {
            # This line contains  ] or }, decrement the indentation level
            $indent--
          }
          $line = (' ' * $indent * 2) + $_.TrimStart().Replace(':  ', ': ')
          if ($_ -match '[\{\[]') {
            # This line contains [ or {, increment the indentation level
            $indent++
          }
          $line
    }) -Join "`n"
}

# Checks if a file exists and prompts for overwrite confirmation
function Confirm-Overwrite {
    param (
        [string]$filePath
    )

    if (Test-Path -Path $filePath) {
        $response = Read-Host "File '$filePath' already exists. Overwrite? (y/n)"
        return ($response -eq "y" -or $response -eq "Y")
    }

    return $true
}

# Select the hlds.exe file using a dialog
function Select-HldsExe {
    $dialog = New-Object System.Windows.Forms.OpenFileDialog
    $dialog.Filter = "EXE files (*.exe)|*.exe"
    $dialog.Title = "Select hlds.exe file"

    if ($dialog.ShowDialog() -eq [System.Windows.Forms.DialogResult]::OK) {
        return $dialog.FileName
    }

    return $null
}

# Create the launch.vs.json configuration
function New-LaunchConfig {
    [CmdletBinding(SupportsShouldProcess=$true)]
    param (
        [string]$hldsExePath,
        [string]$hldsExeDir
    )

    # Create the ".vs" directory if it does not exist
    New-Item -Path ".vs" -ItemType Directory -Force | Out-Null

    $launchConfigName = "launch.vs.json"
    $launchConfigPath = ".vs\$launchConfigName"
    $launchConfigContent = [ordered]@{
        version = "0.2.1"
        configurations = @(
            [ordered]@{
                name = "ReHLDS"
                type = "dll"
                debugType = "native"
                project = "CMakeLists.txt"
                currentDir = $hldsExeDir
                exe = $hldsExePath
                args = @(
                    "-game cstrike",
                    "-insecure",
                    "-noipx",
                    "-maxplayers 32",
                    "+sys_ticrate 0",
                    "+map de_dust2"
                )
            }
        )
    }

    if (Confirm-Overwrite -filePath $launchConfigPath) {
        if ($PSCmdlet.ShouldProcess($launchConfigPath, "Create/Update launch configuration file")) {
            $launchConfigContent | ConvertTo-Json -Depth 3 | Format-Json | Set-Content -Path $launchConfigPath
        }
    }
    else {
        Write-Verbose "Operation canceled. File '$launchConfigName' was not overwritten."
    }
}

# Create the CMakeUserPresets.json configuration
function New-CMakeUserPresets {
    [CmdletBinding(SupportsShouldProcess=$true)]
    param (
        [string]$hldsExeDir
    )

    $binOutputDir = "$hldsExeDir\cstrike\addons\revoice"
    $cmakeUserPresetsFile = "CMakeUserPresets.json"
    $cmakeUserPresetsContent = [ordered]@{
        version = 3
        configurePresets = @(
            [ordered]@{
                name = "ninja-msvc-windows-user"
                inherits = @(
                    "ninja-msvc-windows"
                )
                cacheVariables = @{
                    CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG = "$binOutputDir"
                    CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG = "$binOutputDir"
                }
            }
        )
        buildPresets = @(
            [ordered]@{
                name = "ninja-msvc-windows-debug-user"
                displayName = "[ReHLDS] Ninja MSVC Debug"
                description = "Build using Ninja Multi-Config generator and MSVC compiler with Debug configuration"
                configurePreset = "ninja-msvc-windows-user"
                configuration = "Debug"
                inherits = @(
                    "ninja-msvc-windows-debug"
                )
            }
        )
    }

    if (Confirm-Overwrite -filePath $cmakeUserPresetsFile) {
        if ($PSCmdlet.ShouldProcess($cmakeUserPresetsFile, "Create/Update CMakeUserPresets file")) {
            $cmakeUserPresetsContent | ConvertTo-Json -Depth 3 | Format-Json | Set-Content -Path $cmakeUserPresetsFile
        }
    }
    else {
        Write-Verbose "Operation canceled. File '$cmakeUserPresetsFile' was not overwritten."
    }
}

$hldsExePath = Select-HldsExe

if ($null -ne $hldsExePath) {
    $hldsExeDir = Split-Path -Path $hldsExePath -Parent
    New-LaunchConfig -hldsExePath $hldsExePath -hldsExeDir $hldsExeDir
    New-CMakeUserPresets -hldsExeDir $hldsExeDir
}
else {
    Write-Verbose "The hlds.exe file was not selected."
    exit 1
}
