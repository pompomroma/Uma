# Quick Start: Build for iOS

## 🎮 Get Your Game on iPhone in 5 Steps!

### ✅ Prerequisites
- Mac computer with Xcode installed
- iPhone with iOS 13.0+
- USB cable
- Free Apple ID

---

## 📋 Quick Steps

### 1️⃣ On Mac Terminal
```bash
cd /path/to/RacingGame3D
# Build + archive + export IPA to ios/dist
./build_ios.sh --ipa
```

### 2️⃣ Open Xcode
```bash
open ios/build/RacingGame3DiOS.xcodeproj
```
**Note (Linux/Codespaces)**: Run `./build_ios.sh` to generate the project and a downloadable archive at `ios/dist/RacingGame3DiOS_XcodeProject.tar.gz`, move it to a Mac, then open the `.xcodeproj` in Xcode.

### 3️⃣ Configure Signing
- Xcode → Preferences → Accounts → Add your Apple ID
- Select project → Signing & Capabilities → Choose your Team

### 4️⃣ Connect iPhone
- Plug in via USB
- Tap "Trust" on iPhone
- Select device in Xcode toolbar

### 5️⃣ Build & Run
- Click ▶️ (Play button) or press ⌘R
- On iPhone: Settings → General → VPN & Device Management → Trust your Apple ID
- Launch the game!

---

## 🎯 Controls

**Left side**: Virtual joystick (movement)  
**Right side**: Camera drag  
**Buttons**: Combat abilities  

---

## 🔧 Troubleshooting

**"Xcode project not found" or "No such file"**
→ Run `./build_ios.sh` from the repo root to generate the project files

**"Untrusted Developer"**
→ Settings → General → VPN & Device Management → Trust

**"Failed to code sign"**
→ Add Apple ID in Xcode Preferences → Accounts

**App expires after 7 days**
→ Normal with free Apple ID, just rebuild and reinstall

**Built on non-macOS system**
→ Download `ios/dist/RacingGame3DiOS_XcodeProject.tar.gz` to a Mac, extract, then open the `.xcodeproj` in Xcode

---

## 📚 Need More Help?

See **IOS_BUILD_GUIDE.md** for detailed instructions!

---

**That's it! Have fun! 🎮🏎️**
