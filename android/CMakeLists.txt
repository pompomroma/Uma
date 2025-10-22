cmake_minimum_required(VERSION 3.18.1)
project(RacingGame3DMobile)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Android specific settings
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${ANDROID_NDK}/sources/android/native_app_glue)

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
    
    # Android specific
    android_main.cpp
    
    # Native app glue
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Find required libraries
find_library(log-lib log)
find_library(android-lib android)
find_library(EGL-lib EGL)
find_library(GLESv3-lib GLESv3)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${log-lib}
    ${android-lib}
    ${EGL-lib}
    ${GLESv3-lib}
)

# Compiler definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    PLATFORM_ANDROID=1
    GRAPHICS_OPENGLES=1
)

# Compiler flags for optimization
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
