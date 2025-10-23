# iOS Build Instructions for Racing Game 3D

## Prerequisites

1. **macOS Machine**: iOS builds require macOS with Xcode
2. **Xcode**: Install from the App Store (latest version recommended)
3. **iOS SDK**: Included with Xcode
4. **Developer Account**: For device deployment (optional for simulator)

## Building for iOS

### Method 1: Using the Build Script

```bash
# Make sure you're on macOS
./build_ios.sh

# For debug build
./build_ios.sh Debug
```

### Method 2: Manual Xcode Build

1. Open Terminal and navigate to the project directory
2. Generate Xcode project:
```bash
cd ios
cmake -B build -G Xcode \
    -DCMAKE_SYSTEM_NAME=iOS \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
    -DCMAKE_BUILD_TYPE=Release
```

3. Open the generated Xcode project:
```bash
open build/RacingGame3DiOS.xcodeproj
```

4. In Xcode:
   - Select your target device or simulator
   - Choose the RacingGame3DiOS scheme
   - Click Run (⌘R)

## Features Included

- **Metal Rendering**: Optimized for iOS devices
- **Touch Controls**: Full touch input support
- **Mobile UI**: Virtual controls and HUD
- **Orientation Support**: Landscape mode
- **Performance Optimized**: 60 FPS target

## Device Requirements

- iOS 13.0 or later
- iPhone/iPad with Metal support
- ARM64 architecture

## Troubleshooting

### Common Issues

1. **Code Signing**: Set up your Apple Developer account in Xcode
2. **Provisioning Profile**: Required for device deployment
3. **Metal Support**: Ensure device supports Metal API
4. **Memory**: Game is optimized for devices with 2GB+ RAM

### Build Errors

- **Missing SDK**: Update Xcode to latest version
- **Code Signing**: Check signing settings in Xcode
- **Dependencies**: Ensure all frameworks are linked properly

## Deployment

### Simulator Testing
- Works on iOS Simulator (macOS only)
- Good for development and testing

### Device Testing
- Requires Apple Developer account
- Device must be registered in Xcode
- App can be installed via Xcode or TestFlight

### App Store Distribution
- Requires paid Apple Developer account ($99/year)
- Follow App Store guidelines
- Submit through App Store Connect

## Performance Notes

- Game targets 60 FPS on modern iOS devices
- Metal rendering provides better performance than OpenGL ES
- Touch input is optimized for mobile gameplay
- Memory usage is optimized for mobile constraints

## Development Tips

1. **Test on Real Devices**: Simulator performance differs from real devices
2. **Memory Management**: iOS has strict memory limits
3. **Battery Life**: Optimize for battery efficiency
4. **Touch Responsiveness**: Ensure controls feel natural on touch screens