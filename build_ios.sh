#!/bin/bash

# iOS build script
echo "Building Racing Game 3D for iOS..."

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS builds can only be created on macOS"
    exit 1
fi

# Check for Xcode
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed"
    exit 1
fi

# Set build configuration (Release or Debug)
CONFIGURATION=${1:-Release}
echo "Build configuration: $CONFIGURATION"

# Create build directory
BUILD_DIR="build-ios"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
echo "Configuring with CMake..."
cmake .. \
    -GXcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DIOS=1 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=12.0 \
    -DCMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH=NO \
    -DCMAKE_IOS_INSTALL_COMBINED=YES

# Build with xcodebuild
echo "Building with Xcode..."
xcodebuild \
    -project RacingGame3D.xcodeproj \
    -scheme RacingGame3D \
    -configuration $CONFIGURATION \
    -destination 'generic/platform=iOS' \
    -archivePath RacingGame3D.xcarchive \
    archive

# Export IPA
echo "Exporting IPA..."
xcodebuild \
    -exportArchive \
    -archivePath RacingGame3D.xcarchive \
    -exportPath Export \
    -exportOptionsPlist ../ios/ExportOptions.plist

echo "iOS build complete!"
echo "Archive: build-ios/RacingGame3D.xcarchive"
echo "IPA: build-ios/Export/RacingGame3D.ipa"

# Optional: Install to connected device
read -p "Install to connected iOS device? (y/n) " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    ios-deploy --bundle Export/RacingGame3D.app --debug
fi