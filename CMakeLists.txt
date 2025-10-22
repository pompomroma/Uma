cmake_minimum_required(VERSION 3.16)
project(RacingGame3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(ANDROID)
    message(STATUS "Building for Android")
    set(PLATFORM_NAME "Android")
    set(MOBILE_BUILD TRUE)
elseif(IOS)
    message(STATUS "Building for iOS")
    set(PLATFORM_NAME "iOS")
    set(MOBILE_BUILD TRUE)
    set(CMAKE_OSX_SYSROOT "iphoneos")
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "YOUR_TEAM_ID")
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "12.0")
else()
    message(STATUS "Building for Desktop")
    set(PLATFORM_NAME "Desktop")
    set(MOBILE_BUILD FALSE)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)

# Common source files
set(COMMON_SOURCES
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
)

# Platform-specific sources
set(PLATFORM_SOURCES
    src/Platform/PlatformBase.cpp
    src/Platform/PlatformBase.h
)

# Mobile-specific sources
if(MOBILE_BUILD)
    list(APPEND PLATFORM_SOURCES
        src/Input/TouchInputManager.cpp
        src/Input/TouchInputManager.h
        src/Rendering/MobileRenderer.h
        src/Rendering/MobileRenderer.cpp
        src/Game/MobileGame.h
        src/Game/MobileGame.cpp
    )
endif()

# Android-specific configuration
if(ANDROID)
    list(APPEND PLATFORM_SOURCES
        src/Platform/AndroidPlatform.h
        src/Platform/AndroidPlatform.cpp
        src/Platform/android_main.cpp
    )
    
    # Android libraries
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(EGL-lib EGL)
    find_library(GLESv3-lib GLESv3)
    
    set(PLATFORM_LIBS
        ${log-lib}
        ${android-lib}
        ${EGL-lib}
        ${GLESv3-lib}
    )
    
    # Create shared library for Android
    add_library(${PROJECT_NAME} SHARED
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
    )
    
# iOS-specific configuration
elseif(IOS)
    list(APPEND PLATFORM_SOURCES
        src/Platform/iOSPlatform.h
        src/Platform/iOSPlatform.mm
        src/Platform/iOSAppDelegate.h
        src/Platform/iOSAppDelegate.mm
        src/Platform/iOSViewController.h
        src/Platform/iOSViewController.mm
        src/main_ios.mm
    )
    
    # iOS frameworks
    find_library(UIKIT_FRAMEWORK UIKit)
    find_library(OPENGLES_FRAMEWORK OpenGLES)
    find_library(QUARTZCORE_FRAMEWORK QuartzCore)
    find_library(COREGRAPHICS_FRAMEWORK CoreGraphics)
    find_library(FOUNDATION_FRAMEWORK Foundation)
    find_library(COREMOTION_FRAMEWORK CoreMotion)
    find_library(AUDIOTOOLBOX_FRAMEWORK AudioToolbox)
    find_library(AVFOUNDATION_FRAMEWORK AVFoundation)
    
    set(PLATFORM_LIBS
        ${UIKIT_FRAMEWORK}
        ${OPENGLES_FRAMEWORK}
        ${QUARTZCORE_FRAMEWORK}
        ${COREGRAPHICS_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
        ${COREMOTION_FRAMEWORK}
        ${AUDIOTOOLBOX_FRAMEWORK}
        ${AVFOUNDATION_FRAMEWORK}
    )
    
    # Resources for iOS
    set(IOS_RESOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist
        ${CMAKE_CURRENT_SOURCE_DIR}/ios/LaunchScreen.storyboard
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
    )
    
    # Create iOS app bundle
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
        ${IOS_RESOURCES}
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.yourcompany.racinggame3d"
        MACOSX_BUNDLE_BUNDLE_NAME "Racing Game 3D"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
        RESOURCE "${IOS_RESOURCES}"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
    )
    
# Desktop configuration
else()
    list(APPEND PLATFORM_SOURCES
        src/Platform/DesktopPlatform.h
        src/Platform/DesktopPlatform.cpp
        src/main.cpp
    )
    
    # Find desktop packages
    find_package(OpenGL REQUIRED)
    find_package(glfw3 REQUIRED)
    find_package(GLEW REQUIRED)
    
    set(PLATFORM_LIBS
        OpenGL::GL
        glfw
        GLEW::GLEW
    )
    
    # Create desktop executable
    add_executable(${PROJECT_NAME}
        ${COMMON_SOURCES}
        ${PLATFORM_SOURCES}
    )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} ${PLATFORM_LIBS})

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
    if(MOBILE_BUILD)
        target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
    endif()
endif()

# Copy resources
if(NOT IOS)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)
endif()

# Android-specific build configuration
if(ANDROID)
    # Set Android properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/android/app/src/main/jniLibs/${ANDROID_ABI}
    )
endif()