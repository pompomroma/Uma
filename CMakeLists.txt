cmake_minimum_required(VERSION 3.16)
project(RacingGame3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(ANDROID)
    set(PLATFORM_MOBILE TRUE)
    set(PLATFORM_ANDROID TRUE)
    message(STATUS "Building for Android")
elseif(IOS)
    set(PLATFORM_MOBILE TRUE)
    set(PLATFORM_IOS TRUE)
    message(STATUS "Building for iOS")
else()
    set(PLATFORM_DESKTOP TRUE)
    message(STATUS "Building for Desktop")
endif()

# Mobile-specific settings
if(PLATFORM_MOBILE)
    # Use OpenGL ES for mobile
    add_definitions(-DUSE_OPENGLES)
    add_definitions(-DMOBILE_PLATFORM)
    
    if(PLATFORM_ANDROID)
        add_definitions(-DANDROID_PLATFORM)
    elseif(PLATFORM_IOS)
        add_definitions(-DIOS_PLATFORM)
        set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    endif()
else()
    # Desktop OpenGL
    add_definitions(-DDESKTOP_PLATFORM)
endif()

# Find required packages based on platform
if(PLATFORM_DESKTOP)
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
elseif(PLATFORM_ANDROID)
    # Android NDK packages
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(EGL-lib EGL)
    find_library(GLESv3-lib GLESv3)
elseif(PLATFORM_IOS)
    # iOS frameworks
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(UIKIT_FRAMEWORK UIKit)
    find_library(OPENGLES_FRAMEWORK OpenGLES)
    find_library(METAL_FRAMEWORK Metal)
    find_library(METALKIT_FRAMEWORK MetalKit)
    find_library(COREMOTION_FRAMEWORK CoreMotion)
    find_library(GAMECONTROLLER_FRAMEWORK GameController)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# Platform-specific include directories
if(PLATFORM_MOBILE)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Mobile)
    if(PLATFORM_ANDROID)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Mobile/Android)
    elseif(PLATFORM_IOS)
        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/Mobile/iOS)
    endif()
endif()

# Core source files
set(CORE_SOURCES
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

# Platform-specific sources
if(PLATFORM_DESKTOP)
    set(PLATFORM_SOURCES
        src/main.cpp
        src/Input/InputManager.cpp
        src/Input/InputManager.h
        src/Rendering/Renderer.cpp
        src/Rendering/Renderer.h
    )
elseif(PLATFORM_MOBILE)
    set(PLATFORM_SOURCES
        src/Mobile/MobileMain.cpp
        src/Mobile/TouchInputManager.cpp
        src/Mobile/TouchInputManager.h
        src/Mobile/MobileRenderer.cpp
        src/Mobile/MobileRenderer.h
        src/Mobile/MobileUI.cpp
        src/Mobile/MobileUI.h
        src/Mobile/VirtualControls.cpp
        src/Mobile/VirtualControls.h
    )
    
    if(PLATFORM_ANDROID)
        list(APPEND PLATFORM_SOURCES
            src/Mobile/Android/AndroidMain.cpp
            src/Mobile/Android/AndroidInput.cpp
            src/Mobile/Android/AndroidInput.h
            src/Mobile/Android/AndroidRenderer.cpp
            src/Mobile/Android/AndroidRenderer.h
        )
    elseif(PLATFORM_IOS)
        list(APPEND PLATFORM_SOURCES
            src/Mobile/iOS/iOSMain.mm
            src/Mobile/iOS/iOSInput.mm
            src/Mobile/iOS/iOSInput.h
            src/Mobile/iOS/iOSRenderer.mm
            src/Mobile/iOS/iOSRenderer.h
            src/Mobile/iOS/MetalRenderer.mm
            src/Mobile/iOS/MetalRenderer.h
        )
    endif()
endif()

# Combine all sources
set(ALL_SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES})

# Create executable/library based on platform
if(PLATFORM_ANDROID)
    # Android shared library
    add_library(${PROJECT_NAME} SHARED ${ALL_SOURCES})
    
    # Link Android libraries
    target_link_libraries(${PROJECT_NAME}
        ${log-lib}
        ${android-lib}
        ${EGL-lib}
        ${GLESv3-lib}
    )
elseif(PLATFORM_IOS)
    # iOS app bundle
    add_executable(${PROJECT_NAME} ${ALL_SOURCES})
    
    # Link iOS frameworks
    target_link_libraries(${PROJECT_NAME}
        ${FOUNDATION_FRAMEWORK}
        ${UIKIT_FRAMEWORK}
        ${OPENGLES_FRAMEWORK}
        ${METAL_FRAMEWORK}
        ${METALKIT_FRAMEWORK}
        ${COREMOTION_FRAMEWORK}
        ${GAMECONTROLLER_FRAMEWORK}
    )
    
    # iOS app properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/iOS/Info.plist
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.racinggame3d.mobile"
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID"
    )
else()
    # Desktop executable
    add_executable(${PROJECT_NAME} ${ALL_SOURCES})
    
    # Link desktop libraries
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
    
    # Mobile-specific optimizations
    if(PLATFORM_MOBILE)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -ffast-math)
    endif()
endif()

# Copy assets for mobile platforms
if(PLATFORM_MOBILE)
    # Copy shaders
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
    
    # Copy textures
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/textures 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
    
    # Copy models
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/models 
         DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)
endif()