# iOS Distribution Guide - Racing Game 3D

This guide will help you distribute **Racing Game 3D** on iOS devices independently without requiring a PC connection after initial setup. Users will be able to download and install the game directly from TestFlight or the App Store.

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Apple Developer Account Setup](#apple-developer-account-setup)
3. [Building for Distribution](#building-for-distribution)
4. [TestFlight Distribution (Beta Testing)](#testflight-distribution-beta-testing)
5. [App Store Distribution (Public Release)](#app-store-distribution-public-release)
6. [Updating Your App](#updating-your-app)
7. [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Required Hardware & Software

- **Mac computer** running macOS 12.0 or later
- **Xcode 14.0 or later** (free from Mac App Store)
- **Apple Developer Account** ($99/year USD)
- **CMake** (install via: `brew install cmake`)
- **Stable internet connection** for uploading to App Store Connect

### One-Time Setup

1. **Install Xcode**
   ```bash
   # Download from Mac App Store or:
   xcode-select --install
   ```

2. **Install Xcode Command Line Tools**
   ```bash
   sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer
   ```

3. **Install CMake**
   ```bash
   brew install cmake
   ```

---

## Apple Developer Account Setup

### Step 1: Create Apple Developer Account

1. Go to [developer.apple.com](https://developer.apple.com/)
2. Click **"Account"** → **"Enroll"**
3. Complete enrollment ($99/year)
4. Wait for approval (usually 24-48 hours)

### Step 2: Create App ID

1. Log in to [developer.apple.com/account](https://developer.apple.com/account)
2. Go to **Certificates, Identifiers & Profiles**
3. Click **Identifiers** → **"+"** button
4. Select **"App IDs"** → Continue
5. Configure:
   - **Description**: Racing Game 3D
   - **Bundle ID**: `com.racinggame.mobile` (Explicit)
   - **Capabilities**: Check "Game Center" if desired
6. Click **"Register"**

### Step 3: Set Up App Store Connect

1. Go to [appstoreconnect.apple.com](https://appstoreconnect.apple.com/)
2. Click **"My Apps"** → **"+"** → **"New App"**
3. Fill in required information:
   - **Platform**: iOS
   - **Name**: Racing Game 3D
   - **Primary Language**: English
   - **Bundle ID**: Select `com.racinggame.mobile`
   - **SKU**: `racinggame3d-ios` (or any unique identifier)
   - **User Access**: Full Access
4. Click **"Create"**

### Step 4: Configure Xcode with Apple ID

1. Open **Xcode**
2. Go to **Xcode** → **Settings** → **Accounts**
3. Click **"+"** → **"Apple ID"**
4. Sign in with your Apple Developer account
5. Select your team → **"Download Manual Profiles"**

---

## Building for Distribution

### Method 1: Using the Distribution Script (Recommended)

#### For TestFlight (Beta Testing)

```bash
# Navigate to project root
cd /path/to/RacingGame3D

# Build and create .ipa for TestFlight
./build_ios_distribution.sh testflight
```

The script will:
- Generate Xcode project
- Archive the application
- Create a signed .ipa file
- Place it in `ios/build/Export/RacingGame3DiOS.ipa`

#### For App Store (Production)

```bash
./build_ios_distribution.sh appstore
```

### Method 2: Using Xcode (Alternative)

1. **Open the Project**
   ```bash
   cd ios
   cmake -B build -G Xcode \
       -DCMAKE_SYSTEM_NAME=iOS \
       -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 \
       -DCMAKE_OSX_ARCHITECTURES=arm64 \
       -DCMAKE_BUILD_TYPE=Release
   open build/RacingGame3DiOS.xcodeproj
   ```

2. **Configure Signing**
   - Select project in navigator
   - Select **"RacingGame3DiOS"** target
   - Go to **"Signing & Capabilities"** tab
   - Check **"Automatically manage signing"**
   - Select your **Team**

3. **Archive the App**
   - Select **"Any iOS Device"** as destination
   - Go to **Product** → **Archive**
   - Wait for build to complete

4. **Export Archive**
   - In Organizer window, click **"Distribute App"**
   - Select **"App Store Connect"** → Next
   - Select **"Upload"** → Next
   - Choose signing options → **"Automatically manage signing"**
   - Review content → **"Upload"**

---

## TestFlight Distribution (Beta Testing)

TestFlight allows you to distribute your game to beta testers who can download it directly on their iOS devices via the TestFlight app.

### Step 1: Upload to App Store Connect

#### Using Xcode's Organizer (GUI Method)

After archiving (see above), the app automatically uploads to App Store Connect.

#### Using Command Line (Automated Method)

1. **Generate App-Specific Password**
   - Go to [appleid.apple.com](https://appleid.apple.com)
   - Sign in → **Security** section
   - Click **"Generate Password"** under App-Specific Passwords
   - Save the password securely

2. **Upload IPA**
   ```bash
   xcrun altool --upload-app \
       --type ios \
       --file "ios/build/Export/RacingGame3DiOS.ipa" \
       --username "your-apple-id@email.com" \
       --password "your-app-specific-password"
   ```

   Or use API Key (recommended for automation):
   ```bash
   xcrun altool --upload-app \
       --type ios \
       --file "ios/build/Export/RacingGame3DiOS.ipa" \
       --apiKey YOUR_API_KEY \
       --apiIssuer YOUR_ISSUER_ID
   ```

### Step 2: Configure TestFlight

1. Go to [appstoreconnect.apple.com](https://appstoreconnect.apple.com/)
2. Select **"Racing Game 3D"**
3. Click **"TestFlight"** tab
4. Wait for build to process (10-30 minutes)
5. Once processed, build appears under **"iOS Builds"**

### Step 3: Set Up Beta Testing

#### Internal Testing (Up to 100 testers)

1. Click on the build number
2. Go to **"Internal Testing"** section
3. Click **"+"** next to Internal Testers
4. Add testers by email
5. Enable the build for testing
6. Testers receive email invitation

#### External Testing (Up to 10,000 testers)

1. Go to **"External Testing"** section
2. Click **"+"** to create a new group
3. Add testers (requires Apple review)
4. Submit for Beta App Review
5. After approval (~24 hours), testers can download

### Step 4: Testers Download the Game

1. Testers install **TestFlight** app from App Store
2. They receive invitation email
3. Click invitation link or enter invitation code
4. Open TestFlight app
5. Click **"Install"** next to Racing Game 3D
6. **Game downloads and installs directly on iPhone/iPad**
7. No PC connection required!

---

## App Store Distribution (Public Release)

### Step 1: Prepare App Store Listing

1. Go to [appstoreconnect.apple.com](https://appstoreconnect.apple.com/)
2. Select **"Racing Game 3D"**
3. Click **"App Store"** tab

#### Required Information

**App Information**
- **Privacy Policy URL**: (Create and host a privacy policy)
- **Category**: Games → Racing
- **Content Rights**: Check if applicable

**Pricing and Availability**
- **Price**: Select pricing tier (can be Free)
- **Availability**: Select countries

**App Store Listing** (for each locale)
- **Name**: Racing Game 3D
- **Subtitle**: High-speed 3D racing action
- **Description**: (See template in `ios/AppStoreMetadata.md`)
- **Keywords**: racing, 3D, car, game, speed, mobile
- **Support URL**: Your support website
- **Marketing URL**: Your website (optional)

**Screenshots** (Required)
- iPhone 6.7" Display: 1290 x 2796 pixels (3-10 screenshots)
- iPhone 6.5" Display: 1242 x 2688 pixels (3-10 screenshots)
- iPhone 5.5" Display: 1242 x 2208 pixels (3-10 screenshots)
- iPad Pro (6th Gen) 12.9": 2048 x 2732 pixels (3-10 screenshots)

**App Preview Videos** (Optional)
- 30 seconds max
- Portrait or landscape orientation

**App Icon**
- 1024 x 1024 pixels (PNG, no transparency)

**Build**
- Select the build uploaded from TestFlight

### Step 2: Submit for Review

1. Complete all required fields
2. Fill in **App Review Information**:
   - Contact information
   - Demo account (if needed)
   - Notes for reviewer
3. Fill in **Version Release**:
   - Choose automatic or manual release
4. Click **"Add for Review"**
5. Click **"Submit for Review"**

### Step 3: App Review Process

- **Typical Timeline**: 1-3 days
- **Status Updates**: Check App Store Connect
- **Possible Outcomes**:
  - ✅ **Approved**: App goes live (or scheduled)
  - ❌ **Rejected**: Fix issues and resubmit
  - ⏸️ **Metadata Rejected**: Update info and resubmit

### Step 4: App Goes Live!

Once approved:
- App appears on App Store
- **Users search "Racing Game 3D"**
- **Users tap "Get" → Install**
- **Downloads directly to iPhone/iPad**
- **No PC required!**

---

## Updating Your App

### For New Versions

1. **Update Version Numbers**
   - Edit `ios/Info.plist`:
     ```xml
     <key>CFBundleShortVersionString</key>
     <string>1.1</string>  <!-- User-facing version -->
     <key>CFBundleVersion</key>
     <string>2</string>     <!-- Build number, must increment -->
     ```

2. **Build New Version**
   ```bash
   ./build_ios_distribution.sh testflight
   # or
   ./build_ios_distribution.sh appstore
   ```

3. **Upload to App Store Connect**
   - Use same upload methods as before

4. **For TestFlight**
   - New build automatically available to testers

5. **For App Store**
   - Create new version in App Store Connect
   - Select new build
   - Update "What's New" section
   - Submit for review

---

## Troubleshooting

### Common Issues

#### 1. "No signing certificate found"

**Solution:**
```bash
# Open Xcode
open -a Xcode

# Go to Xcode → Settings → Accounts
# Select your Apple ID → Download Manual Profiles
```

#### 2. "Failed to create provisioning profile"

**Solution:**
- Ensure Bundle ID matches: `com.racinggame.mobile`
- Check Apple Developer Portal for conflicting profiles
- Try manual code signing in Xcode

#### 3. "Build failed to upload"

**Solution:**
```bash
# Validate before uploading
xcrun altool --validate-app \
    --type ios \
    --file "ios/build/Export/RacingGame3DiOS.ipa" \
    --username "your-apple-id@email.com" \
    --password "your-app-specific-password"
```

#### 4. "TestFlight build processing stuck"

**Solution:**
- Wait 30-60 minutes
- Check for email from Apple about issues
- Ensure Info.plist has all required keys
- Check for missing privacy usage descriptions

#### 5. "App rejected for missing functionality"

**Solution:**
- Ensure game runs properly on device
- Provide clear demo instructions in review notes
- Include demo video if possible

### Getting Help

- **Apple Developer Forums**: [developer.apple.com/forums](https://developer.apple.com/forums/)
- **App Store Connect Help**: [help.apple.com/app-store-connect](https://help.apple.com/app-store-connect/)
- **Technical Support**: [developer.apple.com/support](https://developer.apple.com/support/)

---

## Summary: How Users Download Your Game

### Via TestFlight (Beta)
1. User receives invitation email
2. User installs TestFlight app
3. User accepts invitation
4. Game downloads directly to device
5. **No computer needed!**

### Via App Store (Production)
1. User opens App Store on iPhone/iPad
2. User searches "Racing Game 3D"
3. User taps "Get" button
4. Game downloads and installs
5. **No computer needed!**

---

## Cost Summary

- **Apple Developer Account**: $99/year (required)
- **TestFlight**: Free (included)
- **App Store**: Free (included)
- **Mac for building**: One-time cost (if you don't have one)

---

## Timeline

| Task | Time |
|------|------|
| Apple Developer enrollment | 24-48 hours |
| First build and upload | 30-60 minutes |
| TestFlight processing | 10-30 minutes |
| TestFlight Beta Review | Not required for internal testing |
| External TestFlight review | ~24 hours |
| App Store review | 1-3 days |
| **Total to TestFlight** | **1-2 days** |
| **Total to App Store** | **2-4 days** |

---

## Additional Resources

- [App Store Review Guidelines](https://developer.apple.com/app-store/review/guidelines/)
- [TestFlight Beta Testing](https://developer.apple.com/testflight/)
- [App Store Connect Guide](https://developer.apple.com/app-store-connect/)
- [Human Interface Guidelines](https://developer.apple.com/design/human-interface-guidelines/)

---

**Congratulations!** Once published, anyone with an iPhone or iPad can download and play Racing Game 3D directly from TestFlight or the App Store, with no computer connection required!
