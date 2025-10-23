#!/bin/bash

# iOS build script for Racing Game 3D Mobile

set -e

echo "========================================="
echo "Building Racing Game 3D for iOS"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "========================================="
    echo "iOS builds require macOS with Xcode"
    echo "========================================="
    echo ""
    echo "To build for iOS:"
    echo "1. Copy this project to a macOS machine"
    echo "2. Install Xcode from the App Store"
    echo "3. Run: ./build_ios.sh"
    echo ""
    echo "For now, we'll create a mock iOS build structure..."
    echo "========================================="
    
    # Create mock iOS build structure
    mkdir -p ios/build/Release-iphoneos
    echo "Mock iOS app bundle created at: ios/build/Release-iphoneos/"
    echo "This is a placeholder - actual iOS build requires macOS + Xcode"
    exit 0
else
    # Check if Xcode is installed on macOS
    if ! command -v xcodebuild &> /dev/null; then
        echo "Error: Xcode is not installed"
        exit 1
    fi
fi

# Configuration
BUILD_TYPE=${1:-Release}
SCHEME="RacingGame3DiOS"

echo "Build Type: $BUILD_TYPE"

# Create build directory
BUILD_DIR="ios/build"
mkdir -p "$BUILD_DIR"

cd ios

# Generate project with CMake
echo "Generating iOS project..."

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Use Xcode generator on macOS
    cmake -B build -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE"
    
    # Build for device
    echo "Building for iOS device..."
    xcodebuild -project build/RacingGame3DiOS.xcodeproj \
        -scheme "$SCHEME" \
        -configuration "$BUILD_TYPE" \
        -sdk iphoneos \
        -destination 'generic/platform=iOS' \
        clean build
else
    # Use Makefile generator for cross-compilation
    cmake -B build -G "Unix Makefiles" \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DCMAKE_C_COMPILER=clang \
        -DCMAKE_CXX_COMPILER=clang++ \
        -DCMAKE_C_FLAGS="-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk" \
        -DCMAKE_CXX_FLAGS="-arch arm64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk"
    
    # Build
    echo "Building for iOS device..."
    make -C build -j$(nproc)
fi

echo ""
echo "========================================="
echo "iOS build completed successfully!"
echo "App bundle location: ios/build/$BUILD_TYPE-iphoneos/"
echo ""
echo "To install on device:"
echo "1. Open Xcode"
echo "2. Open the project: ios/build/RacingGame3DiOS.xcodeproj"
echo "3. Connect your iOS device"
echo "4. Select your device from the target menu"
echo "5. Click Run (⌘R)"
echo "========================================="

cd ..
