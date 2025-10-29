# iOS Installation Guide - Complete Walkthrough

## 🎯 Overview

This guide will help you install the Racing Game 3D on your iPhone. The process is straightforward and takes about 10-15 minutes.

---

## ⚠️ Critical Information

### iOS Apps MUST Be Built on macOS
**Important**: iOS applications can only be built and deployed on macOS with Xcode installed. 

**If you're currently using**:
- ❌ **GitHub Codespaces** - Linux environment, cannot build iOS apps
- ❌ **Windows** - Cannot build iOS apps directly
- ❌ **Linux** - Cannot build iOS apps directly  
- ✅ **macOS** - Perfect! Continue below

**If you're NOT on macOS**: Download this project to your Mac first, then follow this guide.

---

## 📋 Prerequisites Checklist

Before starting, make sure you have:

- [ ] **Mac computer** running macOS 10.15 or later
- [ ] **Xcode installed** (free from App Store)
- [ ] **iPhone** with iOS 13.0 or later
- [ ] **USB Lightning cable** to connect iPhone to Mac
- [ ] **Apple ID** (free, you probably already have one)
- [ ] **Project downloaded** to your Mac

---

## 🚀 Installation Steps

### Step 1: Get the Project on Your Mac

**Option A: Clone with Git**
```bash
git clone <your-repository-url>
cd RacingGame3D
```

**Option B: Download ZIP**
1. Download the project as a ZIP file
2. Extract it to a folder
3. Open Terminal and navigate to it:
```bash
cd /path/to/RacingGame3D
```

---

### Step 2: Run the Setup Script

In Terminal, run:
```bash
./setup_ios.sh
```

**What this does**:
- ✅ Verifies you're in the correct directory
- ✅ Checks if you're on macOS
- ✅ Generates the Xcode project
- ✅ Shows you exactly what to do next

**If you get "Permission denied"**, run:
```bash
chmod +x setup_ios.sh
./setup_ios.sh
```

---

### Step 3: Open the Project in Xcode

After the setup completes, open the project:

```bash
open ios/build/RacingGame3DiOS.xcodeproj
```

Or manually:
1. Open Xcode
2. File → Open
3. Navigate to: `RacingGame3D/ios/build/RacingGame3DiOS.xcodeproj`
4. Click "Open"

---

### Step 4: Configure Code Signing

This is required to install apps on your iPhone.

1. **Add your Apple ID to Xcode**:
   - Xcode → Preferences (or Settings) → Accounts
   - Click the "+" button
   - Sign in with your Apple ID
   - Close Preferences

2. **Configure the project**:
   - In Xcode, click on the project name in the left sidebar (top item)
   - Click on "RacingGame3DiOS" under TARGETS
   - Click "Signing & Capabilities" tab
   - Under "Team": Select your Apple ID from the dropdown
   - If you see any errors, click "Try Again"

---

### Step 5: Connect Your iPhone

1. **Connect via USB**: Plug your iPhone into your Mac
2. **Trust your Mac**: On your iPhone, tap "Trust" when prompted
3. **Select device**: In Xcode, at the top, click the device menu and select your iPhone (not "Any iOS Device")

---

### Step 6: Build and Install

1. **Click the Play button** (▶️) at the top left of Xcode, or press `⌘R`
2. **Wait for build**: This may take 1-2 minutes the first time
3. **Check your iPhone**: The app should install automatically

---

### Step 7: Trust the Developer Certificate

**First time only**: When you try to open the app on your iPhone, you'll see "Untrusted Developer"

**Fix this**:
1. On your iPhone: Settings → General → VPN & Device Management
2. Tap on your Apple ID under "Developer App"
3. Tap "Trust [Your Apple ID]"
4. Tap "Trust" in the popup

---

### Step 8: Launch the Game! 🎮

Go back to your home screen and tap the game icon. Enjoy!

---

## 🎮 How to Play

### Controls
- **Left side**: Virtual joystick (move your car)
- **Right side**: Touch and drag to look around
- **Combat buttons**: Use abilities during race

### Tips
- Practice the controls in the first few seconds
- Use boost wisely
- Combat abilities recharge over time

---

## 🔧 Troubleshooting

### "This script must be run from the project root directory!"

**Problem**: You're in the wrong folder.

**Solution**:
```bash
cd /path/to/RacingGame3D
./setup_ios.sh
```

---

### "No such file or directory: ./setup_ios.sh"

**Problem**: Either:
- You're in the wrong directory
- The file isn't executable

**Solution**:
```bash
# Make sure you're in the project root
ls -la | grep setup_ios.sh

# If you see it, make it executable
chmod +x setup_ios.sh
./setup_ios.sh

# If you don't see it, navigate to the correct directory
cd /path/to/RacingGame3D
```

---

### "Warning: You're not on macOS!"

**Problem**: You're trying to build on Linux/Windows/Codespaces.

**Solution**: You MUST use a Mac. Download the project to your Mac and run it there.

---

### "Xcode is not installed"

**Problem**: Xcode isn't installed or not in the correct location.

**Solution**:
1. Open App Store on your Mac
2. Search for "Xcode"
3. Click "Get" or "Install" (it's free but large, ~10GB)
4. Wait for installation (can take 30+ minutes)
5. Open Xcode once to agree to license
6. Run the setup script again

---

### "Failed to code sign"

**Problem**: No Apple ID configured or Team not selected.

**Solution**:
1. Xcode → Preferences → Accounts
2. Add your Apple ID if not there
3. In project settings → Signing & Capabilities
4. Select your Team
5. Try building again

---

### "Could not launch" or "Unable to install"

**Problem**: Developer certificate not trusted on iPhone.

**Solution**:
1. iPhone → Settings → General → VPN & Device Management
2. Tap your Apple ID under Developer App
3. Tap Trust

---

### "App crashes immediately"

**Problem**: Usually a compatibility issue.

**Solution**:
1. Make sure your iPhone has iOS 13.0 or later
2. Try cleaning and rebuilding:
   - Xcode → Product → Clean Build Folder (⇧⌘K)
   - Then Build again (⌘B)
3. Check Xcode console for error messages

---

### App expires after 7 days

**This is normal** with a free Apple ID!

**Solution**: 
- Reconnect your iPhone and click Run (⌘R) in Xcode
- The app will be reinstalled with a new 7-day certificate
- Your Apple ID allows 3 apps at a time with this method

**To avoid this**:
- Pay for Apple Developer Program ($99/year) - apps last 1 year
- Or just rebuild weekly (free!)

---

### "CMake Error: The source directory does not exist"

**Problem**: You're trying to run build commands from the wrong directory.

**Solution**:
```bash
# Don't run commands from subdirectories!
# Always run from project root:
cd /path/to/RacingGame3D
./setup_ios.sh
```

---

## 💡 Pro Tips

1. **Keep Xcode Updated**: Latest version = fewer problems
2. **Clean Build Folder**: If weird errors occur, clean and rebuild
3. **Check iOS Version**: Game requires iOS 13.0+
4. **Free Apple ID Limitations**: Max 3 apps, 7-day expiration
5. **Development Team**: Each Mac can only have one development team selected at a time

---

## 📱 System Requirements

### Mac Requirements
- **OS**: macOS 10.15 (Catalina) or later
- **Xcode**: Version 11.0 or later
- **Disk Space**: ~15GB (for Xcode + build)
- **RAM**: 8GB minimum, 16GB recommended

### iPhone Requirements
- **iOS**: 13.0 or later
- **Device**: iPhone 6S or newer
- **Storage**: ~100MB for the app

---

## 🎓 What's Happening Behind the Scenes?

When you run `./setup_ios.sh`:
1. Script finds the project root directory
2. Checks if you're on macOS
3. Runs `build_ios.sh` which generates an Xcode project
4. Xcode project is created at `ios/build/RacingGame3DiOS.xcodeproj`
5. Project includes all source files, frameworks, and settings

When you click Run in Xcode:
1. Xcode compiles all C++ and Objective-C++ source files
2. Links with iOS frameworks (Metal, UIKit, etc.)
3. Creates an `.app` bundle
4. Code signs it with your certificate
5. Installs it on your iPhone via USB
6. Launches the app

---

## 🆘 Still Having Issues?

If you're still stuck:

1. **Read error messages carefully** - they usually tell you exactly what's wrong
2. **Check you're on macOS** - iOS apps cannot be built anywhere else
3. **Verify all prerequisites** - Xcode installed, iPhone connected, etc.
4. **Try the manual process**: See `IOS_BUILD_GUIDE.md` for detailed technical steps
5. **Check iOS version**: Your iPhone must have iOS 13.0 or later

---

## ✅ Success Checklist

You should now have:
- [ ] Project downloaded to your Mac
- [ ] Xcode project generated
- [ ] Apple ID added to Xcode
- [ ] iPhone connected and trusted
- [ ] App built and installed
- [ ] Developer certificate trusted on iPhone
- [ ] Game running on your iPhone!

---

## 🎉 Enjoy the Game!

Have fun racing! The game includes:
- 3rd person racing gameplay
- Touch controls optimized for mobile
- Combat abilities
- Physics-based car handling
- Beautiful 3D graphics

**Share with friends**: They can follow this same guide to install it on their iPhones too!

---

**That's it! Happy racing! 🏎️💨**
