#!/bin/bash

# Demo script showing complete iOS distribution process
# This script demonstrates how to make the game downloadable and playable independently

set -e

echo "========================================="
echo "Racing Game 3D - iOS Distribution Demo"
echo "========================================="
echo ""

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "⚠️  This demo requires macOS with Xcode"
    echo "   The app can still be built for iOS on other platforms"
    echo "   but distribution requires macOS."
    echo ""
    echo "   To test distribution on non-macOS:"
    echo "   1. Transfer the project to a Mac"
    echo "   2. Run this script there"
    echo "   3. Or use the generated Xcode project"
    echo ""
    exit 0
fi

# Check dependencies
echo "🔍 Checking dependencies..."

if ! command -v xcodebuild &> /dev/null; then
    echo "❌ Xcode is not installed"
    echo "   Please install Xcode from the App Store"
    exit 1
fi

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake is not installed"
    echo "   Install via: brew install cmake"
    exit 1
fi

if ! command -v python3 &> /dev/null; then
    echo "❌ Python 3 is not installed"
    echo "   Install via: brew install python3"
    exit 1
fi

echo "✅ All dependencies found"
echo ""

# Step 1: Generate app icons
echo "🎨 Step 1: Generating app icons..."
if [ -f "generate_icons.py" ]; then
    python3 generate_icons.py
    echo "✅ App icons generated"
else
    echo "⚠️  Icon generation script not found"
fi
echo ""

# Step 2: Build the app
echo "🔨 Step 2: Building iOS app..."
if [ -f "build_ios.sh" ]; then
    ./build_ios.sh Release
    echo "✅ App built successfully"
else
    echo "❌ Build script not found"
    exit 1
fi
echo ""

# Step 3: Create distribution package
echo "📦 Step 3: Creating distribution package..."
if [ -f "build_distribution.sh" ]; then
    ./build_distribution.sh Release
    echo "✅ Distribution package created"
else
    echo "❌ Distribution script not found"
    exit 1
fi
echo ""

# Step 4: Show distribution options
echo "🚀 Step 4: Distribution Options"
echo "========================================="
echo ""
echo "The app is now ready for independent iOS distribution!"
echo ""
echo "📱 Distribution Methods Available:"
echo ""
echo "1. 🖥️  Xcode Installation (Easiest for developers)"
echo "   - Open: ios/build/RacingGame3DiOS.xcodeproj"
echo "   - Connect iOS device via USB"
echo "   - Click Run (⌘R)"
echo ""
echo "2. 🧪 TestFlight Beta (Recommended for testing)"
echo "   - Archive app in Xcode"
echo "   - Upload to App Store Connect"
echo "   - Add testers via TestFlight"
echo "   - Users install via TestFlight app"
echo ""
echo "3. 🏪 App Store (Public release)"
echo "   - Complete TestFlight testing"
echo "   - Submit for App Store review"
echo "   - Once approved, publicly available"
echo ""
echo "4. 🌐 Web Distribution (Enterprise/Ad-hoc)"
echo "   - Host files on web server"
echo "   - Users visit distribution URL"
echo "   - Tap 'Install' button"
echo ""

# Step 5: Test web distribution locally
echo "🧪 Step 5: Testing Web Distribution Locally"
echo "========================================="
echo ""
echo "To test web distribution:"
echo "1. Start local server:"
echo "   cd distribution && python3 serve.py"
echo ""
echo "2. Open on iOS device:"
echo "   http://your-ip:8000"
echo ""
echo "3. Tap 'Install on iOS' button"
echo ""

# Step 6: Show next steps
echo "📋 Next Steps"
echo "========================================="
echo ""
echo "To make the app truly downloadable and playable independently:"
echo ""
echo "1. 🎯 Choose distribution method:"
echo "   - TestFlight for beta testing"
echo "   - App Store for public release"
echo "   - Web distribution for enterprise"
echo ""
echo "2. 🔐 Set up code signing:"
echo "   - Add Apple Developer account to Xcode"
echo "   - Configure development team"
echo "   - Set up provisioning profiles"
echo ""
echo "3. 📱 Test on real devices:"
echo "   - Install on various iOS devices"
echo "   - Test different screen sizes"
echo "   - Verify performance"
echo ""
echo "4. 🚀 Deploy:"
echo "   - Upload to TestFlight/App Store"
echo "   - Or host web distribution files"
echo "   - Share with users"
echo ""

# Show file locations
echo "📁 Generated Files"
echo "========================================="
echo ""
echo "Distribution files created in: ios/distribution/"
echo ""
if [ -d "distribution" ]; then
    echo "Files available:"
    ls -la distribution/ | grep -v "^total" | awk '{print "  " $9 " (" $5 " bytes)"}'
fi
echo ""

# Show app info
echo "📱 App Information"
echo "========================================="
echo ""
echo "App Name: Racing Game 3D"
echo "Bundle ID: com.racinggame.mobile"
echo "Version: 1.0.0"
echo "Minimum iOS: 13.0"
echo "Devices: iPhone, iPad"
echo "Orientation: Landscape"
echo "Graphics: Metal"
echo ""

echo "🎉 Demo Complete!"
echo "========================================="
echo ""
echo "The Racing Game 3D app is now configured for independent iOS distribution."
echo "Users can download and play the game without requiring a PC connection."
echo ""
echo "For detailed instructions, see:"
echo "- ios/README.md"
echo "- IOS_DISTRIBUTION_GUIDE.md"
echo ""
echo "Happy gaming! 🏎️"