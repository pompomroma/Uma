cmake_minimum_required(VERSION 3.18)
project(RacingGame3DiOS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# iOS specific settings
set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum iOS deployment version")
set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH "NO")
set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "13.0")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

# Source files
set(SOURCES
    # Core game files
    ../src/Game.cpp
    ../src/Game.h
    ../src/Math/Vector3.cpp
    ../src/Math/Vector3.h
    ../src/Math/Vector2.h
    ../src/Math/Matrix4.cpp
    ../src/Math/Matrix4.h
    ../src/Math/Quaternion.cpp
    ../src/Math/Quaternion.h
    ../src/Camera/Camera.cpp
    ../src/Camera/Camera.h
    ../src/Physics/Car.cpp
    ../src/Physics/Car.h
    ../src/Physics/PhysicsEngine.cpp
    ../src/Physics/PhysicsEngine.h
    ../src/Rendering/Renderer.cpp
    ../src/Rendering/Renderer.h
    ../src/Input/InputManager.cpp
    ../src/Input/InputManager.h
    ../src/Input/TouchInputManager.cpp
    ../src/Input/TouchInputManager.h
    ../src/World/Track.cpp
    ../src/World/Track.h
    ../src/Utils/Shader.cpp
    ../src/Utils/Shader.h
    ../src/Combat/Player.cpp
    ../src/Combat/Player.h
    ../src/Combat/Projectile.cpp
    ../src/Combat/Projectile.h
    ../src/Combat/Shield.cpp
    ../src/Combat/Shield.h
    ../src/Combat/CombatManager.cpp
    ../src/Combat/CombatManager.h
    ../src/UI/MobileUI.cpp
    ../src/UI/MobileUI.h
    ../src/Platform/PlatformDetect.h
    
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
find_library(OPENGLES_LIBRARY OpenGLES)

# Create iOS app bundle
add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCES})

# Add Metal shader to bundle
set_source_files_properties(shaders/Shader.metal PROPERTIES
    MACOSX_PACKAGE_LOCATION "Shaders"
)
target_sources(${PROJECT_NAME} PRIVATE shaders/Shader.metal)

# Set bundle properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.racinggame.mobile"
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer"
    XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
)

# Link frameworks
target_link_libraries(${PROJECT_NAME}
    ${UIKIT_LIBRARY}
    ${FOUNDATION_LIBRARY}
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${GAMECONTROLLER_LIBRARY}
    ${OPENGLES_LIBRARY}
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_IOS=1
    PLATFORM_MOBILE=1
    GRAPHICS_METAL=1
    GRAPHICS_OPENGLES=1
    INPUT_TOUCH=1
    INPUT_KEYBOARD_MOUSE=0
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
