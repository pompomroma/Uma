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
cd /path/to/RacingGame3D   # navigate to the repo root
bash ./build_ios.sh        # generates Xcode project and builds

# Optional: archive and try to auto-install to a connected iPhone
bash ./build_ios.sh --install
```

### 2️⃣ Open Xcode
```bash
open ios/build/RacingGame3DiOS.xcodeproj
```
**Note**: If you built this on a non-macOS system, transfer the entire project folder to a Mac first, then open the `.xcodeproj` file in Xcode.

If you accidentally ran desktop build steps and ended up inside `build/`, you'll see `./build_ios.sh: No such file or directory`. Fix by running:
```bash
cd ..              # go back to repo root
bash ./build_ios.sh
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

**"Xcode project not found" or "No such file"**
→ Make sure you ran `./build_ios.sh` first to generate the project files

**"Untrusted Developer"**
→ Settings → General → VPN & Device Management → Trust

**"Failed to code sign"**
→ Add Apple ID in Xcode Preferences → Accounts

**App expires after 7 days**
→ Normal with free Apple ID, just rebuild and reinstall

**Built on non-macOS system**
→ Transfer the entire project folder to a Mac, then open the `.xcodeproj` file in Xcode

---

## 📚 Need More Help?

See **IOS_BUILD_GUIDE.md** for detailed instructions!

---

**That's it! Have fun! 🎮🏎️**
