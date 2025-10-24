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
