#!/bin/bash
set -e

# Default values
JOBS=0
GENERATOR="ninja"
BUILD_TYPE="Release"
PRESERVE_BUILD=0
LINK_STATIC_GCC=OFF
LINK_STATIC_STDCPP=OFF
OPTIMIZE_NATIVE=OFF
ADDITIONAL_CMAKE_ARGS=""

# Display help message
show_help() {
    cat << EOF
Usage: tools/linux/build.sh [options]

Options:
  -h, --help
        Show this help message and exit.

  -b, --build-type <type>
        Set the build type (Debug, Release, RelWithDebInfo) (default: Release).

  -c, --compiler <name>
        Specify the compiler to use (gcc or clang) (default: auto-detected).

  -j, --jobs <n>
        Number of parallel jobs for the build (default: number of CPU cores).

  -k, --keep-build
        Preserve the build directory; does not delete it before building.

  -o, --optimize
        Enable optimizations for the current CPU (equivalent to compiler flag -march=native).

  -s, --static-runtime
        Enable static linking of runtime libraries (e.g., libgcc, libstdc++).

  -- <args>
        Pass remaining arguments directly to CMake.

Examples:
  tools/linux/build.sh
        Automatically detect the compiler and build with default settings.

  tools/linux/build.sh -c gcc -b debug
        Build using the GCC compiler with the Debug build type.

  tools/linux/build.sh --compiler clang --optimize
        Build using the Clang compiler with optimizations enabled for the current CPU.

  tools/linux/build.sh -- -DENABLE_SPEEX_VORBIS_PSY=ON
        Automatically detect the compiler and pass additional options to CMake.
EOF
}

# Parse command-line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help)
            show_help
            exit 0
            ;;
        -b|--build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        -c|--compiler)
            COMPILER="$2"
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -k|--keep-build)
            PRESERVE_BUILD=1
            shift
            ;;
        -o|--optimize)
            OPTIMIZE_NATIVE=ON
            shift
            ;;
        -s|--static-runtime)
            LINK_STATIC_GCC=ON
            LINK_STATIC_STDCPP=ON
            shift
            ;;
        --)
            shift
            ADDITIONAL_CMAKE_ARGS="$*"
            break
            ;;
        *)
            echo "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Check availability of Ninja build system
check_ninja() {
    if ! command -v ninja &> /dev/null; then
        echo "Error: Ninja build system not found. Please install Ninja build."
        exit 1
    fi

    echo "-- Found Ninja $(ninja --version)"
}

# Check CMake availability and minimum version
check_cmake() {
    local required_major=3
    local required_minor=21

    if ! command -v cmake &> /dev/null; then
        echo "Error: CMake not found. Please install CMake."
        exit 1
    fi

    local cmake_version
    cmake_version=$(cmake --version | head -n1 | sed 's/cmake version //')
    echo "-- Found CMake version $cmake_version"

    local cmake_major cmake_minor
    cmake_major=$(echo "$cmake_version" | cut -d. -f1)
    cmake_minor=$(echo "$cmake_version" | cut -d. -f2)

    if (( cmake_major < required_major || (cmake_major == required_major && cmake_minor < required_minor) )); then
        echo "Error: CMake version $required_major.$required_minor or higher is required. Please update CMake."
        exit 1
    fi
}

# Select a compiler to use
select_compiler() {
    if [[ -z "$COMPILER" ]]; then
        if command -v gcc &> /dev/null; then
            COMPILER="gcc"
        elif command -v clang &> /dev/null; then
            COMPILER="clang"
        else
            echo "Error: No suitable compiler found. Please install GCC or Clang."
            exit 1
        fi
    fi
}

# Select toolchain based on compiler
select_toolchain() {
    case "$COMPILER" in
        gcc)
            if ! command -v gcc &> /dev/null; then
                echo "Error: GCC not found."
                exit 1
            fi
            TOOLCHAIN="gcc-linux"
            ;;
        clang)
            if ! command -v clang &> /dev/null; then
                echo "Error: Clang not found."
                exit 1
            fi
            TOOLCHAIN="clang-linux"
            ;;
        *)
            echo "Error: Unsupported compiler '$COMPILER'. Use gcc or clang."
            exit 1
            ;;
    esac
}

# Select a CMake preset based on build type
select_preset() {
    CONFIGURE_PRESET="$GENERATOR-$TOOLCHAIN"

    local build_type_lower
    build_type_lower=$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')

    case "$build_type_lower" in
        debug)
            BUILD_PRESET="$CONFIGURE_PRESET-debug"
            ;;
        release)
            BUILD_PRESET="$CONFIGURE_PRESET-release"
            ;;
        relwithdebinfo)
            BUILD_PRESET="$CONFIGURE_PRESET-reldebinfo"
            ;;
        *)
            echo "Error: Unsupported build type: $BUILD_TYPE"
            exit 1
            ;;
    esac

    echo "-- CMake build preset: $BUILD_PRESET"
}

# Remove build directories if necessary
perform_cleanup() {
    if [ "$PRESERVE_BUILD" -eq 0 ]; then
        if [ -d "build" ]; then
            echo "-- Removing build directory"
            rm -rf build
        fi
    else
        echo "-- Preserving build directory"
    fi
}

# Configure the build with CMake
configure_build() {
    echo ""
    cmake \
        --preset "$CONFIGURE_PRESET" \
        -DLINK_STATIC_GCC="$LINK_STATIC_GCC" \
        -DLINK_STATIC_STDCPP="$LINK_STATIC_STDCPP" \
        -DOPTIMIZE_FOR_CURRENT_CPU="$OPTIMIZE_NATIVE" \
        ${ADDITIONAL_CMAKE_ARGS:+"$ADDITIONAL_CMAKE_ARGS"}
}

# Build the project using CMake
build_project() {
    local args=(
        --build
        --preset "$BUILD_PRESET"
    )

    if [ "$JOBS" -ne 0 ]; then
        args+=(--parallel "$JOBS")
    fi

    if [ "$PRESERVE_BUILD" -ne 0 ]; then
        args+=(--clean-first)
    fi

    cmake "${args[@]}"
}

echo ""

# Build process
check_ninja
check_cmake
select_compiler
select_toolchain
select_preset
perform_cleanup
configure_build
build_project

echo ""
echo "Build completed successfully!"
echo "Artifacts can be found in the \"bin\" directory."
echo ""
