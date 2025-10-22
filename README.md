# Uma Racing - 3D Third-Person Racing Game for iOS

A high-performance 3D racing game built with C++ and OpenGL ES, featuring smooth third-person camera movement, dynamic running/dashing mechanics, and optimized for iPhone devices.

## Features

- **3rd Person Camera**: Smooth camera following with dynamic movement and shake effects
- **Advanced Player Movement**: Running, dashing, and jumping with realistic physics
- **Racing Mechanics**: Checkpoint-based racing with timing and lap counting
- **Visual Effects**: Speed-based visual effects, dash trails, and dynamic lighting
- **Touch Controls**: Intuitive touch-based movement and gesture controls
- **Optimized for iOS**: Built specifically for iPhone with Metal/OpenGL ES 3.0

## Project Structure

```
Uma Racing/
├── src/
│   ├── engine/           # Core game engine
│   │   ├── Math.h/.cpp   # 3D math utilities (Vec3, Mat4, Quaternion)
│   │   ├── Camera.h/.cpp # 3rd person camera system
│   │   ├── Shader.h/.cpp # OpenGL shader management
│   │   ├── Mesh.h/.cpp   # 3D mesh rendering
│   │   └── Transform.h   # 3D transform component
│   ├── game/             # Game-specific code
│   │   ├── Game.h/.cpp   # Main game class
│   │   ├── Player.h/.cpp # Player controller with racing mechanics
│   │   └── Track.h/.cpp  # Racing track and checkpoints
│   ├── platform/ios/     # iOS-specific code
│   │   ├── AppDelegate.h/.mm
│   │   └── ViewController.h/.mm
│   ├── shaders/          # GLSL shaders
│   │   ├── basic.vert    # Vertex shader
│   │   └── basic.frag    # Fragment shader
│   └── main.cpp          # Application entry point
├── assets/               # Game assets (models, textures, sounds)
├── build/               # Build output directory
├── CMakeLists.txt       # CMake build configuration
├── Info.plist          # iOS app configuration
├── build_ios.sh        # iOS build script
└── README.md           # This file
```

## Prerequisites

### Required Software (macOS only)

1. **macOS 10.15+** - iOS development requires macOS
2. **Xcode 12+** - Available from the Mac App Store
3. **CMake 3.20+** - Install via Homebrew: `brew install cmake`
4. **iOS Developer Account** - Required for device deployment

### Hardware Requirements

- **Mac computer** with Apple Silicon or Intel processor
- **iPhone/iPad** running iOS 12.0+ (for device testing)
- **8GB RAM minimum** for Xcode and compilation

## Installation & Setup

### Step 1: Clone and Prepare

```bash
# Navigate to your project directory
cd /path/to/your/project

# The project files are already created in your workspace
```

### Step 2: Configure Development Team

1. Open `CMakeLists.txt`
2. Replace `YOUR_TEAM_ID` with your Apple Developer Team ID
3. You can find your Team ID in Xcode → Preferences → Accounts

### Step 3: Build the Project

```bash
# Make the build script executable (if not already)
chmod +x build_ios.sh

# Run the build script
./build_ios.sh
```

### Step 4: Open in Xcode

```bash
# Open the generated Xcode project
open build/ios/UmaRacing.xcodeproj
```

### Step 5: Configure Xcode Project

1. **Select your Development Team**:
   - Select the UmaRacing project in the navigator
   - Go to "Signing & Capabilities"
   - Choose your development team

2. **Set Bundle Identifier**:
   - Change `com.yourcompany.umaracing` to your unique identifier
   - Example: `com.yourname.umaracing`

3. **Choose Target Device**:
   - Select your connected iPhone/iPad, or
   - Choose an iOS Simulator

### Step 6: Build and Run

- Press `Cmd+R` or click the Play button in Xcode
- The game will compile and launch on your device/simulator

## Game Controls

### Touch Controls

- **Movement**: Touch and drag to move the character
  - Drag up/down: Move forward/backward
  - Drag left/right: Turn left/right
- **Dash**: Make a long, quick swipe gesture
- **Start Race**: Tap anywhere to begin the race

### Gameplay

1. **Race Start**: Tap the screen to start the race timer
2. **Checkpoints**: Navigate through yellow checkpoint markers in order
3. **Speed Effects**: Visual effects intensify with movement speed
4. **Dash Mechanics**: Use dash for speed bursts with cooldown period
5. **Camera**: Automatically follows player with smooth movement

## Technical Details

### Graphics Pipeline

- **Renderer**: OpenGL ES 3.0 with custom shaders
- **Lighting**: Phong lighting model with dynamic effects
- **Effects**: Speed-based glows, dash trails, camera shake
- **Performance**: Optimized for 60 FPS on modern iOS devices

### Physics System

- **Movement**: Smooth interpolated movement with acceleration/deceleration
- **Collision**: Ground collision detection with gravity simulation
- **States**: Idle, Running, Dashing, Jumping, Falling state machine

### Camera System

- **Type**: Third-person following camera
- **Features**: 
  - Smooth position interpolation
  - Automatic target tracking
  - Dynamic shake effects during dashing
  - Adjustable follow distance and speed

## Customization

### Modifying Game Parameters

Edit values in the respective header files:

**Player Movement** (`src/game/Player.h`):
```cpp
float runSpeed = 10.0f;        // Base running speed
float dashSpeed = 25.0f;       // Dash speed multiplier
float dashDuration = 0.3f;     // How long dash lasts
float dashCooldown = 1.0f;     // Cooldown between dashes
```

**Camera Settings** (`src/engine/Camera.h`):
```cpp
Vec3 offset(0, 5, 10);         // Camera position offset
float followSpeed = 5.0f;      // How fast camera follows
float smoothTime = 0.3f;       // Camera smoothing factor
```

### Adding New Features

1. **New Meshes**: Add to `Mesh::create*()` methods
2. **Visual Effects**: Modify fragment shader in `basic.frag`
3. **Game Mechanics**: Extend `Game.cpp` and `Player.cpp`
4. **UI Elements**: Add to `ViewController.mm`

## Troubleshooting

### Common Build Issues

**"No iOS SDK found"**
- Install Xcode from the Mac App Store
- Run `xcode-select --install` in Terminal

**"Development Team not found"**
- Sign up for Apple Developer Program
- Add your Apple ID in Xcode → Preferences → Accounts

**"Code signing error"**
- Ensure your Bundle Identifier is unique
- Check that your device is registered in your developer account

### Performance Issues

**Low FPS on device**
- Reduce mesh complexity in `Mesh.cpp`
- Lower shader quality in `basic.frag`
- Disable anti-aliasing in `ViewController.mm`

**Touch input lag**
- Ensure `preferredFramesPerSecond = 60` in ViewController
- Check for blocking operations in game loop

## Dependencies

### Included Libraries

- **OpenGL ES 3.0** - Graphics rendering (iOS system framework)
- **GLKit** - OpenGL utilities (iOS system framework)
- **UIKit** - iOS user interface (iOS system framework)
- **QuartzCore** - Animation and effects (iOS system framework)

### No External Dependencies

This project is designed to use only iOS system frameworks, eliminating the need for external package managers or third-party libraries.

## Performance Optimization

### Rendering Optimizations

- Frustum culling for off-screen objects
- Batch rendering for similar objects
- Efficient vertex buffer management
- Optimized shader uniforms

### Memory Management

- Smart pointers for automatic memory management
- Efficient mesh data structures
- Minimal dynamic allocations in game loop

## Source Information & Validity (2025)

This implementation is based on current iOS development practices and APIs that are valid as of 2025:

- **OpenGL ES 3.0**: Still supported on iOS (though Metal is preferred for new projects)
- **CMake iOS support**: Actively maintained and widely used
- **iOS 12.0+ deployment target**: Covers 95%+ of active iOS devices
- **C++17 standard**: Fully supported by current Xcode versions

### Alternative Modern Approach

For new projects in 2025, consider using:
- **Metal** instead of OpenGL ES (Apple's preferred graphics API)
- **Swift** with **Metal** for better iOS integration
- **RealityKit** for advanced 3D features

However, this C++ OpenGL ES approach remains valid and provides:
- Cross-platform compatibility potential
- Lower-level control over rendering
- Educational value for graphics programming
- Smaller binary size compared to Swift/Metal alternatives

## License

This project is provided as educational material. Modify and use according to your needs.

## Support

For technical questions about iOS development:
- Apple Developer Documentation: https://developer.apple.com/documentation/
- iOS Development Forums: https://developer.apple.com/forums/

For C++ and OpenGL questions:
- OpenGL ES Documentation: https://www.khronos.org/opengles/
- C++ Reference: https://en.cppreference.com/