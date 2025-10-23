# Racing Game 3D

A cross-platform 3D racing game with PvP combat, built with C++ and supporting both desktop (OpenGL) and mobile (Metal on iOS) platforms.

## 🎮 Platforms Supported

- **Desktop**: Windows, macOS, Linux (OpenGL)
- **Mobile**: iOS (Metal rendering)
- **Android**: Coming soon

## 🎯 Game Features

### Racing Mode
- 3D racing with realistic physics
- Multiple cars and tracks
- Lap timing and best time tracking
- AI opponents

### PvP Combat Mode
- Real-time multiplayer combat
- Character progression system
- Multiple abilities (attacks, shields, teleport)
- Level-up stat distribution

### Cross-Platform Features
- **Desktop**: Keyboard + mouse controls
- **Mobile**: Touch controls with virtual joystick
- **Adaptive UI**: Responsive interface for all screen sizes
- **Metal Rendering**: Native iOS graphics performance

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

## 🔨 Building the Project

### Desktop Build

```bash
# Install dependencies first (see above)
mkdir build
cd build
cmake ..
make
```

### iOS Build

**Requirements**: macOS with Xcode installed

```bash
# Make scripts executable
chmod +x build_ios.sh deploy_ios.sh

# Build for iOS
./build_ios.sh

# Deploy to device (opens Xcode)
./deploy_ios.sh
```

For detailed iOS setup instructions, see [iOS_BUILD_GUIDE.md](iOS_BUILD_GUIDE.md).

## 🚀 Running the Game

### Desktop
```bash
./RacingGame3D
```

### iOS
1. Build using the iOS build script
2. Open the generated Xcode project
3. Configure code signing with your Apple Developer account
4. Run on your iOS device

## 🎮 Controls

### Desktop Controls

#### Racing Mode
- **WASD** - Car movement (accelerate/brake/steer)
- **Mouse** - Camera look around
- **Space** - Brake/Reverse
- **Shift** - Boost
- **C** - Handbrake

#### PvP Combat Mode
- **WASD** - Character movement
- **Mouse** - Look direction
- **Q** - Fist attack
- **E** - Laser attack
- **F** - Shield toggle
- **R** - Teleport
- **Tab** - Stats menu

### iOS Touch Controls

#### Racing Mode
- **Left side**: Virtual joystick for steering
- **Right side**: Touch buttons for accelerate/brake/boost
- **Two-finger drag**: Camera rotation

#### PvP Combat Mode
- **Left side**: Virtual joystick for movement
- **Right side**: Action buttons (Q/E/F/R)
- **Camera**: Automatic follow with touch override

### Menu Navigation
- **Desktop**: Mouse click, keyboard shortcuts
- **iOS**: Touch gestures, tap to select

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

## iOS Porting Notes

This C++ code can be ported to iOS using:
1. **Metal** instead of OpenGL for rendering
2. **UIKit** or **SwiftUI** for UI elements
3. **Core Motion** for device orientation
4. **GameController** framework for input

The porting process would require significant modifications to the rendering system and input handling.