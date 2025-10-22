#!/bin/bash

# 3D Racing Game Build Script
# This script builds the racing game project

echo "Building 3D Racing Game..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created: ./RacingGame3D"
    echo "Run the game with: ./RacingGame3D"
else
    echo "Build failed!"
    exit 1
fi