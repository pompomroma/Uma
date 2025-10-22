# 3D Racing Game

A 3rd person 3D racing game built with C++ and OpenGL.

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

## iOS Porting Notes

This C++ code can be ported to iOS using:
1. **Metal** instead of OpenGL for rendering
2. **UIKit** or **SwiftUI** for UI elements
3. **Core Motion** for device orientation
4. **GameController** framework for input

The porting process would require significant modifications to the rendering system and input handling.