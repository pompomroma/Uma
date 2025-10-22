#!/bin/bash

# iOS build script for Racing Game 3D Mobile

set -e

echo "========================================="
echo "Building Racing Game 3D for iOS"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS builds must be performed on macOS"
    exit 1
fi

# Check if Xcode is installed
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed"
    exit 1
fi

# Configuration
BUILD_TYPE=${1:-Release}
SCHEME="RacingGame3DiOS"

echo "Build Type: $BUILD_TYPE"

# Create build directory
BUILD_DIR="ios/build"
mkdir -p "$BUILD_DIR"

cd ios

# Generate Xcode project with CMake
echo "Generating Xcode project..."
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
