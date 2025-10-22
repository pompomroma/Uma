#!/bin/bash

# 3D Racing Game Build Script
# This script builds the racing game project

echo "Building 3D Racing Game..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
BUILD_TYPE=${1:-Debug}
echo "Configuring project with CMake (BUILD_TYPE=$BUILD_TYPE)..."
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created: ./RacingGame3D"
    echo "Run the game with: ./RacingGame3D"
    echo "Tip: pass 'Release' to this script for optimized builds."
else
    echo "Build failed!"
    exit 1
fi