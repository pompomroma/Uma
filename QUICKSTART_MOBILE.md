# Quick Start Guide - Mobile Version

Get Racing Game 3D running on your mobile device in minutes!

---

## 🚀 Quick Start

### For Android Users

#### Prerequisites (One-time setup)
```bash
# Install Android Studio and NDK
# Download from: https://developer.android.com/studio

# Set environment variables
export ANDROID_HOME=/path/to/android/sdk
export ANDROID_NDK=/path/to/android/ndk
```

#### Build & Install
```bash
# 1. Clone the repository
git clone <repository-url>
cd RacingGame3D

# 2. Build for Android
./build_android.sh Release arm64-v8a

# 3. Install on device
adb install android/build/outputs/apk/release/RacingGame3DMobile-release.apk
```

**That's it!** Launch the app on your device.

---

### For iOS Users

#### Prerequisites (One-time setup)
- macOS with Xcode installed
- Apple Developer account (for device deployment)

#### Build & Install
```bash
# 1. Clone the repository
git clone <repository-url>
cd RacingGame3D

# 2. Generate Xcode project
./build_ios.sh Release

# 3. Open in Xcode
open ios/build/RacingGame3DiOS.xcodeproj

# 4. In Xcode:
#    - Select your development team
#    - Connect your iOS device
#    - Click Run (⌘R)
```

**That's it!** The app will install and launch on your device.

---

## 🎮 Basic Controls

### Racing Mode
```
Left Joystick  → Steering + Acceleration/Brake
Right Joystick → Camera control
Right Buttons  → Boost, Handbrake
Top Right      → Pause, Reset
```

### Combat Mode
```
Left Joystick  → Movement (360°)
Right Joystick → Aim/Look
Right Buttons  → Attack, Shield, Teleport
Top Right      → Pause, Stats Menu
```

---

## 📱 First Time Setup

1. **Launch the app**
2. **Tutorial** will guide you through controls
3. **Customize** controls in Settings if needed
4. **Choose mode**: Racing or Combat
5. **Start playing!**

---

## ⚙️ Recommended Settings

### For Best Performance
- Resolution: Auto
- Graphics: Medium
- Frame Rate: 60 FPS
- Effects: Low

### For Best Battery Life
- Resolution: 75%
- Graphics: Low
- Frame Rate: 30 FPS
- Low Power Mode: ON

### For Best Visuals
- Resolution: 100%
- Graphics: High
- Frame Rate: 60 FPS
- Effects: High
- Shadows: High

---

## 🔧 Common Issues

### Android

**"App won't install"**
→ Enable "Install from Unknown Sources" in Settings

**"Touch controls not showing"**
→ Check Settings → Controls → Show Virtual Controls

**"Low FPS"**
→ Lower graphics settings or enable 30 FPS mode

### iOS

**"Untrusted Developer"**
→ Settings → General → Device Management → Trust developer

**"Code signing failed"**
→ Set your team in Xcode project settings

**"App crashes on launch"**
→ Check device meets iOS 13.0+ requirement

---

## 💡 Pro Tips

1. **Customize controls** before your first race
2. **Adjust sensitivity** to match your preference
3. **Use headphones** for better audio experience
4. **Close other apps** for better performance
5. **Keep device charged** for longer sessions

---

## 📖 Need More Help?

- **Full Documentation**: See `MOBILE_README.md`
- **Control Details**: See `MOBILE_CONTROLS.md`
- **Implementation Details**: See `MOBILE_IMPLEMENTATION_SUMMARY.md`

---

## 🎯 What's Next?

1. **Complete tutorial** (3 minutes)
2. **Try Racing Mode** (start with easy track)
3. **Try Combat Mode** (single-player match)
4. **Customize controls** (find your perfect setup)
5. **Challenge friends** (multiplayer coming soon!)

---

**Enjoy the game! 🏎️💨**
