# Racing Game 3D - Mobile Version

This is the mobile-optimized version of the 3D Racing Game, supporting both **Android** and **iOS** platforms.

## 🎮 Mobile Features

### Touch Controls
- **Virtual Joystick (Left)**: Steering and acceleration/brake
- **Virtual Joystick (Right)**: Camera control
- **Virtual Buttons**: Boost, handbrake, attack, shield, and other actions
- **Multi-touch Support**: Simultaneous input handling
- **Gesture Recognition**: Tap, swipe, pinch, and rotation

### Mobile-Specific Optimizations
- **Adaptive Screen Resolution**: Supports all screen sizes and orientations
- **Safe Area Support**: Handles notches and rounded corners
- **Low Power Mode**: Battery-saving features
- **Touch Sensitivity**: Customizable touch response
- **Virtual Control Layout**: Auto-adjusts for portrait/landscape

## 📱 Platform Support

### Android
- **Minimum SDK**: Android 7.0 (API 24)
- **Target SDK**: Android 13 (API 33)
- **Graphics**: OpenGL ES 3.0
- **Architectures**: ARM64, ARM32, x86, x86_64

### iOS
- **Minimum Version**: iOS 13.0+
- **Graphics**: Metal
- **Devices**: iPhone, iPad
- **Orientation**: Landscape (recommended)

## 🛠️ Build Instructions

### Prerequisites

#### For Android:
1. **Android Studio** (version 2021.1.1 or later)
2. **Android NDK** (version 25 or later)
3. **CMake** (version 3.18.1 or later)
4. **Android SDK** with API level 24+

Set environment variables:
```bash
export ANDROID_HOME=/path/to/android/sdk
export ANDROID_NDK=/path/to/android/ndk
```

#### For iOS:
1. **macOS** (required for iOS development)
2. **Xcode** (version 14.0 or later)
3. **CMake** (version 3.18 or later)
4. **Apple Developer Account** (for device deployment)

---

## 🔨 Building for Android

### Method 1: Using the Build Script

```bash
./build_android.sh Release arm64-v8a
```

Parameters:
- First argument: Build type (`Debug` or `Release`) - default: `Release`
- Second argument: ABI (`arm64-v8a`, `armeabi-v7a`, `x86`, `x86_64`) - default: `arm64-v8a`

### Method 2: Using Android Studio

1. Open Android Studio
2. Select "Open an Existing Project"
3. Navigate to the `android` folder
4. Wait for Gradle sync to complete
5. Click "Run" or press `Shift+F10`

### Method 3: Using Gradle Command Line

```bash
cd android
gradle assembleRelease
```

The APK will be generated in:
```
android/build/outputs/apk/release/RacingGame3DMobile-release.apk
```

### Installing on Android Device

```bash
# Enable USB debugging on your device
adb install android/build/outputs/apk/release/RacingGame3DMobile-release.apk
```

Or simply open the APK file on your device.

---

## 🍎 Building for iOS

### Method 1: Using the Build Script

```bash
./build_ios.sh Release
```

Parameter:
- Build type (`Debug` or `Release`) - default: `Release`

### Method 2: Using Xcode

1. Open Terminal and navigate to the `ios` folder
2. Generate Xcode project:
   ```bash
   cmake -B build -G Xcode -DCMAKE_SYSTEM_NAME=iOS
   ```
3. Open the generated project:
   ```bash
   open build/RacingGame3DiOS.xcodeproj
   ```
4. In Xcode:
   - Select your development team in the project settings
   - Connect your iOS device
   - Select your device from the target menu
   - Click Run (⌘R)

### Code Signing

For iOS device deployment, you need:
1. An Apple Developer account
2. A valid provisioning profile
3. Code signing identity configured in Xcode

To set up code signing:
1. Open the Xcode project
2. Select the project in the navigator
3. Go to "Signing & Capabilities"
4. Select your team from the dropdown
5. Xcode will automatically manage certificates and profiles

---

## 🎯 Touch Controls Layout

### Landscape Mode (Recommended)
```
┌─────────────────────────────────────────────────────┐
│  [Pause] [Reset]                                    │
│                                                      │
│                              [Attack1] [Attack2]     │
│                              [Shield]  [Teleport]    │
│                                                      │
│  (Left)                                   (Right)    │
│  Joystick                                 Joystick   │
│    ●                    [Accelerate]         ●       │
│                         [Brake]                      │
│                         [Boost]                      │
└─────────────────────────────────────────────────────┘
```

### Portrait Mode (Racing)
```
┌──────────────────────┐
│    [Pause] [Reset]   │
│                      │
│                      │
│                      │
│    [Accelerate]      │
│    [Brake]           │
│    [Boost]           │
│                      │
│                      │
│    (Left)    (Right) │
│  Joystick  Joystick  │
│      ●         ●     │
└──────────────────────┘
```

---

## ⚙️ Configuration Options

### Touch Sensitivity
Adjust in-game through settings or modify in code:
```cpp
touchInputManager->setTouchSensitivity(1.5f); // Range: 0.5 - 2.0
```

### Joystick Deadzone
Adjust the minimum movement threshold:
```cpp
touchInputManager->setJoystickDeadzone(0.15f); // Range: 0.0 - 0.3
```

### Virtual Control Visibility
Toggle on-screen controls:
```cpp
mobileUI->setShowVirtualControls(true);
```

---

## 🎨 Graphics Settings

### Android (OpenGL ES 3.0)
- Adaptive resolution scaling
- MSAA support (2x/4x)
- Texture compression (ETC2)
- Dynamic lighting with optimized shaders

### iOS (Metal)
- Native Metal rendering
- Tile-based deferred rendering
- HDR support on compatible devices
- Dynamic resolution scaling

---

## 📊 Performance Optimization

### Mobile-Specific Optimizations
1. **Level of Detail (LOD)**: Automatic mesh quality adjustment
2. **Occlusion Culling**: Don't render off-screen objects
3. **Texture Streaming**: Load textures on-demand
4. **Particle Limits**: Reduced particle counts
5. **Shadow Quality**: Adjustable shadow resolution
6. **Physics Update Rate**: Adaptive timestep

### Battery Optimization
- Low power mode reduces:
  - Frame rate (60 FPS → 30 FPS)
  - Shadow quality
  - Particle effects
  - Post-processing effects

Enable low power mode:
```cpp
game->setLowPowerMode(true);
```

---

## 🐛 Debugging

### Android Debugging

View logs:
```bash
adb logcat | grep RacingGame3D
```

Enable debug mode in CMake:
```bash
./build_android.sh Debug arm64-v8a
```

### iOS Debugging

1. Open project in Xcode
2. Set breakpoints in C++ or Objective-C code
3. Run with debugger (⌘R)
4. View console output in Xcode's debug area

---

## 📁 Project Structure

```
/workspace/
├── src/                      # Shared C++ game code
│   ├── Platform/             # Platform detection
│   ├── Input/                # Input handling
│   │   ├── InputManager.*    # Desktop + Mobile input
│   │   └── TouchInputManager.* # Touch-specific input
│   ├── UI/                   # UI components
│   │   └── MobileUI.*        # Mobile-specific UI
│   └── ...                   # Other game systems
│
├── android/                  # Android-specific files
│   ├── CMakeLists.txt        # Android build config
│   ├── android_main.cpp      # Android entry point
│   ├── AndroidManifest.xml   # App manifest
│   └── build.gradle          # Gradle build script
│
├── ios/                      # iOS-specific files
│   ├── CMakeLists.txt        # iOS build config
│   ├── ios_main.mm           # iOS entry point
│   ├── GameViewController.mm # Main view controller
│   └── Info.plist            # iOS app info
│
├── build_android.sh          # Android build script
├── build_ios.sh              # iOS build script
└── MOBILE_README.md          # This file
```

---

## 🎮 Gameplay Modes

### Racing Mode
- Use left joystick to steer
- Virtual buttons for acceleration/brake
- Boost button for speed burst
- Handbrake for tight turns

### Combat Mode (PvP)
- WASD-style movement with left joystick
- Right joystick for camera/aim
- Attack buttons for weapons
- Shield button for defense
- Teleport button for evasive maneuvers

---

## 🔧 Troubleshooting

### Android Issues

**Problem**: "Failed to initialize GLES"
- **Solution**: Ensure your device supports OpenGL ES 3.0

**Problem**: APK won't install
- **Solution**: Enable "Install from Unknown Sources" in device settings

**Problem**: Touch input not working
- **Solution**: Check that multi-touch is enabled in AndroidManifest.xml

### iOS Issues

**Problem**: "Code signing failed"
- **Solution**: Set up your development team in Xcode project settings

**Problem**: App crashes on launch
- **Solution**: Check device meets minimum iOS 13.0 requirement

**Problem**: Metal initialization fails
- **Solution**: Ensure device supports Metal (iPhone 5s or later)

---

## 📝 Development Notes

### Adding New Mobile Features

1. **Touch Gestures**: Extend `TouchInputManager::detectGestures()`
2. **Virtual Controls**: Add buttons via `TouchInputManager::addButton()`
3. **Platform-Specific Code**: Use `#if PLATFORM_MOBILE` macros
4. **UI Layouts**: Modify in `MobileUI::layoutForLandscape/Portrait()`

### Graphics API Differences

**OpenGL ES (Android)** vs **Metal (iOS)**:
- Shader languages differ (GLSL vs MSL)
- Use platform detection to load appropriate shaders
- Renderer abstraction handles API differences

---

## 🚀 Publishing

### Android (Google Play)

1. Generate signed APK:
   ```bash
   cd android
   gradle assembleRelease
   ```
2. Create keystore for signing
3. Sign APK with keystore
4. Upload to Google Play Console

### iOS (App Store)

1. Archive in Xcode (Product → Archive)
2. Validate archive
3. Upload to App Store Connect
4. Submit for review

---

## 📄 License

See main README.md for license information.

---

## 🤝 Contributing

Mobile-specific contributions welcome! Please ensure:
- Code works on both Android and iOS
- Touch controls are intuitive
- Performance is acceptable on mid-range devices
- Battery usage is optimized

---

## 💡 Tips for Mobile Development

1. **Test on Real Devices**: Emulators don't accurately represent touch input
2. **Monitor Performance**: Use profiling tools (Android Studio Profiler, Xcode Instruments)
3. **Consider Screen Sizes**: Test on various devices (phones, tablets)
4. **Optimize for Battery**: Mobile users are sensitive to battery drain
5. **Handle Interruptions**: Calls, notifications, app switching
6. **Safe Areas**: Account for notches and rounded corners

---

## 📞 Support

For mobile-specific issues, please include:
- Platform (Android/iOS)
- Device model and OS version
- Build configuration (Debug/Release)
- Logs (adb logcat or Xcode console)

---

**Enjoy Racing Game 3D on mobile! 🏎️📱**
