# Quick Start: Build for iOS

## 🎮 Get Your Game on iPhone in 5 Steps!

### ✅ Prerequisites
- Mac computer with Xcode installed
- iPhone with iOS 13.0+
- USB cable
- Free Apple ID

### ⚠️ Important Note
**If you're viewing this in GitHub Codespaces or on Linux**: You need to download/clone this project to your Mac first. iOS apps can only be built on macOS with Xcode.

---

## 📋 Quick Steps

### 1️⃣ On Mac Terminal
```bash
# Navigate to the project
cd /path/to/RacingGame3D

# Run the setup script
./setup_ios.sh
```
**The setup script will guide you through the entire process!**

### Alternative: Manual Build
```bash
cd /path/to/RacingGame3D
./build_ios.sh
```

### 2️⃣ Open Xcode
```bash
open ios/build/RacingGame3DiOS.xcodeproj
```

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

**"Error: This script must be run from the project root directory!"**
→ You're in the wrong directory. Run:
```bash
cd /path/to/RacingGame3D
./setup_ios.sh
```

**"Xcode project not found" or "No such file"**
→ Make sure you ran `./setup_ios.sh` or `./build_ios.sh` first to generate the project files

**"Warning: You're not on macOS!"**
→ iOS apps can only be built on Mac. Download this project to your Mac and run the setup script there

**"Untrusted Developer"**
→ Settings → General → VPN & Device Management → Trust

**"Failed to code sign"**
→ Add Apple ID in Xcode Preferences → Accounts

**App expires after 7 days**
→ Normal with free Apple ID, just rebuild and reinstall

---

## 📚 Need More Help?

See **IOS_BUILD_GUIDE.md** for detailed instructions!

---

**That's it! Have fun! 🎮🏎️**
