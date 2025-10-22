# Racing Game 3D - Mobile Edition

A high-performance 3D racing game optimized for mobile platforms (Android and iOS).

## 🚀 Features

### Mobile-Optimized Gameplay
- **Touch Controls**: Intuitive virtual joysticks and buttons
- **Gesture Support**: Tap, swipe, pinch-to-zoom, and multi-touch
- **Adaptive Performance**: Automatic quality adjustment based on device capabilities
- **Battery Optimization**: Efficient rendering and power management

### Cross-Platform Support
- **Android**: NDK-based native code with OpenGL ES 3.0
- **iOS**: Metal and OpenGL ES support with native Objective-C++ integration
- **Unified Codebase**: Shared C++ core with platform-specific optimizations

### Advanced Graphics
- **OpenGL ES 3.0**: Modern mobile graphics pipeline
- **Level of Detail (LOD)**: Automatic mesh simplification for performance
- **Batch Rendering**: Optimized draw calls for mobile GPUs
- **Compressed Textures**: Platform-specific texture compression

### Racing Features
- **3D Physics**: Realistic car physics optimized for mobile
- **Track System**: Procedural and pre-built racing tracks
- **Combat Mode**: PvP racing with weapons and power-ups
- **Career Mode**: Progressive gameplay with unlockables

## 📱 System Requirements

### Android
- **Minimum**: Android 5.0 (API 21), OpenGL ES 3.0
- **Recommended**: Android 8.0+, 3GB RAM, Adreno 530+ or Mali-G71+
- **Storage**: 500MB free space

### iOS
- **Minimum**: iOS 12.0, iPhone 6s/iPad Air 2 or newer
- **Recommended**: iOS 15.0+, iPhone 8+ or iPad Pro
- **Storage**: 500MB free space

## 🛠️ Building the Game

### Prerequisites

#### Android Development
```bash
# Install Android Studio and SDK
# Set environment variables
export ANDROID_HOME=/path/to/android/sdk
export ANDROID_NDK_HOME=/path/to/android/ndk
export PATH=$PATH:$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools
```

#### iOS Development (macOS only)
```bash
# Install Xcode from App Store
# Install Xcode Command Line Tools
xcode-select --install
```

### Quick Build
```bash
# Make build script executable
chmod +x build_mobile.sh

# Build for Android (debug)
./build_mobile.sh android debug

# Build for iOS (release)
./build_mobile.sh ios release

# Build for both platforms
./build_mobile.sh both release
```

### Manual Build

#### Android
```bash
cd android
./gradlew assembleDebug      # Debug build
./gradlew assembleRelease    # Release build
```

#### iOS
```bash
mkdir -p build/ios && cd build/ios
cmake ../.. -DCMAKE_SYSTEM_NAME=iOS -G Xcode
xcodebuild -project RacingGame3D.xcodeproj -scheme RacingGame3D -configuration Release
```

## 🎮 Controls

### Touch Controls
- **Virtual Joystick (Left)**: Steering control
- **Accelerate Button (Bottom Right)**: Gas pedal
- **Brake Button (Right)**: Brake/Reverse
- **Boost Button**: Temporary speed boost
- **Handbrake Button (Bottom Left)**: Emergency brake
- **Camera Joystick (Top Right)**: Camera control

### Gestures
- **Pinch**: Zoom camera in/out
- **Tap**: Select menu items, pause game
- **Swipe**: Navigate menus
- **Long Press**: Access advanced options

### Optional Controls
- **Tilt Steering**: Use device gyroscope for steering (iOS)
- **External Controllers**: MFi controllers (iOS), Bluetooth gamepads (Android)

## ⚡ Performance Optimization

### Automatic Quality Adjustment
The game automatically detects device capabilities and adjusts:

- **Low-End Devices**: Reduced polygon count, simplified shaders, lower resolution
- **Mid-Range Devices**: Balanced quality and performance
- **High-End Devices**: Full visual effects, high resolution, advanced lighting

### Manual Performance Settings
Access via pause menu → Settings → Graphics:

- **Performance Level**: Low/Medium/High/Auto
- **Resolution Scale**: 50%-100%
- **Effect Quality**: Minimal/Standard/Enhanced
- **Frame Rate Target**: 30/60 FPS

### Battery Optimization
- **Adaptive Frame Rate**: Reduces FPS when not needed
- **Background Pause**: Automatically pauses when app goes to background
- **Thermal Management**: Reduces quality if device overheats

## 📁 Project Structure

```
├── src/                          # Core game source code
│   ├── Mobile/                   # Mobile-specific code
│   │   ├── MobileRenderer.cpp    # Mobile-optimized renderer
│   │   ├── TouchInputManager.cpp # Touch input handling
│   │   ├── VirtualControls.cpp   # UI controls
│   │   ├── Android/              # Android-specific code
│   │   └── iOS/                  # iOS-specific code
│   ├── Game.cpp                  # Main game logic
│   ├── Physics/                  # Physics engine
│   ├── Rendering/                # Desktop renderer
│   └── ...
├── android/                      # Android project
│   ├── app/                      # Android app
│   │   ├── src/main/jni/        # Native code
│   │   └── src/main/java/       # Java/Kotlin code
│   └── build.gradle             # Android build config
├── iOS/                         # iOS project files
├── assets/                      # Game assets
│   ├── shaders/                 # GLSL shaders
│   ├── textures/               # Texture files
│   └── models/                 # 3D models
├── build_mobile.sh             # Mobile build script
└── CMakeLists.txt              # Cross-platform build config
```

## 🔧 Development Guide

### Adding New Features

1. **Core Logic**: Add to `src/` directory using C++
2. **Mobile UI**: Update `VirtualControls.cpp` for new buttons/controls
3. **Platform Integration**: Add platform-specific code in `Android/` or `iOS/`
4. **Assets**: Place new assets in `assets/` directory

### Platform-Specific Code

#### Android (Java/Kotlin + C++)
```cpp
#ifdef ANDROID_PLATFORM
// Android-specific code
#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "RacingGame", __VA_ARGS__)
#endif
```

#### iOS (Objective-C++ + C++)
```cpp
#ifdef IOS_PLATFORM
// iOS-specific code
#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#endif
```

### Performance Profiling

#### Android
```bash
# GPU profiling
adb shell setprop debug.egl.profiler 1

# CPU profiling with systrace
python systrace.py --time=10 -a com.racinggame3d.mobile gfx input view
```

#### iOS
```bash
# Use Xcode Instruments
# Profile → GPU Driver
# Profile → Time Profiler
```

## 🐛 Troubleshooting

### Common Build Issues

#### Android
- **NDK not found**: Set `ANDROID_NDK_HOME` environment variable
- **Gradle sync failed**: Update Android Studio and Gradle plugin
- **OpenGL ES errors**: Ensure device supports OpenGL ES 3.0

#### iOS
- **Code signing error**: Set correct development team in Xcode
- **Metal not supported**: Fallback to OpenGL ES on older devices
- **Simulator issues**: Use physical device for testing

### Runtime Issues

#### Performance Problems
- Check device temperature (thermal throttling)
- Reduce graphics quality in settings
- Close other apps to free memory
- Update device drivers

#### Touch Input Issues
- Ensure multi-touch is enabled
- Check screen calibration
- Verify touch zones don't overlap

## 📊 Performance Benchmarks

### Target Performance
- **60 FPS**: High-end devices (iPhone 12+, Snapdragon 855+)
- **30 FPS**: Mid-range devices (iPhone 8+, Snapdragon 660+)
- **Stable**: Low-end devices with quality reduction

### Memory Usage
- **Base Game**: ~150MB RAM
- **With Assets**: ~300MB RAM
- **Peak Usage**: ~400MB RAM (loading)

### Battery Life
- **Continuous Play**: 2-4 hours depending on device
- **Standby**: Minimal battery drain when paused

## 🔄 Updates and Deployment

### Android Deployment
```bash
# Generate signed APK
./gradlew assembleRelease

# Upload to Google Play Console
# Use Android App Bundle (.aab) for Play Store
./gradlew bundleRelease
```

### iOS Deployment
```bash
# Archive for App Store
xcodebuild -workspace RacingGame3D.xcworkspace -scheme RacingGame3D -configuration Release archive

# Upload to App Store Connect via Xcode or Application Loader
```

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on both Android and iOS
5. Submit a pull request

## 📞 Support

- **Issues**: GitHub Issues
- **Documentation**: See `/docs` directory
- **Performance**: Use built-in profiling tools
- **Community**: Join our Discord server

## 🎯 Roadmap

### Version 1.1
- [ ] Multiplayer racing
- [ ] More tracks and cars
- [ ] Advanced graphics effects
- [ ] Social features

### Version 1.2
- [ ] AR mode support
- [ ] Cloud save synchronization
- [ ] Tournament system
- [ ] Custom track editor

---

**Happy Racing! 🏁**