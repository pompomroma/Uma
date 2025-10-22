#!/bin/bash

# Uma Racing - iOS Build Script
# This script builds the iOS racing game using CMake

set -e

echo "Building Uma Racing for iOS..."

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS builds require macOS with Xcode installed"
    exit 1
fi

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Please install CMake first."
    echo "You can install it via Homebrew: brew install cmake"
    exit 1
fi

# Check if Xcode is installed
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed. Please install Xcode from the App Store."
    exit 1
fi

# Create build directory
mkdir -p build/ios

# Configure CMake for iOS
cd build/ios

cmake ../.. \
    -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM="YOUR_TEAM_ID" \
    -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY="iPhone Developer" \
    -DIOS=ON

echo "CMake configuration complete."
echo ""
echo "To build the project:"
echo "1. Open build/ios/UmaRacing.xcodeproj in Xcode"
echo "2. Set your Development Team in the project settings"
echo "3. Connect your iOS device or select a simulator"
echo "4. Build and run the project (Cmd+R)"
echo ""
echo "Or build from command line:"
echo "xcodebuild -project UmaRacing.xcodeproj -scheme UmaRacing -configuration Release -destination 'generic/platform=iOS'"