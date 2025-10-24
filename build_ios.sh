#!/bin/bash

# iOS build script for Racing Game 3D Mobile

set -e

echo "========================================="
echo "Building Racing Game 3D for iOS"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Warning: iOS builds should be performed on macOS"
    echo "Generating Xcode project anyway for reference..."
fi

# Check if Xcode is installed (only on macOS)
if [[ "$OSTYPE" == "darwin"* ]]; then
    if ! command -v xcodebuild &> /dev/null; then
        echo "Error: Xcode is not installed"
        exit 1
    fi
    
    if ! command -v cmake &> /dev/null; then
        echo "Error: CMake is not installed. Install via: brew install cmake"
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

# Generate Xcode project with CMake
echo "Generating Xcode project..."
# On macOS we can generate an actual Xcode project. On non-macOS we still
# generate into ios/build, but only for reference and without iOS SDKs.
if [[ "$OSTYPE" == "darwin"* ]]; then
    # Use Xcode generator on macOS
    cmake -B build -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
        -DPLATFORM_IOS=1 \
        -DPLATFORM_MOBILE=1
else
    # On non-macOS, ensure the output path exists and place a helpful
    # placeholder so instructions don't point to a missing file.
    mkdir -p build
    PLACEHOLDER="build/RacingGame3DiOS.xcodeproj"
    if [[ ! -d "$PLACEHOLDER" ]]; then
        mkdir -p "$PLACEHOLDER"
        cat > "$PLACEHOLDER/READ_THIS_FIRST.txt" <<EOF
This is a placeholder created on a non-macOS environment.

To generate a real Xcode project, run this script on a Mac with Xcode and CMake installed.
Expected location on macOS: ios/build/RacingGame3DiOS.xcodeproj
EOF
    fi
    echo ""
    echo "Note: Cross-compilation to iOS from non-macOS is not supported."
    echo "A placeholder has been created at ios/$PLACEHOLDER to guide you."
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Build for device
    echo "Building for iOS device..."
    xcodebuild -project build/RacingGame3DiOS.xcodeproj \
        -scheme "$SCHEME" \
        -configuration "$BUILD_TYPE" \
        -sdk iphoneos \
        -destination 'generic/platform=iOS' \
        -allowProvisioningUpdates \
        clean build
    
    echo ""
    echo "========================================="
    echo "iOS build completed successfully!"
    echo "App bundle location: ios/build/$BUILD_TYPE-iphoneos/"
    echo ""
    echo "To install on device:"
    echo "1. Open Xcode"
    echo "2. Open the project: ios/build/RacingGame3DiOS.xcodeproj"
    echo "3. Connect your iOS device via USB"
    echo "4. Sign in with your Apple ID in Xcode (Preferences > Accounts)"
    echo "5. Select your device from the target menu"
    echo "6. Select a Development Team in the project settings"
    echo "7. Click Run (⌘R)"
    echo ""
    echo "For installing on your iPhone:"
    echo "- Connect iPhone via USB"
    echo "- Trust this computer on iPhone if prompted"
    echo "- In iPhone Settings > General > VPN & Device Management"
    echo "  trust the developer certificate"
    echo "========================================="
else
    echo ""
    echo "========================================="
    echo "Xcode project placeholder prepared."
    echo "When run on macOS, the real project will be created at:"
    echo "ios/build/RacingGame3DiOS.xcodeproj"
    echo ""
    echo "Transfer this project to a Mac and run this script again to build."
    echo "========================================="
fi

cd ..
