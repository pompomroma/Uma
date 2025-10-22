# CMakeLists.txt for Android Racing Game 3D
cmake_minimum_required(VERSION 3.22.1)

project(RacingGame3D)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Android specific settings
set(PLATFORM_ANDROID TRUE)
set(PLATFORM_MOBILE TRUE)
add_definitions(-DANDROID_PLATFORM)
add_definitions(-DMOBILE_PLATFORM)
add_definitions(-DUSE_OPENGLES)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../../external)

# Find Android libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)
find_library(jnigraphics-lib jnigraphics)

# Core source files (relative to project root)
set(CORE_SOURCES
    ../../../src/Game.cpp
    ../../../src/Math/Vector3.cpp
    ../../../src/Math/Matrix4.cpp
    ../../../src/Math/Quaternion.cpp
    ../../../src/Camera/Camera.cpp
    ../../../src/Physics/Car.cpp
    ../../../src/Physics/PhysicsEngine.cpp
    ../../../src/World/Track.cpp
    ../../../src/Utils/Shader.cpp
    ../../../src/Combat/Player.cpp
    ../../../src/Combat/Projectile.cpp
    ../../../src/Combat/Shield.cpp
    ../../../src/Combat/CombatManager.cpp
)

# Mobile source files
set(MOBILE_SOURCES
    ../../../src/Mobile/MobileMain.cpp
    ../../../src/Mobile/MobileRenderer.cpp
    ../../../src/Mobile/TouchInputManager.cpp
    ../../../src/Mobile/VirtualControls.cpp
)

# Android specific sources
set(ANDROID_SOURCES
    AndroidMain.cpp
    AndroidInput.cpp
    AndroidRenderer.cpp
)

# Combine all sources
set(ALL_SOURCES ${CORE_SOURCES} ${MOBILE_SOURCES} ${ANDROID_SOURCES})

# Create shared library
add_library(${PROJECT_NAME} SHARED ${ALL_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
    ${jnigraphics-lib}
)

# Compiler flags
target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall -Wextra -O3 -ffast-math
    -fvisibility=hidden
    -ffunction-sections -fdata-sections
)

# Linker flags
set_target_properties(${PROJECT_NAME} PROPERTIES
    LINK_FLAGS "-Wl,--gc-sections -Wl,--strip-all"
)