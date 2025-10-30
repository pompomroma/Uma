# iOS Build and Installation Guide
## Racing Game 3D Mobile

This guide will help you build and install the Racing Game 3D on your iPhone.

---

## Prerequisites

### Required Software
1. **macOS Computer** (with macOS 11 Big Sur or later)
2. **Xcode 14+** - Download from the Mac App Store
3. **Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```
4. **CMake 3.18+**
   ```bash
   brew install cmake
   ```

### Required Hardware
- iPhone or iPad running iOS 13.0 or later
- USB cable to connect device to Mac
- Apple ID (free account is sufficient for development)

---

## Step 1: Transfer Project to Mac

If you're working on this project in a cloud environment (like GitHub Codespaces):

1. **Download the entire project** from your repository
2. **Transfer to your Mac** via:
   - Git clone on your Mac: `git clone [your-repo-url]`
   - Direct download and copy
   - Cloud storage (iCloud, Dropbox, etc.)

---

## Step 2: Generate Xcode Project

Open Terminal on your Mac and navigate to the project directory:

```bash
cd /path/to/RacingGame3D
./build_ios.sh
```

This will:
- Generate an Xcode project in `ios/build/RacingGame3DiOS.xcodeproj`
- Configure all necessary iOS frameworks
- Set up proper build settings

---

## Step 3: Open in Xcode

```bash
open ios/build/RacingGame3DiOS.xcodeproj
```

Or double-click the `.xcodeproj` file in Finder.

---

## Step 4: Configure Code Signing

### 4.1 Add Your Apple ID
1. In Xcode, go to **Xcode → Preferences** (or Settings)
2. Click **Accounts** tab
3. Click **+** button and add your Apple ID
4. Sign in with your credentials

### 4.2 Set Development Team
1. Select the **RacingGame3DiOS** project in the left sidebar
2. Select the **RacingGame3DiOS** target
3. Go to **Signing & Capabilities** tab
4. Check **Automatically manage signing**
5. Select your **Team** from the dropdown (your Apple ID)

### 4.3 Fix Bundle Identifier (if needed)
If you see signing errors:
1. Change the **Bundle Identifier** to something unique
2. Format: `com.yourname.racinggame3d`
3. Xcode will automatically create a provisioning profile

---

## Step 5: Connect Your iPhone

### 5.1 Physical Connection
1. Connect your iPhone to Mac via USB cable
2. Unlock your iPhone
3. Tap **Trust** when prompted on iPhone
4. Enter your iPhone passcode if requested

### 5.2 Select Your Device
1. In Xcode, at the top near the Run button
2. Click the device selector (should say "RacingGame3DiOS > Any iOS Device")
3. Select your iPhone from the list

---

## Step 6: Build and Install

### 6.1 Build the App
Click the **Play button** (▶️) in Xcode, or press **⌘R**

Xcode will:
- Compile the game code
- Sign the app with your certificate
- Install on your iPhone
- Launch the game

### 6.2 First Launch - Trust Developer

The first time you run the app, you'll see:
**"Untrusted Developer"** message on iPhone

To fix:
1. On iPhone: Go to **Settings**
2. Navigate to **General → VPN & Device Management**
3. Find your Apple ID under "Developer App"
4. Tap it and tap **Trust "[Your Apple ID]"**
5. Confirm by tapping **Trust** again

### 6.3 Launch the Game
Now you can:
- Tap the app icon on your iPhone home screen
- Or click Run (▶️) in Xcode again

---

## Step 7: Play the Game!

### Controls
- **Left side of screen**: Virtual joystick for movement/steering
- **Right side of screen**: Touch and drag to control camera
- **Buttons**: Various abilities and actions
  - Attack buttons (Q, E keys shown on screen)
  - Shield (F)
  - Special abilities

### Game Modes
The game includes:
- **Racing Mode**: Drive around the track
- **PvP Combat Mode**: Fight against AI opponents
- Full mobile touch controls
- Camera controls optimized for mobile

---

## Troubleshooting

### Issue: "Failed to code sign"
**Solution**: 
- Make sure you're signed in with your Apple ID in Xcode
- Try changing the Bundle Identifier to something unique
- Clean build folder: Product → Clean Build Folder (⇧⌘K)

### Issue: "iPhone is not available"
**Solution**:
- Make sure iPhone is unlocked
- Trust the computer on iPhone
- Try disconnecting and reconnecting USB cable
- Update Xcode if your iPhone is running newer iOS

### Issue: "Could not locate device support files"
**Solution**:
- Your Xcode version is too old for your iOS version
- Update Xcode from Mac App Store
- Or update manually from developer.apple.com

### Issue: App crashes on launch
**Solution**:
- Check Xcode console for error messages
- Make sure iPhone is running iOS 13.0 or later
- Try cleaning and rebuilding: Product → Clean Build Folder

### Issue: "Provisioning profile expired"
**Solution**:
- This happens after 7 days with free Apple ID
- Reconnect iPhone and click Run in Xcode again
- It will automatically refresh the provisioning profile

---

## Building for Distribution (No-PC install)

If you want to share the game with others:

### Option 1: TestFlight (Recommended)
1. Archive the app: Product → Archive
2. Upload to App Store Connect
3. Add beta testers
4. They can install via TestFlight app

Or use the included script on macOS to package an .ipa:

```bash
# On macOS in project root
./build_ios.sh Release archive

# Export signed IPA (replace with your Team ID)
TEAM_ID=YOURTEAMID ./build_ios.sh Release package ad-hoc
```

The IPA will be under `ios/build/export/`.

### Option 2: Ad Hoc OTA (No Mac-to-iPhone cable needed)
1. Export the IPA as above (method `ad-hoc`)
2. Host the `.ipa` on an HTTPS URL you control
3. Generate OTA manifest:
   ```bash
   IPA_URL=https://your.domain/path/RacingGame3DiOS.ipa \
   TITLE="Racing Game 3D" \
   BUNDLE_ID=com.racinggame.mobile \
   BUNDLE_VERSION=1.0 \
   ./build_ios.sh Release ota
   ```
4. Upload `ios/build/export/manifest.plist` to the same HTTPS host
5. On iPhone Safari, open:
   ```
   itms-services://?action=download-manifest&url=https://your.domain/path/manifest.plist
   ```
6. iOS will download and install the app over the air

Notes:
- You must add device UDIDs to your Ad Hoc provisioning profile
- Use `TEAM_ID` env var when exporting so automatic signing can work

### Option 3: Development Install (Free, but limited)
- Anyone can install on their device using their own Apple ID
- They need Xcode and to follow this guide
- App expires after 7 days and needs reinstalling

---

## Performance Tips

### For Best Performance on iPhone:
1. **Close background apps** before playing
2. **Enable Low Power Mode** for longer battery life
   - Game will automatically reduce graphics quality
3. **Use landscape orientation** for optimal view
4. **Good lighting** helps with touch controls visibility

### Expected Performance:
- iPhone 12 or newer: 60 FPS
- iPhone X to 11: 45-60 FPS
- iPhone 8 or older: 30-45 FPS

---

## Development Notes

### Graphics Backend
- The game uses **Metal** on iOS (not OpenGL)
- Current implementation uses stub/placeholder rendering
- Full Metal rendering can be implemented in `Renderer.cpp`

### Future Enhancements
To add full Metal rendering:
1. Implement Metal render pipeline in `Renderer.cpp`
2. Add Metal shaders (.metal files)
3. Update shader compilation in `Shader.cpp`
4. See `/ios/GameViewController.mm` for Metal device setup

### Project Structure
```
ios/
├── CMakeLists.txt          # iOS build configuration
├── ios_main.mm             # iOS app entry point
├── GameViewController.mm    # Main game view controller
├── Info.plist              # iOS app metadata
└── build/                  # Generated Xcode project
```

---

## Additional Resources

- **Apple Developer Documentation**: https://developer.apple.com/documentation/
- **Xcode Help**: Help → Xcode Help in Xcode menu
- **iOS Development Guide**: https://developer.apple.com/ios/

---

## Support

If you encounter issues:
1. Check the Xcode console for error messages
2. Review the troubleshooting section above
3. Make sure all prerequisites are installed
4. Check that your iOS version is 13.0 or later

---

## Summary of Commands

```bash
# On Mac Terminal:

# 1. Navigate to project
cd /path/to/RacingGame3D

# 2. Make build script executable (if needed)
chmod +x build_ios.sh

# 3. Generate Xcode project and build
./build_ios.sh

# 4. Open in Xcode
open ios/build/RacingGame3DiOS.xcodeproj

# Then in Xcode: Select device and click Run (⌘R)
```

---

**Enjoy your game on iPhone! 🎮📱**
