#!/bin/bash

# Android build script for Racing Game 3D Mobile

set -e

echo "========================================="
echo "Building Racing Game 3D for Android"
echo "========================================="

# Check if ANDROID_NDK is set
if [ -z "$ANDROID_NDK" ]; then
    echo "Error: ANDROID_NDK environment variable is not set"
    echo "Please set it to your Android NDK path:"
    echo "export ANDROID_NDK=/path/to/android-ndk"
    exit 1
fi

# Check if ANDROID_SDK is set
if [ -z "$ANDROID_HOME" ]; then
    echo "Warning: ANDROID_HOME environment variable is not set"
    echo "Please set it to your Android SDK path for full functionality"
fi

# Configuration
BUILD_TYPE=${1:-Release}
ABI=${2:-arm64-v8a}

echo "Build Type: $BUILD_TYPE"
echo "ABI: $ABI"

# Create build directory
BUILD_DIR="android/build"
mkdir -p "$BUILD_DIR"

cd android

# Build using Gradle (if available)
if command -v gradle &> /dev/null; then
    echo "Building with Gradle..."
    gradle clean
    gradle assemble${BUILD_TYPE}
    
    echo ""
    echo "========================================="
    echo "Build completed successfully!"
    echo "APK location: android/build/outputs/apk/${BUILD_TYPE,,}/"
    echo "========================================="
else
    echo "Gradle not found. Building with CMake only..."
    
    # Build with CMake
    cd "$BUILD_DIR"
    
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
        -DANDROID_ABI="$ABI" \
        -DANDROID_PLATFORM=android-24 \
        -DANDROID_STL=c++_shared \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    
    make -j$(nproc)
    
    echo ""
    echo "========================================="
    echo "CMake build completed!"
    echo "Library location: android/build/"
    echo "Note: Use Gradle to create a complete APK"
    echo "========================================="
fi

cd ../..
