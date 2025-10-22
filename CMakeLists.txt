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

# Add include directories for dependencies
include_directories(${GLEW_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Game.h
    src/Math/Vector3.cpp
    src/Math/Vector3.h
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
)

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

# Build types and debug flags
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Define macro for debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_BUILD=1)
endif()
