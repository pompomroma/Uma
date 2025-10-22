#pragma once

// Platform detection macros
#if defined(__ANDROID__)
    #define PLATFORM_ANDROID 1
    #define PLATFORM_MOBILE 1
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
        #define PLATFORM_IOS 1
        #define PLATFORM_MOBILE 1
    #elif TARGET_OS_MAC
        #define PLATFORM_MACOS 1
        #define PLATFORM_DESKTOP 1
    #endif
#elif defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS 1
    #define PLATFORM_DESKTOP 1
#elif defined(__linux__)
    #if defined(__ANDROID__)
        // Already handled above
    #else
        #define PLATFORM_LINUX 1
        #define PLATFORM_DESKTOP 1
    #endif
#endif

// Set defaults if not defined
#ifndef PLATFORM_MOBILE
    #define PLATFORM_MOBILE 0
#endif

#ifndef PLATFORM_DESKTOP
    #define PLATFORM_DESKTOP 0
#endif

#ifndef PLATFORM_ANDROID
    #define PLATFORM_ANDROID 0
#endif

#ifndef PLATFORM_IOS
    #define PLATFORM_IOS 0
#endif

#ifndef PLATFORM_WINDOWS
    #define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_LINUX
    #define PLATFORM_LINUX 0
#endif

#ifndef PLATFORM_MACOS
    #define PLATFORM_MACOS 0
#endif

// Graphics API detection
#if PLATFORM_IOS
    #define GRAPHICS_METAL 1
    #define GRAPHICS_OPENGL 0
    #define GRAPHICS_OPENGLES 0
#elif PLATFORM_ANDROID
    #define GRAPHICS_OPENGLES 1
    #define GRAPHICS_OPENGL 0
    #define GRAPHICS_METAL 0
#else
    #define GRAPHICS_OPENGL 1
    #define GRAPHICS_OPENGLES 0
    #define GRAPHICS_METAL 0
#endif

// Input system detection
#if PLATFORM_MOBILE
    #define INPUT_TOUCH 1
    #define INPUT_KEYBOARD_MOUSE 0
#else
    #define INPUT_TOUCH 0
    #define INPUT_KEYBOARD_MOUSE 1
#endif

// Utility functions
namespace Platform {
    inline bool isMobile() {
        return PLATFORM_MOBILE;
    }
    
    inline bool isDesktop() {
        return PLATFORM_DESKTOP;
    }
    
    inline bool isAndroid() {
        return PLATFORM_ANDROID;
    }
    
    inline bool isIOS() {
        return PLATFORM_IOS;
    }
    
    inline bool hasTouchInput() {
        return INPUT_TOUCH;
    }
    
    inline const char* getPlatformName() {
#if PLATFORM_ANDROID
        return "Android";
#elif PLATFORM_IOS
        return "iOS";
#elif PLATFORM_WINDOWS
        return "Windows";
#elif PLATFORM_MACOS
        return "macOS";
#elif PLATFORM_LINUX
        return "Linux";
#else
        return "Unknown";
#endif
    }
}
