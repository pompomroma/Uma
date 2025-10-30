#!/bin/bash

# iOS Distribution Build Script
# Creates a distributable iOS app bundle

set -e

echo "========================================="
echo "Building Racing Game 3D for iOS Distribution"
echo "========================================="

# Configuration
BUILD_TYPE=${1:-Release}
SCHEME="RacingGame3DiOS"
BUNDLE_ID="com.racinggame.mobile"
APP_NAME="Racing Game 3D"
VERSION="1.0.0"

echo "Build Type: $BUILD_TYPE"
echo "Bundle ID: $BUNDLE_ID"
echo "App Name: $APP_NAME"
echo "Version: $VERSION"

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS distribution builds must be performed on macOS"
    echo "Please run this script on a Mac with Xcode installed"
    exit 1
fi

# Check dependencies
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Install via: brew install cmake"
    exit 1
fi

# Create build directory
BUILD_DIR="build"
DIST_DIR="distribution"
mkdir -p "$BUILD_DIR"
mkdir -p "$DIST_DIR"

# Generate Xcode project
echo "Generating Xcode project..."
cmake -B "$BUILD_DIR" -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DPLATFORM_IOS=1 \
    -DPLATFORM_MOBILE=1

# Build for device
echo "Building for iOS device..."
xcodebuild -project "$BUILD_DIR/RacingGame3DiOS.xcodeproj" \
    -scheme "$SCHEME" \
    -configuration "$BUILD_TYPE" \
    -sdk iphoneos \
    -destination 'generic/platform=iOS' \
    -allowProvisioningUpdates \
    clean build

# Find the built app
APP_PATH=$(find "$BUILD_DIR" -name "RacingGame3DiOS.app" -type d | head -1)

if [ -z "$APP_PATH" ]; then
    echo "Error: Could not find built app bundle"
    exit 1
fi

echo "Found app at: $APP_PATH"

# Create distribution package
echo "Creating distribution package..."

# Copy app to distribution directory
cp -R "$APP_PATH" "$DIST_DIR/"

# Create IPA file (for manual installation)
echo "Creating IPA file..."
cd "$DIST_DIR"
mkdir -p Payload
cp -R RacingGame3DiOS.app Payload/
zip -r "RacingGame3DiOS.ipa" Payload/
rm -rf Payload/

# Create app bundle for direct installation
echo "Creating app bundle..."
APP_BUNDLE_DIR="RacingGame3DiOS.app"
if [ -d "$APP_BUNDLE_DIR" ]; then
    # Create a zip file for easy transfer
    zip -r "RacingGame3DiOS.zip" "$APP_BUNDLE_DIR"
    echo "Created: RacingGame3DiOS.zip"
fi

# Create installation instructions
cat > "INSTALLATION.md" << EOF
# Racing Game 3D - iOS Installation Guide

## Files Included:
- \`RacingGame3DiOS.app\` - Direct app bundle
- \`RacingGame3DiOS.ipa\` - iOS App Archive
- \`RacingGame3DiOS.zip\` - Compressed app bundle

## Installation Methods:

### Method 1: Xcode Installation (Recommended)
1. Open Xcode on your Mac
2. Connect your iOS device via USB
3. Open the project: \`../build/RacingGame3DiOS.xcodeproj\`
4. Select your device from the target menu
5. Sign in with your Apple ID in Xcode (Preferences > Accounts)
6. Select a Development Team in the project settings
7. Click Run (⌘R)

### Method 2: Direct App Bundle Installation
1. Transfer \`RacingGame3DiOS.app\` to your iOS device
2. Use a tool like 3uTools or iMazing to install the app
3. Trust the developer certificate in Settings > General > VPN & Device Management

### Method 3: IPA Installation
1. Use iTunes or Finder to install \`RacingGame3DiOS.ipa\`
2. Or use a tool like AltStore for sideloading
3. Trust the developer certificate when prompted

## Requirements:
- iOS 13.0 or later
- iPhone 6s or later, iPad Air 2 or later
- Metal-compatible graphics
- At least 100MB free storage

## Troubleshooting:
- If the app doesn't install, check that you have a valid Apple Developer account
- Make sure to trust the developer certificate in iOS Settings
- Ensure your device meets the minimum requirements

## Support:
For technical support or questions, contact the development team.
EOF

# Create web distribution files
echo "Creating web distribution files..."
cp ../distribution.html .
cp ../manifest.plist .

# Update manifest with correct URLs (placeholder)
sed -i '' 's|https://your-domain.com|https://your-actual-domain.com|g' manifest.plist

# Create a simple server script for local testing
cat > "serve.py" << 'EOF'
#!/usr/bin/env python3
import http.server
import socketserver
import os

PORT = 8000

class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        self.send_header('Access-Control-Allow-Methods', 'GET, POST, OPTIONS')
        self.send_header('Access-Control-Allow-Headers', 'Content-Type')
        super().end_headers()

    def do_GET(self):
        if self.path == '/':
            self.path = '/distribution.html'
        return super().do_GET()

if __name__ == "__main__":
    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    with socketserver.TCPServer(("", PORT), MyHTTPRequestHandler) as httpd:
        print(f"Serving at http://localhost:{PORT}")
        print("Open this URL on your iOS device to download the app")
        httpd.serve_forever()
EOF

chmod +x serve.py

cd ..

echo ""
echo "========================================="
echo "iOS Distribution Build Completed!"
echo "========================================="
echo ""
echo "Distribution files created in: ios/$DIST_DIR/"
echo ""
echo "Files created:"
echo "- RacingGame3DiOS.app (App bundle)"
echo "- RacingGame3DiOS.ipa (iOS App Archive)"
echo "- RacingGame3DiOS.zip (Compressed bundle)"
echo "- distribution.html (Web download page)"
echo "- manifest.plist (OTA installation manifest)"
echo "- INSTALLATION.md (Installation instructions)"
echo "- serve.py (Local web server for testing)"
echo ""
echo "To test web distribution locally:"
echo "1. cd ios/$DIST_DIR"
echo "2. python3 serve.py"
echo "3. Open http://localhost:8000 on your iOS device"
echo ""
echo "To install on device:"
echo "1. Open ios/$DIST_DIR/INSTALLATION.md for detailed instructions"
echo "2. Use Xcode for the easiest installation method"
echo ""
echo "For App Store distribution:"
echo "1. Archive the app in Xcode"
echo "2. Upload to App Store Connect"
echo "3. Submit for review"
echo "========================================="