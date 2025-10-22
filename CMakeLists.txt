cmake_minimum_required(VERSION 3.20)
project(UmaRacing VERSION 1.0.0 LANGUAGES CXX OBJCXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# iOS specific settings
if(IOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
    
    # Enable ARC (Automatic Reference Counting)
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

# Find required packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(src)
include_directories(src/engine)
include_directories(src/game)
include_directories(src/platform/ios)

# Source files
set(ENGINE_SOURCES
    src/engine/Renderer.cpp
    src/engine/Camera.cpp
    src/engine/Physics.cpp
    src/engine/Input.cpp
    src/engine/Math.cpp
    src/engine/Mesh.cpp
    src/engine/Shader.cpp
    src/engine/Texture.cpp
    src/engine/Transform.cpp
)

set(GAME_SOURCES
    src/game/Game.cpp
    src/game/Player.cpp
    src/game/Track.cpp
    src/game/GameState.cpp
    src/game/RacingController.cpp
)

set(PLATFORM_SOURCES
    src/platform/ios/AppDelegate.mm
    src/platform/ios/ViewController.mm
    src/platform/ios/IOSInput.mm
    src/platform/ios/IOSRenderer.mm
    src/main.cpp
)

# Create executable
add_executable(UmaRacing
    ${ENGINE_SOURCES}
    ${GAME_SOURCES}
    ${PLATFORM_SOURCES}
)

# Link libraries
if(IOS)
    target_link_libraries(UmaRacing
        "-framework Foundation"
        "-framework UIKit"
        "-framework OpenGLES"
        "-framework GLKit"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework MetalKit"
        "-framework AVFoundation"
        "-framework AudioToolbox"
    )
else()
    target_link_libraries(UmaRacing ${OPENGL_LIBRARIES})
endif()

# Set bundle properties for iOS
if(IOS)
    set_target_properties(UmaRacing PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.yourcompany.umaracing"
    )
endif()