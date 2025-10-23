cmake_minimum_required(VERSION 3.16)
project(RacingGame3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Game.h
    src/Math/Vector3.cpp
    src/Math/Vector3.h
    src/Math/Vector2.h
    src/Math/Matrix4.cpp
    src/Math/Matrix4.h
    src/Math/Quaternion.cpp
    src/Math/Quaternion.h
    src/Camera/Camera.cpp
    src/Camera/Camera.h
    src/Physics/Car.cpp
    src/Physics/Car.h
    src/Physics/PhysicsEngine.cpp
    src/Physics/PhysicsEngine.h
    src/Rendering/Renderer.cpp
    src/Rendering/Renderer.h
    src/Input/InputManager.cpp
    src/Input/InputManager.h
    src/World/Track.cpp
    src/World/Track.h
    src/Utils/Shader.cpp
    src/Utils/Shader.h
    src/Combat/Player.cpp
    src/Combat/Player.h
    src/Combat/Projectile.cpp
    src/Combat/Projectile.h
    src/Combat/Shield.cpp
    src/Combat/Shield.h
    src/Combat/CombatManager.cpp
    src/Combat/CombatManager.h
    src/Platform/PlatformDetect.h
)

# Add mobile sources if building for mobile platforms
if(PLATFORM_MOBILE)
    list(APPEND SOURCES
        src/Input/TouchInputManager.cpp
        src/Input/TouchInputManager.h
        src/UI/MobileUI.cpp
        src/UI/MobileUI.h
    )
endif()

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    OpenGL::GL 
    glfw 
    GLEW::GLEW
)

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()