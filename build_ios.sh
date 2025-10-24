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
    # On non-macOS, CMake can't generate Xcode projects
    # Prepare a clean build directory and instructions
    echo "Running on non-macOS system. Preparing iOS project for macOS transfer..."
    
    # Create build directory
    rm -rf build
    mkdir -p build
    
    # Create a README with instructions
    cat > build/README_MAC_BUILD.txt << 'EOF'
iOS Build Instructions for macOS
=================================

This project must be built on macOS with Xcode installed.

Prerequisites:
- macOS with Xcode 14.0 or later
- CMake 3.18 or later (install via: brew install cmake)
- iOS 13.0+ device or simulator

Steps to Build:
1. Ensure you have transferred the ENTIRE project folder to your Mac
   (not just the ios/build folder)

2. Open Terminal and navigate to the project's ios folder:
   cd /path/to/RacingGame3D/ios

3. Run the build script:
   ./build_ios.sh

   This will:
   - Generate the Xcode project: ios/build/RacingGame3DiOS.xcodeproj
   - Build the app for iOS device

4. To open in Xcode:
   open build/RacingGame3DiOS.xcodeproj

5. In Xcode:
   - Connect your iOS device
   - Select your device from the target menu
   - Select a Development Team in Signing & Capabilities
   - Click Run (⌘R) to build and install

Alternative - Generate project only (without building):
   cd ios
   cmake -B build -G Xcode \
       -DCMAKE_SYSTEM_NAME=iOS \
       -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
       -DCMAKE_OSX_ARCHITECTURES=arm64 \
       -DPLATFORM_IOS=1 \
       -DPLATFORM_MOBILE=1
   
   open build/RacingGame3DiOS.xcodeproj

For more information, see the project's IOS_BUILD_GUIDE.md
EOF
    
    echo "✓ Build preparation complete"
    echo ""
    echo "Created: ios/build/README_MAC_BUILD.txt"
fi

if [[ "$OSTYPE" == "darwin"* ]]; then
    # Verify xcodeproj was created
    if [ -f "build/RacingGame3DiOS.xcodeproj/project.pbxproj" ]; then
        echo "✓ Xcode project generated successfully: build/RacingGame3DiOS.xcodeproj"
        
        # Build for device
        echo ""
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
        echo "Error: Failed to generate Xcode project"
        exit 1
    fi
else
    echo ""
    echo "========================================="
    echo "iOS Project Ready for macOS Build"
    echo "========================================="
    echo ""
    echo "⚠️  iOS apps can only be built on macOS with Xcode"
    echo ""
    echo "To build this project on a Mac:"
    echo ""
    echo "1. Transfer the ENTIRE project folder to a Mac:"
    echo "   - Via network: scp -r $(dirname $(pwd)) user@mac:~/"
    echo "   - Or use: USB drive, AirDrop, GitHub, etc."
    echo ""
    echo "2. On the Mac, open Terminal and run:"
    echo "   cd RacingGame3D/ios"
    echo "   ./build_ios.sh"
    echo ""
    echo "3. This will generate: ios/build/RacingGame3DiOS.xcodeproj"
    echo ""
    echo "4. Open and run in Xcode:"
    echo "   open build/RacingGame3DiOS.xcodeproj"
    echo ""
    echo "📄 Detailed instructions: ios/build/README_MAC_BUILD.txt"
    echo "========================================="
fi

cd ..
