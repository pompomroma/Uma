#!/bin/bash

# iOS build script for Racing Game 3D Mobile

set -e

echo "========================================="
echo "Building Racing Game 3D for iOS"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS builds must be performed on macOS"
    echo "Current OS: $OSTYPE"
    echo ""
    echo "To build for iOS, you need to:"
    echo "1. Transfer this project to a macOS machine"
    echo "2. Install Xcode from the App Store"
    echo "3. Run this script on macOS"
    echo ""
    echo "Alternatively, use a macOS-based CI/CD service like:"
    echo "- GitHub Actions with macOS runners"
    echo "- Bitrise"
    echo "- CircleCI with macOS executors"
    exit 1
fi

# Check if Xcode is installed
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed"
    echo "Please install Xcode from the App Store"
    exit 1
fi

# Check if Xcode command line tools are installed
if ! xcode-select -p &> /dev/null; then
    echo "Installing Xcode command line tools..."
    xcode-select --install
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
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
    -DCMAKE_IOS_INSTALL_COMBINED=YES

if [ $? -ne 0 ]; then
    echo "Error: Failed to generate Xcode project"
    exit 1
fi

# Build for device
echo "Building for iOS device..."
xcodebuild -project build/RacingGame3DiOS.xcodeproj \
    -scheme "$SCHEME" \
    -configuration "$BUILD_TYPE" \
    -sdk iphoneos \
    -destination 'generic/platform=iOS' \
    clean build

if [ $? -eq 0 ]; then
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
    echo "5. Set your Development Team in project settings"
    echo "6. Click Run (⌘R) to install and run on device"
    echo ""
    echo "Note: You'll need an Apple Developer account to run on physical devices"
    echo "========================================="
else
    echo ""
    echo "========================================="
    echo "Build failed! Check the error messages above."
    echo "Common issues:"
    echo "1. Missing Development Team - set in Xcode project settings"
    echo "2. Invalid provisioning profile"
    echo "3. Code signing issues"
    echo "========================================="
    exit 1
fi

cd ..
