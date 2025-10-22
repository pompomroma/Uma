#!/bin/bash

# 3D Racing Game Build Script
# This script builds the racing game project
#
# Usage:
#   ./build.sh          - Build in Debug mode (default)
#   ./build.sh release  - Build in Release mode
#   ./build.sh debug    - Build in Debug mode (explicit)

# Determine build type
BUILD_TYPE="Debug"
if [ "$1" = "release" ] || [ "$1" = "Release" ]; then
    BUILD_TYPE="Release"
elif [ "$1" = "debug" ] || [ "$1" = "Debug" ]; then
    BUILD_TYPE="Debug"
elif [ -n "$1" ]; then
    echo "Unknown build type: $1"
    echo "Usage: $0 [debug|release]"
    exit 1
fi

echo "Building 3D Racing Game in $BUILD_TYPE mode..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "Build successful! ($BUILD_TYPE mode)"
    echo "========================================="
    echo "Executable: ./build/RacingGame3D"
    echo ""
    if [ "$BUILD_TYPE" = "Debug" ]; then
        echo "Debug build created with:"
        echo "  - Debug symbols enabled (-g)"
        echo "  - No optimization (-O0)"
        echo "  - Debug assertions enabled"
        echo ""
        echo "To debug with gdb:"
        echo "  gdb ./build/RacingGame3D"
        echo ""
        echo "To debug with lldb:"
        echo "  lldb ./build/RacingGame3D"
    else
        echo "Release build created with:"
        echo "  - Optimizations enabled (-O3)"
        echo "  - No debug symbols"
    fi
    echo "========================================="
else
    echo "Build failed!"
    exit 1
fi