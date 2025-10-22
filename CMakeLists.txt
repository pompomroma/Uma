cmake_minimum_required(VERSION 3.16)
project(RacingGame3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options
option(USE_SDL2_MOBILE "Build with SDL2 and OpenGL ES for mobile" OFF)

# Dependencies
if (USE_SDL2_MOBILE)
    add_definitions(-DPLATFORM_MOBILE)
    # SDL2 for windowing/input across Android/iOS
    find_package(SDL2 REQUIRED)
else()
    add_definitions(-DPLATFORM_DESKTOP)
    # Desktop path: OpenGL + GLFW + GLEW
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
if (USE_SDL2_MOBILE)
    if (TARGET SDL2::SDL2)
        # Prefer modern CMake target includes
    else()
        include_directories(${SDL2_INCLUDE_DIRS})
    endif()
endif()

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
    src/Combat/Player.cpp
    src/Combat/Player.h
    src/Combat/Projectile.cpp
    src/Combat/Projectile.h
    src/Combat/Shield.cpp
    src/Combat/Shield.h
    src/Combat/CombatManager.cpp
    src/Combat/CombatManager.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
if (USE_SDL2_MOBILE)
    if (TARGET SDL2::SDL2)
        target_link_libraries(${PROJECT_NAME} SDL2::SDL2)
    else()
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    endif()

    # Link against OpenGL ES depending on platform
    if (APPLE)
        # iOS/tvOS use the OpenGLES framework
        find_library(OPENGLES_FRAMEWORK OpenGLES)
        if (OPENGLES_FRAMEWORK)
            target_link_libraries(${PROJECT_NAME} ${OPENGLES_FRAMEWORK})
        endif()
    elseif(ANDROID)
        target_link_libraries(${PROJECT_NAME} GLESv3)
    else()
        # Fallback for Linux builds targeting GLES
        find_library(GLESv3_LIB GLESv3)
        if (GLESv3_LIB)
            target_link_libraries(${PROJECT_NAME} ${GLESv3_LIB})
        else()
            message(WARNING "GLESv3 not found; falling back to desktop GL for local builds")
            find_package(OpenGL REQUIRED)
            target_link_libraries(${PROJECT_NAME} OpenGL::GL)
        endif()
    endif()
else()
    target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        GLEW::GLEW
    )
endif()

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()