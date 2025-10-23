# iOS Build Guide for Racing Game 3D

This guide explains how to build and deploy the Racing Game 3D project for iOS devices.

## Prerequisites

### Required Software
- **macOS** (iOS builds require macOS)
- **Xcode 14.0+** (available from the App Store)
- **CMake 3.18+** (install via Homebrew: `brew install cmake`)
- **Apple Developer Account** (for device deployment)

### Hardware Requirements
- Mac computer (MacBook, iMac, Mac mini, or Mac Studio)
- iOS device running iOS 13.0+ (for testing on device)

## Project Features on iOS

The iOS version includes all desktop features adapted for mobile:

### Core Gameplay
- **Racing Mode**: Full 3D racing with touch controls
- **PvP Combat Mode**: Multiplayer combat with touch-optimized controls
- **Camera System**: Touch-based camera controls with gestures

### Mobile Optimizations
- **Touch Controls**: Virtual joystick and buttons
- **Metal Rendering**: Native iOS graphics using Metal API
- **Adaptive UI**: Responsive interface for different screen sizes
- **Battery Optimization**: Low-power mode support
- **Orientation Support**: Landscape mode optimized

### Input Systems
- **Virtual Joystick**: Dynamic positioning for movement
- **Touch Gestures**: Tap, swipe, pinch, and drag support
- **Camera Drag**: Two-finger camera rotation
- **Button Controls**: Virtual buttons for actions

## Building for iOS

### Method 1: Local Build on macOS

1. **Clone/Transfer the Project**
   ```bash
   # If cloning fresh
   git clone <repository-url>
   cd racing-game-3d
   
   # If transferring from Linux/Windows
   # Copy the entire project folder to your Mac
   ```

2. **Install Dependencies**
   ```bash
   # Install Xcode from App Store first
   # Then install CMake
   brew install cmake
   
   # Install Xcode command line tools
   xcode-select --install
   ```

3. **Build the Project**
   ```bash
   # Make sure build script is executable
   chmod +x build_ios.sh
   
   # Build for iOS
   ./build_ios.sh
   ```

4. **Open in Xcode**
   ```bash
   # The build script will generate an Xcode project
   open ios/build/RacingGame3DiOS.xcodeproj
   ```

### Method 2: GitHub Actions (Automated)

Create `.github/workflows/ios-build.yml`:

```yaml
name: iOS Build

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  build-ios:
    runs-on: macos-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Xcode
      uses: maxim-lobanov/setup-xcode@v1
      with:
        xcode-version: latest-stable
    
    - name: Install CMake
      run: brew install cmake
    
    - name: Build iOS Project
      run: |
        chmod +x build_ios.sh
        ./build_ios.sh
    
    - name: Archive Build Artifacts
      uses: actions/upload-artifact@v3
      with:
        name: ios-build
        path: ios/build/Release-iphoneos/
```

## Deployment to iOS Device

### 1. Configure Code Signing

1. Open the Xcode project: `ios/build/RacingGame3DiOS.xcodeproj`
2. Select the project in the navigator
3. Go to "Signing & Capabilities" tab
4. Set your **Development Team** (requires Apple Developer account)
5. Ensure **Bundle Identifier** is unique (e.g., `com.yourname.racinggame`)

### 2. Connect iOS Device

1. Connect your iPhone/iPad via USB
2. Trust the computer on your device when prompted
3. In Xcode, select your device from the destination menu

### 3. Install and Run

1. Click the **Run** button (⌘R) in Xcode
2. The app will be built, installed, and launched on your device

### 4. Trust Developer Certificate

First time running:
1. Go to **Settings > General > VPN & Device Management**
2. Find your developer certificate
3. Tap **Trust** to allow the app to run

## Troubleshooting

### Common Issues

#### "Metal is not supported"
- **Solution**: Ensure you're using a device with Metal support (iPhone 5s+ or iPad Air+)

#### Code Signing Errors
- **Solution**: 
  1. Ensure you have a valid Apple Developer account
  2. Check that your device is registered in your developer account
  3. Verify the bundle identifier is unique

#### Build Failures
- **Solution**:
  1. Clean build folder: `rm -rf ios/build`
  2. Regenerate project: `./build_ios.sh`
  3. Check Xcode version compatibility

#### Touch Controls Not Working
- **Solution**: 
  1. Ensure `multipleTouchEnabled = YES` in GameViewController
  2. Check touch input manager initialization
  3. Verify virtual control positioning

### Performance Optimization

#### For Better Performance:
```objc
// In GameViewController.mm
self.mtkView.preferredFramesPerSecond = 30; // Reduce from 60 for battery life
```

#### For Better Battery Life:
```cpp
// Enable low power mode when needed
game->setLowPowerMode(true);
```

## Game Controls on iOS

### Racing Mode
- **Left Side**: Virtual joystick for steering
- **Right Side**: 
  - Top button: Accelerate
  - Bottom button: Brake
  - Side button: Boost
- **Two-finger drag**: Camera rotation

### PvP Combat Mode
- **Left Side**: Virtual joystick for movement
- **Right Side**:
  - Q button: Fist attack
  - E button: Laser attack
  - F button: Shield toggle
  - R button: Teleport
- **Camera**: Automatic follow with touch override

### Menu Navigation
- **Tap**: Select menu items
- **Swipe**: Navigate between screens

## Distribution

### TestFlight (Recommended for Testing)
1. Archive the app in Xcode (Product > Archive)
2. Upload to App Store Connect
3. Add internal/external testers
4. Distribute via TestFlight

### App Store Release
1. Complete app metadata in App Store Connect
2. Submit for review
3. Release after approval

## File Structure

```
ios/
├── CMakeLists.txt          # iOS build configuration
├── Info.plist             # App metadata and permissions
├── LaunchScreen.storyboard # Launch screen UI
├── ios_main.mm            # iOS app entry point
└── GameViewController.mm   # Main game view controller

src/
├── Platform/
│   └── PlatformDetect.h   # Platform detection macros
├── Rendering/
│   ├── Renderer.cpp       # Cross-platform renderer
│   └── RendererMetal.mm   # iOS Metal implementation
├── Input/
│   └── TouchInputManager.* # Touch input handling
└── UI/
    └── MobileUI.*         # Mobile UI components
```

## Support

For issues specific to iOS builds:
1. Check this guide first
2. Verify your macOS and Xcode versions
3. Test on the iOS Simulator before device
4. Check Apple Developer documentation for code signing issues

The iOS version maintains full feature parity with the desktop version while providing an optimized mobile gaming experience.