#!/bin/bash

# iOS Distribution Build Script for Racing Game 3D
# This script creates a distributable .ipa file for App Store or TestFlight

set -e

echo "========================================="
echo "Racing Game 3D - iOS Distribution Build"
echo "========================================="

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS distribution builds must be performed on macOS"
    exit 1
fi

# Check for required tools
if ! command -v xcodebuild &> /dev/null; then
    echo "Error: Xcode is not installed"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "Error: CMake is not installed. Install via: brew install cmake"
    exit 1
fi

# Configuration
SCHEME="RacingGame3DiOS"
CONFIGURATION="Release"
ARCHIVE_PATH="ios/build/RacingGame3DiOS.xcarchive"
EXPORT_PATH="ios/build/Export"
PROJECT_PATH="ios/build/RacingGame3DiOS.xcodeproj"

# Distribution type
DISTRIBUTION_TYPE=${1:-testflight}

if [[ "$DISTRIBUTION_TYPE" == "appstore" ]]; then
    EXPORT_OPTIONS_PLIST="ios/ExportOptions.plist"
    echo "Building for App Store distribution..."
elif [[ "$DISTRIBUTION_TYPE" == "testflight" ]]; then
    EXPORT_OPTIONS_PLIST="ios/ExportOptionsTestFlight.plist"
    echo "Building for TestFlight distribution..."
else
    echo "Invalid distribution type. Use 'testflight' or 'appstore'"
    exit 1
fi

# Clean previous builds
echo ""
echo "Cleaning previous builds..."
rm -rf "$ARCHIVE_PATH"
rm -rf "$EXPORT_PATH"

# Generate Xcode project if it doesn't exist
if [ ! -d "$PROJECT_PATH" ]; then
    echo ""
    echo "Generating Xcode project..."
    cd ios
    cmake -B build -G Xcode \
        -DCMAKE_SYSTEM_NAME=iOS \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
        -DCMAKE_OSX_ARCHITECTURES=arm64 \
        -DCMAKE_BUILD_TYPE=Release \
        -DPLATFORM_IOS=1 \
        -DPLATFORM_MOBILE=1
    cd ..
fi

# Archive the app
echo ""
echo "Archiving application..."
xcodebuild archive \
    -project "$PROJECT_PATH" \
    -scheme "$SCHEME" \
    -configuration "$CONFIGURATION" \
    -archivePath "$ARCHIVE_PATH" \
    -destination 'generic/platform=iOS' \
    -allowProvisioningUpdates \
    CODE_SIGN_STYLE=Automatic

# Check if archive was successful
if [ ! -d "$ARCHIVE_PATH" ]; then
    echo "Error: Archive creation failed"
    exit 1
fi

echo ""
echo "Archive created successfully!"

# Export the .ipa file
echo ""
echo "Exporting .ipa file..."
xcodebuild -exportArchive \
    -archivePath "$ARCHIVE_PATH" \
    -exportPath "$EXPORT_PATH" \
    -exportOptionsPlist "$EXPORT_OPTIONS_PLIST" \
    -allowProvisioningUpdates

# Check if export was successful
if [ ! -f "$EXPORT_PATH/RacingGame3DiOS.ipa" ]; then
    echo "Error: IPA export failed"
    exit 1
fi

echo ""
echo "========================================="
echo "Distribution build completed successfully!"
echo "========================================="
echo ""
echo "IPA Location: $EXPORT_PATH/RacingGame3DiOS.ipa"
echo ""

if [[ "$DISTRIBUTION_TYPE" == "testflight" ]]; then
    echo "Next Steps for TestFlight:"
    echo "1. Open Transporter app (or use xcrun altool)"
    echo "2. Upload the .ipa file to App Store Connect"
    echo "3. Go to App Store Connect > TestFlight"
    echo "4. Wait for processing (10-30 minutes)"
    echo "5. Add internal/external testers"
    echo "6. Testers will receive TestFlight invitation"
    echo ""
    echo "Quick upload command:"
    echo "xcrun altool --upload-app --type ios --file \"$EXPORT_PATH/RacingGame3DiOS.ipa\" --username YOUR_APPLE_ID --password YOUR_APP_SPECIFIC_PASSWORD"
else
    echo "Next Steps for App Store:"
    echo "1. Open Transporter app (or use xcrun altool)"
    echo "2. Upload the .ipa file to App Store Connect"
    echo "3. Go to App Store Connect > My Apps"
    echo "4. Select your app and fill in metadata"
    echo "5. Submit for App Review"
    echo "6. Wait for approval (1-3 days typically)"
    echo "7. App will be available on App Store"
fi

echo ""
echo "For automatic upload using command line:"
echo "xcrun altool --upload-app --type ios \\"
echo "  --file \"$EXPORT_PATH/RacingGame3DiOS.ipa\" \\"
echo "  --username YOUR_APPLE_ID \\"
echo "  --password YOUR_APP_SPECIFIC_PASSWORD"
echo ""
echo "Note: App-specific password can be generated at appleid.apple.com"
echo "========================================="
