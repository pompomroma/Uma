#!/bin/bash

# 3D Racing Game Build Script
# This script builds the racing game project

echo "Building 3D Racing Game..."

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project with CMake..."
BUILD_TYPE=${BUILD_TYPE:-Release}
if [ -n "$1" ]; then
  BUILD_TYPE="$1"
fi
echo "Using build type: $BUILD_TYPE"
cmake .. -DCMAKE_BUILD_TYPE=$BUILD_TYPE

# Build the project
echo "Building project..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "Build successful! Executable created: ./RacingGame3D"
echo "Run the game with: ./RacingGame3D"
if [ "$BUILD_TYPE" = "Debug" ]; then
  echo "Debug symbols are enabled. You can run gdb with: gdb ./RacingGame3D"
fi
else
    echo "Build failed!"
    exit 1
fi