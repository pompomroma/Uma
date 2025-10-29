#!/bin/bash

# Universal iOS Setup Script - Run from anywhere!
# This script finds the project root and sets up iOS build

set -e

echo "========================================="
echo "Racing Game 3D - iOS Setup"
echo "========================================="
echo ""

# Find the project root directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR"

# If script is in a subdirectory, try to find the actual root
if [[ ! -f "$PROJECT_ROOT/build_ios.sh" ]]; then
    # Search up the directory tree
    CURRENT_DIR="$PWD"
    while [[ "$CURRENT_DIR" != "/" ]]; do
        if [[ -f "$CURRENT_DIR/build_ios.sh" ]] && [[ -d "$CURRENT_DIR/ios" ]]; then
            PROJECT_ROOT="$CURRENT_DIR"
            break
        fi
        CURRENT_DIR="$(dirname "$CURRENT_DIR")"
    done
fi

# Verify we found the project root
if [[ ! -f "$PROJECT_ROOT/build_ios.sh" ]]; then
    echo "❌ Error: Could not find project root!"
    echo ""
    echo "Please make sure you're running this from within the RacingGame3D project."
    echo "Current directory: $PWD"
    exit 1
fi

echo "📁 Project root found: $PROJECT_ROOT"
echo ""

# Check if running on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "⚠️  Warning: You're not on macOS!"
    echo ""
    echo "This project is currently running in a Linux environment (likely GitHub Codespaces)."
    echo "iOS apps can only be built and deployed on macOS with Xcode."
    echo ""
    echo "📋 Here's what you need to do:"
    echo ""
    echo "1. Download this project to your Mac:"
    echo "   - If using git: git clone <repository-url>"
    echo "   - Or download as ZIP and extract"
    echo ""
    echo "2. On your Mac, open Terminal and run:"
    echo "   cd /path/to/RacingGame3D"
    echo "   chmod +x setup_ios.sh"
    echo "   ./setup_ios.sh"
    echo ""
    echo "3. The script will generate the Xcode project"
    echo ""
    echo "4. Open the project in Xcode:"
    echo "   open ios/build/RacingGame3DiOS.xcodeproj"
    echo ""
    echo "5. Connect your iPhone and click Run (⌘R)"
    echo ""
    echo "========================================="
    echo ""
    read -p "Press Enter to generate Xcode project files anyway (for transfer to Mac)..."
fi

# Navigate to project root
cd "$PROJECT_ROOT"

# Check for required files
if [[ ! -d "ios" ]]; then
    echo "❌ Error: 'ios' directory not found!"
    exit 1
fi

# Run the build script
echo "🔨 Generating Xcode project..."
echo ""
bash build_ios.sh

echo ""
echo "========================================="
echo "✅ Setup Complete!"
echo "========================================="
echo ""

if [[ "$OSTYPE" == "darwin"* ]]; then
    echo "🎉 You can now build and run on your iPhone!"
    echo ""
    echo "📝 Next Steps:"
    echo ""
    echo "1. Open Xcode project:"
    echo "   open ios/build/RacingGame3DiOS.xcodeproj"
    echo ""
    echo "2. Connect your iPhone via USB"
    echo ""
    echo "3. In Xcode:"
    echo "   - Select your iPhone from the device menu"
    echo "   - Go to Project Settings → Signing & Capabilities"
    echo "   - Add your Apple ID and select a Team"
    echo ""
    echo "4. Click the ▶️ (Play) button or press ⌘R"
    echo ""
    echo "5. On your iPhone:"
    echo "   Settings → General → VPN & Device Management"
    echo "   → Trust your developer certificate"
    echo ""
    echo "🎮 Then enjoy the game!"
else
    echo "📦 Xcode project files are ready!"
    echo ""
    echo "Transfer to your Mac and follow the steps above."
fi

echo ""
echo "========================================="
