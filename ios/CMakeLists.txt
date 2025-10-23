cmake_minimum_required(VERSION 3.18)
project(RacingGame3DiOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# iOS specific settings
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0")
set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build archs" FORCE)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Source files
set(SOURCES
    # Core game files
    ../src/Game.cpp
    ../src/Math/Vector3.cpp
    ../src/Math/Matrix4.cpp
    ../src/Math/Quaternion.cpp
    ../src/Camera/Camera.cpp
    ../src/Physics/Car.cpp
    ../src/Physics/PhysicsEngine.cpp
    ../src/Rendering/Renderer.cpp
    ../src/Input/InputManager.cpp
    ../src/Input/TouchInputManager.cpp
    ../src/World/Track.cpp
    ../src/Utils/Shader.cpp
    ../src/Combat/Player.cpp
    ../src/Combat/Projectile.cpp
    ../src/Combat/Shield.cpp
    ../src/Combat/CombatManager.cpp
    ../src/UI/MobileUI.cpp
    
    # iOS specific
    ios_main.mm
    GameViewController.mm
)

# iOS framework dependencies
find_library(UIKIT_LIBRARY UIKit)
find_library(FOUNDATION_LIBRARY Foundation)
find_library(METAL_LIBRARY Metal)
find_library(METALKIT_LIBRARY MetalKit)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(GAMECONTROLLER_LIBRARY GameController)

# Create iOS app bundle
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})

# Set bundle properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.racinggame.mobile"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
)

# Link frameworks
target_link_libraries(${PROJECT_NAME}
    ${UIKIT_LIBRARY}
    ${FOUNDATION_LIBRARY}
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${GAMECONTROLLER_LIBRARY}
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_IOS=1
    GRAPHICS_METAL=1
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE
    -fobjc-arc
    -fvisibility=hidden
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3
        -ffast-math
        -DNDEBUG
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O0
        -g
        -DDEBUG
    )
endif()
