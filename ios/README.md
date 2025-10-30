# Racing Game 3D - iOS Distribution

This directory contains all the necessary files and scripts to build and distribute the Racing Game 3D app for iOS devices.

## Quick Start

### For Developers (Xcode Installation)
```bash
# Build and install directly to device
./build_ios.sh Release

# Then open in Xcode and run to device
open build/RacingGame3DiOS.xcodeproj
```

### For Distribution (TestFlight/App Store)
```bash
# Build distribution package
./build_distribution.sh Release

# Files will be created in distribution/ directory
```

### For Web Distribution (Testing)
```bash
# Build distribution package first
./build_distribution.sh Release

# Start local web server
./test_distribution.py

# Open http://your-ip:8000 on iOS device
```

## Files Overview

### Core App Files
- `ios_main.mm` - iOS app entry point
- `GameViewController.mm` - Main game view controller
- `Info.plist` - App configuration and metadata
- `LaunchScreen.storyboard` - App launch screen
- `CMakeLists.txt` - Build configuration

### Assets
- `Assets.xcassets/` - App icons and images
- `generate_icons.py` - Icon generation script

### Build Scripts
- `build_ios.sh` - Standard iOS build script
- `build_distribution.sh` - Distribution build script
- `test_distribution.py` - Local web server for testing

### Distribution Files
- `distribution.html` - Web download page
- `manifest.plist` - OTA installation manifest
- `INSTALLATION.md` - Installation instructions

## App Features

### Technical Features
- **Platform:** iOS 13.0+
- **Graphics:** Metal rendering
- **Architecture:** ARM64
- **Orientation:** Landscape only
- **Devices:** iPhone and iPad

### Game Features
- 3D racing gameplay
- Touch controls
- Combat system
- Mobile-optimized UI
- High-performance graphics

## Installation Methods

### 1. Xcode Development (Easiest)
- Requires Mac with Xcode
- Direct installation to device
- Best for developers and testers

### 2. TestFlight Beta
- Apple's official beta testing platform
- No Mac required for users
- Up to 10,000 external testers
- Automatic updates

### 3. App Store
- Public distribution
- No installation restrictions
- Professional distribution
- Automatic updates

### 4. Web Distribution
- Enterprise/Ad-hoc distribution
- Custom installation process
- Requires web server hosting

## System Requirements

### iOS Device
- iOS 13.0 or later
- iPhone 6s or later
- iPad Air 2 or later
- Metal-compatible graphics
- At least 100MB free storage

### Development
- macOS 10.15 or later
- Xcode 12.0 or later
- Apple Developer Account
- CMake 3.18 or later

## Build Process

### 1. Generate Icons
```bash
python3 generate_icons.py
```

### 2. Build App
```bash
# Development build
./build_ios.sh Debug

# Release build
./build_ios.sh Release
```

### 3. Create Distribution
```bash
# Full distribution package
./build_distribution.sh Release
```

## Configuration

### App Settings
- **Bundle ID:** com.racinggame.mobile
- **Display Name:** Racing Game 3D
- **Version:** 1.0.0
- **Category:** Games
- **Minimum iOS:** 13.0

### Code Signing
- Automatic signing (recommended)
- Requires Apple ID with developer account
- Works for development and TestFlight

### App Icons
- Generated automatically with placeholder icons
- Replace with custom 1024x1024 icon
- All required sizes included

## Testing

### Local Testing
```bash
# Start web server
./test_distribution.py

# Test on iOS device
# Open http://your-ip:8000 in Safari
```

### Device Testing
1. Connect iOS device to Mac
2. Trust computer on device
3. Install app via Xcode
4. Trust developer certificate in Settings

## Troubleshooting

### Common Issues

**Build Errors:**
- Update Xcode to latest version
- Clean build folder
- Check all dependencies

**Installation Issues:**
- Trust developer certificate
- Check device compatibility
- Ensure sufficient storage

**App Crashes:**
- Check device logs in Xcode
- Verify Metal support
- Test on different devices

### Debug Mode
- Enable debug logging
- Use Xcode debugging tools
- Check device console

## Security

### App Transport Security
- Configured for development
- Consider restricting for production

### Privacy Permissions
- Camera: Not used
- Microphone: Not used
- Add more as needed

### Code Signing
- Use proper certificates
- Keep private keys secure
- Rotate certificates regularly

## Performance

### Optimization
- Release build for distribution
- Strip debug symbols
- Optimize assets

### Monitoring
- Track frame rates
- Monitor memory usage
- Optimize based on data

## Distribution

### TestFlight
1. Archive app in Xcode
2. Upload to App Store Connect
3. Add testers via TestFlight
4. Send invitations

### App Store
1. Complete TestFlight testing
2. Submit for review
3. Once approved, publicly available

### Web Distribution
1. Host files on web server
2. Update URLs in manifest.plist
3. Share distribution URL

## Support

### Documentation
- `IOS_DISTRIBUTION_GUIDE.md` - Comprehensive guide
- `INSTALLATION.md` - Installation instructions
- This README - Quick reference

### Getting Help
- Check troubleshooting section
- Review Apple's documentation
- Contact development team

## License

This project is licensed under the MIT License. See the main project README for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Changelog

### Version 1.0.0
- Initial iOS release
- 3D racing gameplay
- Touch controls
- Combat system
- Mobile-optimized UI

---

For more detailed information, see `IOS_DISTRIBUTION_GUIDE.md`.