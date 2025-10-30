# 3D Racing Game

A 3rd person 3D racing game built with C++ and OpenGL.

**Now with iOS Support! 🎮📱** - Build and play on your iPhone!

## Dependencies

### Required Libraries
- **OpenGL** - Graphics rendering
- **GLFW3** - Window management and input
- **GLEW** - OpenGL extension loading

### Installation Instructions

#### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install libglfw3-dev libglew-dev libgl1-mesa-dev
```

#### macOS (with Homebrew):
```bash
brew install glfw glew
```

#### Windows (vcpkg):
```bash
vcpkg install glfw3 glew
```

## Building the Project

```bash
mkdir build
cd build
cmake ..
make
```

## Running the Game

```bash
./RacingGame3D
```

## Controls

- **WASD** - Car movement
- **Mouse** - Camera look around
- **Space** - Brake
- **Shift** - Boost/Dash

## Project Structure

```
src/
├── main.cpp                 # Entry point
├── Game.cpp/h              # Main game class
├── Math/                   # 3D math utilities
│   ├── Vector3.cpp/h
│   ├── Matrix4.cpp/h
│   └── Quaternion.cpp/h
├── Camera/                 # Camera system
│   └── Camera.cpp/h
├── Physics/                # Physics engine
│   ├── Car.cpp/h
│   └── PhysicsEngine.cpp/h
├── Rendering/              # Graphics rendering
│   └── Renderer.cpp/h
├── Input/                  # Input handling
│   └── InputManager.cpp/h
├── World/                  # Game world
│   └── Track.cpp/h
└── Utils/                  # Utilities
    └── Shader.cpp/h
```

## 📱 iOS Build Support

### ✅ Full iOS Support Added!

The game now builds and runs on iPhone with full feature parity!

#### Quick Start - Development Build

**Prerequisites**: Mac with Xcode, iPhone with iOS 13.0+

```bash
# 1. Run build script
./build_ios.sh

# 2. Open in Xcode
open ios/build/RacingGame3DiOS.xcodeproj

# 3. Connect iPhone, select device, and click Run (⌘R)
```

#### 🚀 NEW! Distribution Build (No PC Required for Users)

**Make your game downloadable on any iPhone/iPad without connecting to a computer!**

```bash
# Build for TestFlight or App Store
./build_ios_distribution.sh testflight

# Users download directly via TestFlight app or App Store
# No USB cables, no Xcode, no Mac required for end users!
```

**See [DISTRIBUTION_QUICKSTART.md](DISTRIBUTION_QUICKSTART.md) for complete instructions.**

#### 📚 Documentation
- **[QUICKSTART_IOS.md](QUICKSTART_IOS.md)** - Fast 5-step guide
- **[IOS_BUILD_GUIDE.md](IOS_BUILD_GUIDE.md)** - Comprehensive guide with troubleshooting
- **[DISTRIBUTION_QUICKSTART.md](DISTRIBUTION_QUICKSTART.md)** - 🚀 **NEW!** Distribute via TestFlight/App Store
- **[IOS_DISTRIBUTION_GUIDE.md](IOS_DISTRIBUTION_GUIDE.md)** - Complete App Store distribution guide
- **[BUILD_STATUS.md](BUILD_STATUS.md)** - Technical details

#### 🎮 iOS Features
- ✅ Touch controls with virtual joystick
- ✅ Camera drag controls
- ✅ All racing mechanics
- ✅ PvP combat mode
- ✅ Physics simulation
- ✅ Mobile-optimized UI
- ✅ Orientation handling
- ✅ Low power mode support

#### 🛠️ Technical
- Platform-aware conditional compilation
- OpenGL on desktop, Metal stubs on iOS
- Touch input system integrated
- iOS frameworks: Metal, UIKit, GameController
- Universal codebase for all platforms