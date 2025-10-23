#!/bin/bash

# iOS Deployment Script for Racing Game 3D
# This script helps deploy the built iOS app to a device

set -e

echo "========================================="
echo "iOS Deployment Helper"
echo "========================================="

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: iOS deployment must be performed on macOS"
    echo "Current OS: $OSTYPE"
    echo ""
    echo "Please transfer this project to a macOS machine and run:"
    echo "1. ./build_ios.sh (to build the project)"
    echo "2. ./deploy_ios.sh (to deploy to device)"
    exit 1
fi

# Check if Xcode project exists
if [ ! -d "ios/build/RacingGame3DiOS.xcodeproj" ]; then
    echo "Xcode project not found. Building first..."
    ./build_ios.sh
fi

echo "Opening Xcode project for deployment..."
open ios/build/RacingGame3DiOS.xcodeproj

echo ""
echo "========================================="
echo "Deployment Instructions"
echo "========================================="
echo ""
echo "In Xcode:"
echo "1. Connect your iOS device via USB"
echo "2. Select your device from the destination menu"
echo "3. Go to Project Settings > Signing & Capabilities"
echo "4. Set your Development Team (Apple Developer Account required)"
echo "5. Ensure Bundle Identifier is unique"
echo "6. Click Run (⌘R) to install on device"
echo ""
echo "First time setup on device:"
echo "1. Go to Settings > General > VPN & Device Management"
echo "2. Find your developer certificate"
echo "3. Tap 'Trust' to allow the app to run"
echo ""
echo "Game Controls on iOS:"
echo "- Racing Mode: Virtual joystick + touch buttons"
echo "- PvP Mode: Movement joystick + action buttons"
echo "- Camera: Two-finger drag to rotate"
echo ""
echo "Enjoy playing Racing Game 3D on your iPhone!"
echo "========================================="