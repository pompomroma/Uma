# Quick Start: Build for iOS

## 🎮 Get Your Game on iPhone in 5 Steps!

### ✅ Prerequisites
- **Mac computer with Xcode installed** (iOS builds only work on macOS)
- iPhone with iOS 13.0+
- USB cable
- Free Apple ID

**⚠️ Note**: If you're on Linux/Windows, see `IOS_BUILD_GUIDE.md` for instructions on transferring the project to a Mac first.

---

## 📋 Quick Steps

### 1️⃣ On Mac Terminal
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
