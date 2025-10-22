# Racing Game 3D - Mobile Build Guide

## Overview
The game has been fully adapted for mobile platforms (Android and iOS) with touch controls, platform-specific optimizations, and mobile-friendly UI.

## Features

### Mobile-Specific Features
- **Touch Controls**: 
  - Virtual joysticks for movement and camera
  - On-screen buttons for actions
  - Gesture support (tap, double-tap, swipe, pinch)
  - Optional tilt controls for steering (using accelerometer)

- **Performance Optimizations**:
  - Dynamic quality adjustment based on device performance
  - Render scaling for lower-end devices
  - Simplified shaders for mobile GPUs
  - Reduced particle effects option
  - Automatic texture compression

- **Mobile UI**:
  - Larger UI elements for touch interaction
  - Responsive layout that adapts to screen size and orientation
  - Touch-friendly menus and HUD
  - Visual feedback for touch inputs

- **Platform Integration**:
  - Haptic feedback support
  - Battery level monitoring
  - Thermal state management
  - Background/foreground handling
  - Device orientation support

## Building for Android

### Prerequisites
1. Android SDK (API level 21+)
2. Android NDK (r21 or later)
3. CMake 3.16+
4. Java JDK 8+
5. Android Studio (optional, for debugging)

### Environment Setup
```bash
export ANDROID_SDK_HOME=/path/to/android-sdk
export ANDROID_NDK_HOME=/path/to/android-ndk
```

### Build Steps
```bash
# Make build script executable
chmod +x build_android.sh

# Build Debug APK
./build_android.sh Debug

# Build Release APK
./build_android.sh Release
```

The APK will be generated at:
- Debug: `android/app/build/outputs/apk/debug/app-debug.apk`
- Release: `android/app/build/outputs/apk/release/app-release.apk`

### Installing on Device
```bash
# Install debug build
adb install android/app/build/outputs/apk/debug/app-debug.apk

# Install release build (requires signing)
adb install android/app/build/outputs/apk/release/app-release.apk
```

## Building for iOS

### Prerequisites
1. macOS with Xcode 12+
2. iOS SDK 12.0+
3. CMake 3.16+
4. Apple Developer Account (for device deployment)
5. ios-deploy (optional, for command-line deployment)

### Build Steps
```bash
# Make build script executable
chmod +x build_ios.sh

# Build Debug
./build_ios.sh Debug

# Build Release
./build_ios.sh Release
```

The iOS app will be generated at:
- Archive: `build-ios/RacingGame3D.xcarchive`
- IPA: `build-ios/Export/RacingGame3D.ipa`

### Installing on Device
1. Open the generated Xcode project in `build-ios/`
2. Select your development team
3. Connect your iOS device
4. Build and run from Xcode

Or use ios-deploy:
```bash
ios-deploy --bundle build-ios/Export/RacingGame3D.app --debug
```

## Project Structure

### Mobile-Specific Files
```
src/
├── Platform/           # Platform abstraction layer
│   ├── PlatformBase.h/cpp
│   ├── AndroidPlatform.h/cpp
│   ├── iOSPlatform.h/cpp (Objective-C++)
│   └── DesktopPlatform.h/cpp
├── Input/
│   └── TouchInputManager.h/cpp  # Touch input handling
├── Rendering/
│   └── MobileRenderer.h/cpp     # Mobile-optimized rendering
└── Game/
    └── MobileGame.h/cpp         # Mobile game logic

android/               # Android-specific files
├── app/
│   ├── build.gradle
│   └── src/main/
│       ├── AndroidManifest.xml
│       └── java/com/racinggame3d/mobile/
│           └── MainActivity.java

ios/                   # iOS-specific files
├── Info.plist
└── LaunchScreen.storyboard
```

## Control Schemes

### Racing Mode
- **Tilt Controls** (optional): Tilt device left/right to steer
- **Touch Steering**: Virtual joystick on left side
- **Accelerate**: Button on bottom right
- **Brake**: Button next to accelerate
- **Boost**: Button above accelerate
- **Camera**: Swipe or right joystick

### Combat Mode
- **Movement**: Left virtual joystick
- **Camera**: Right virtual joystick
- **Attack**: Action buttons on right
- **Shield**: Shield button
- **Special**: Special ability button

### Gestures
- **Double Tap**: Toggle camera mode
- **Pinch**: Zoom camera
- **Swipe**: Navigate menus

## Performance Settings

The game automatically adjusts quality based on device performance:

### High-End Devices (4GB+ RAM)
- Full resolution rendering
- High-quality textures
- All particle effects
- Post-processing enabled
- 60 FPS target

### Mid-Range Devices (2-4GB RAM)
- 85% render scale
- Medium textures
- Reduced particles
- Limited post-processing
- 60 FPS target

### Low-End Devices (<2GB RAM)
- 75% render scale
- Low textures
- Minimal particles
- No post-processing
- 30 FPS target

## Testing on Desktop

You can test mobile features on desktop:
1. Build the desktop version normally
2. The desktop platform simulates touch with mouse
3. Use `--mobile` flag to enable mobile UI

```bash
./build/RacingGame3D --mobile
```

## Troubleshooting

### Android Issues
- **Build fails**: Check NDK/SDK paths are correctly set
- **Black screen**: Ensure device supports OpenGL ES 3.0
- **Low FPS**: Try reducing graphics quality in settings
- **Controls not working**: Check touch input permissions

### iOS Issues
- **Build fails**: Ensure Xcode command line tools are installed
- **Signing errors**: Set up development team in Xcode
- **Performance issues**: Enable release mode for better performance
- **Orientation problems**: Game is locked to landscape mode

## Configuration

### Modifying Controls
Edit `src/Input/TouchInputManager.cpp` to customize control layouts.

### Adjusting Performance
Edit `src/Game/MobileGame.cpp` `optimizeForDevice()` method to change quality thresholds.

### Platform-Specific Features
- Android: Edit `MainActivity.java` for Android-specific features
- iOS: Edit iOS platform files for iOS-specific features

## Known Limitations
- Multiplayer not yet implemented for mobile
- Cloud save functionality pending
- Some visual effects simplified for mobile GPUs
- iOS Metal renderer not yet implemented (using OpenGL ES)

## Future Improvements
- Add multiplayer support
- Implement cloud saves
- Add controller support
- Optimize further for low-end devices
- Add more gesture controls
- Implement iOS Metal renderer
- Add social features (leaderboards, achievements)

## Support
For issues or questions, please refer to the main project documentation or submit an issue on the project repository.