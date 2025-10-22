# Debug Guide for 3D Racing Game

## Quick Start

### Building for Debug
```bash
# Option 1: Use the debug build script (recommended)
./debug_build.sh

# Option 2: Use the main build script with debug flag
./build.sh --debug

# Option 3: Manual build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)
```

### Running with Debugger
```bash
# With GDB
gdb ./build/RacingGame3D
(gdb) run
(gdb) bt  # Show backtrace on crash

# With LLDB (macOS/Linux)
lldb ./build/RacingGame3D
(lldb) run
(lldb) bt  # Show backtrace on crash

# With Valgrind (memory debugging)
valgrind --tool=memcheck --leak-check=full ./build/RacingGame3D
```

## Debug Features

### 1. Debug Build Configuration
- **Debug Symbols**: Full debugging information included (`-g`)
- **No Optimization**: Code compiled with `-O0` for accurate debugging
- **Debug Macros**: `DEBUG_BUILD` and `DEBUG` defined
- **Assertions**: Debug assertions enabled
- **Verbose Output**: Detailed console logging

### 2. Debug Rendering
The game includes visual debugging features accessible through the debug info display:

#### Enable Debug Rendering
```cpp
// In your game code
game.setShowDebugInfo(true);
```

#### Debug Visuals Include:
- **Coordinate Axes**: Red (X), Green (Y), Blue (Z) axes at origin
- **Grid**: Ground plane grid for spatial reference
- **Bounding Boxes**: Collision volume visualization
- **Performance Stats**: FPS, draw calls, triangle count

### 3. Debug Console Output
When built in debug mode, the game outputs detailed information:

#### Physics Debug Info
```
Physics Debug Info:
  Cars: 1
  Gravity: 0, -9.81, 0
  Ground Height: 0
  Ground Friction: 0.8
  Car 0 - Pos: (0, 0, 0) Vel: (0, 0, 0)
```

#### Car Debug Info
```
Car Debug Info:
  Position: (0, 0, 0)
  Velocity: (0, 0, 0)
  Speed: 0 / 50
  Throttle: 0
  Brake: 0
  Steer: 0
  Boost: OFF
  Handbrake: OFF
  On Ground: YES
  Mass: 1200 kg
```

#### Track Debug Info
```
Track Debug Info:
  Type: 0
  Radius: 50
  Width: 10
  Resolution: 100
  Banking: 0
  Vertices: 400
  Indices: 1200
  Checkpoints: 4
  Current Lap: 0
  Current Checkpoint: 0
```

### 4. Debug Controls
- **F1**: Toggle debug info display
- **F2**: Toggle wireframe rendering
- **F3**: Reset car position
- **F4**: Reload shaders
- **F5**: Toggle physics debug visualization

### 5. Performance Debugging

#### Built-in Performance Monitoring
```cpp
// Get performance stats
float fps = game.getFPS();
int drawCalls = renderer->getDrawCalls();
int triangles = renderer->getTrianglesRendered();
```

#### Profiling Tools
```bash
# CPU profiling with perf (Linux)
perf record ./RacingGame3D
perf report

# Memory profiling with Valgrind
valgrind --tool=massif ./RacingGame3D
ms_print massif.out.* > memory_profile.txt

# GPU profiling (NVIDIA)
nvprof ./RacingGame3D
```

## Common Debugging Scenarios

### 1. Crash on Startup
```bash
gdb ./RacingGame3D
(gdb) run
# When it crashes:
(gdb) bt
(gdb) info registers
(gdb) print variable_name
```

**Common Causes:**
- Missing dependencies (GLFW, GLEW, OpenGL)
- Shader compilation errors
- Null pointer dereferences

### 2. Performance Issues
```bash
# Check for memory leaks
valgrind --leak-check=full ./RacingGame3D

# Profile CPU usage
perf top -p $(pgrep RacingGame3D)
```

**Common Causes:**
- Inefficient rendering loops
- Memory leaks
- Excessive draw calls

### 3. Physics Problems
Enable physics debug output and check:
- Car position/velocity values
- Ground collision detection
- Force calculations

### 4. Rendering Issues
Enable debug rendering to visualize:
- Camera position and orientation
- Object bounding boxes
- Lighting calculations

## Debug Build vs Release Build

| Feature | Debug Build | Release Build |
|---------|-------------|---------------|
| Optimization | `-O0` (none) | `-O3` (full) |
| Debug Symbols | ✅ Included | ❌ Stripped |
| Assertions | ✅ Enabled | ❌ Disabled |
| Console Output | ✅ Verbose | ❌ Minimal |
| File Size | Larger | Smaller |
| Performance | Slower | Faster |
| Debugging | Easy | Difficult |

## Debugging Tools

### 1. GDB Commands
```bash
# Basic commands
(gdb) run                    # Start program
(gdb) break main             # Set breakpoint
(gdb) continue               # Continue execution
(gdb) step                   # Step into function
(gdb) next                   # Step over function
(gdb) print var              # Print variable value
(gdb) bt                     # Show call stack
(gdb) info locals            # Show local variables

# Advanced commands
(gdb) watch variable         # Watch variable changes
(gdb) catch throw            # Catch C++ exceptions
(gdb) thread apply all bt    # Show all thread stacks
```

### 2. Valgrind Options
```bash
# Memory error detection
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./RacingGame3D

# Performance profiling
valgrind --tool=callgrind ./RacingGame3D
kcachegrind callgrind.out.*

# Cache profiling
valgrind --tool=cachegrind ./RacingGame3D
```

### 3. Static Analysis
```bash
# Clang static analyzer
scan-build make

# Cppcheck
cppcheck --enable=all src/
```

## Troubleshooting

### Build Issues
1. **Missing GLAD**: Ensure GLAD is properly installed or bundled
2. **OpenGL Version**: Check OpenGL 3.3+ support
3. **Compiler Errors**: Verify C++17 compiler support

### Runtime Issues
1. **Segmentation Fault**: Run with GDB to get stack trace
2. **OpenGL Errors**: Check shader compilation logs
3. **Performance**: Use profiling tools to identify bottlenecks

### Debug Output Not Showing
1. Ensure debug build: `cmake .. -DCMAKE_BUILD_TYPE=Debug`
2. Check console output redirection
3. Verify `DEBUG_BUILD` macro is defined

## Best Practices

1. **Always use debug builds during development**
2. **Enable all compiler warnings** (`-Wall -Wextra -pedantic`)
3. **Use static analysis tools** regularly
4. **Profile early and often** to catch performance issues
5. **Test with different build configurations**
6. **Use version control** to track debugging changes
7. **Document debugging findings** for future reference

## Additional Resources

- [GDB Manual](https://www.gnu.org/software/gdb/documentation/)
- [Valgrind Documentation](https://valgrind.org/docs/)
- [OpenGL Debugging](https://www.khronos.org/opengl/wiki/Debug_Output)
- [C++ Debugging Best Practices](https://isocpp.org/wiki/faq/debugging)