#!/bin/bash

# Debug Build Script for 3D Racing Game
# This script builds the project in debug mode with all debugging features enabled

echo "Building 3D Racing Game in DEBUG mode..."

# Clean build directory
echo "Cleaning build directory..."
rm -rf build

# Create build directory
mkdir -p build
cd build

# Configure with CMake for debug build
echo "Configuring project with CMake for DEBUG build..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_VERBOSE_MAKEFILE=ON \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build the project
echo "Building project with debug symbols..."
make -j$(nproc) VERBOSE=1

# Check if build was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ DEBUG BUILD SUCCESSFUL!"
    echo "📁 Executable created: ./build/RacingGame3D"
    echo ""
    echo "🐛 DEBUGGING OPTIONS:"
    echo "  • Run with GDB:     gdb ./RacingGame3D"
    echo "  • Run with Valgrind: valgrind --tool=memcheck --leak-check=full ./RacingGame3D"
    echo "  • Run with LLDB:    lldb ./RacingGame3D"
    echo ""
    echo "🔧 DEBUG FEATURES ENABLED:"
    echo "  • Debug symbols (-g)"
    echo "  • No optimization (-O0)"
    echo "  • Debug assertions (DEBUG_BUILD defined)"
    echo "  • Debug console output"
    echo "  • Memory debugging support"
    echo ""
    echo "▶️  Run normally: ./RacingGame3D"
else
    echo "❌ BUILD FAILED!"
    echo "Check the error messages above for details."
    exit 1
fi