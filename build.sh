#!/bin/bash

# 3D Racing Game Build Script
# This script builds the racing game project

# Parse command line arguments
BUILD_TYPE="Debug"
CLEAN=false
VERBOSE=false

while [[ $# -gt 0 ]]; do
    case $1 in
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Options:"
            echo "  --release    Build in release mode (optimized)"
            echo "  --debug      Build in debug mode (default, with debug symbols)"
            echo "  --clean      Clean build directory before building"
            echo "  --verbose    Enable verbose output"
            echo "  -h, --help   Show this help message"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            echo "Use --help for usage information"
            exit 1
            ;;
    esac
done

echo "Building 3D Racing Game in $BUILD_TYPE mode..."

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    echo "Cleaning build directory..."
    rm -rf build
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
CMAKE_ARGS="-DCMAKE_BUILD_TYPE=$BUILD_TYPE"

if [ "$VERBOSE" = true ]; then
    CMAKE_ARGS="$CMAKE_ARGS -DCMAKE_VERBOSE_MAKEFILE=ON"
fi

cmake .. $CMAKE_ARGS

# Build the project
echo "Building project..."
MAKE_ARGS="-j$(nproc)"

if [ "$VERBOSE" = true ]; then
    MAKE_ARGS="$MAKE_ARGS VERBOSE=1"
fi

make $MAKE_ARGS

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created: ./RacingGame3D"
    echo "Build type: $BUILD_TYPE"
    if [ "$BUILD_TYPE" = "Debug" ]; then
        echo "Debug symbols included for debugging with GDB/LLDB"
        echo "Run with debugger: gdb ./RacingGame3D"
    fi
    echo "Run the game with: ./RacingGame3D"
else
    echo "Build failed!"
    exit 1
fi