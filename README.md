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

#### Quick Start - iOS Build

**Prerequisites**: Mac with Xcode, iPhone with iOS 13.0+

```bash
# 1) From the repository root (important!)
bash ./build_ios.sh          # generates an Xcode project and builds

# 2) Optional: auto-archive and try to install to a connected device
bash ./build_ios.sh --install

# 3) Open the generated project in Xcode if you prefer manual install
open ios/build/RacingGame3DiOS.xcodeproj

# 4) Connect iPhone, select device, choose your Team, and press ⌘R
```

Tip: If you are inside the `build/` folder and see “No such file or directory” for `./build_ios.sh`, run `../build_ios.sh` instead or go back to the repo root with `cd ..`.

#### 📚 Documentation
- **[QUICKSTART_IOS.md](QUICKSTART_IOS.md)** - Fast 5-step guide
- **[IOS_BUILD_GUIDE.md](IOS_BUILD_GUIDE.md)** - Comprehensive guide with troubleshooting
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