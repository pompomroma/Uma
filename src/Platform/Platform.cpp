#include "Platform.h"
#include "../Math/Vector2.h"
#include <iostream>
#include <vector>
#include <unordered_map>

#if PLATFORM_ANDROID
    #include <android/log.h>
    #include <jni.h>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
    #include <EGL/egl.h>
    #include <GLES3/gl3.h>
#elif PLATFORM_IOS
    #import <UIKit/UIKit.h>
    #import <OpenGLES/ES3/gl.h>
    #import <CoreMotion/CoreMotion.h>
    #import <AVFoundation/AVFoundation.h>
#else
    #include <GL/glew.h>
    #include <GLFW/glfw3.h>
#endif

namespace Platform {
    PlatformInfo PlatformManager::s_platformInfo;
    bool PlatformManager::s_initialized = false;

    bool PlatformManager::initialize() {
        if (s_initialized) return true;

        // Initialize platform-specific systems
        #if PLATFORM_ANDROID
            s_platformInfo.type = PlatformType::Android;
            s_platformInfo.graphicsAPI = GraphicsAPI::OpenGL_ES;
            s_platformInfo.hasTouch = true;
            s_platformInfo.hasAccelerometer = true;
            s_platformInfo.hasGyroscope = true;
            s_platformInfo.maxTextureSize = 4096;
            s_platformInfo.maxVertexAttribs = 16;
            s_platformInfo.supportsInstancing = true;
            s_platformInfo.supportsVAO = true;
        #elif PLATFORM_IOS
            s_platformInfo.type = PlatformType::iOS;
            s_platformInfo.graphicsAPI = GraphicsAPI::OpenGL_ES;
            s_platformInfo.hasTouch = true;
            s_platformInfo.hasAccelerometer = true;
            s_platformInfo.hasGyroscope = true;
            s_platformInfo.maxTextureSize = 4096;
            s_platformInfo.maxVertexAttribs = 16;
            s_platformInfo.supportsInstancing = true;
            s_platformInfo.supportsVAO = true;
        #else
            s_platformInfo.type = PlatformType::Windows;
            s_platformInfo.graphicsAPI = GraphicsAPI::OpenGL;
            s_platformInfo.hasTouch = false;
            s_platformInfo.hasAccelerometer = false;
            s_platformInfo.hasGyroscope = false;
            s_platformInfo.maxTextureSize = 8192;
            s_platformInfo.maxVertexAttribs = 32;
            s_platformInfo.supportsInstancing = true;
            s_platformInfo.supportsVAO = true;
        #endif

        s_platformInfo.screenDensity = 1.0f;
        s_platformInfo.screenWidth = 1920;
        s_platformInfo.screenHeight = 1080;

        s_initialized = true;
        return true;
    }

    void PlatformManager::shutdown() {
        if (!s_initialized) return;
        
        #if PLATFORM_ANDROID
            // Android cleanup
        #elif PLATFORM_IOS
            // iOS cleanup
        #else
            glfwTerminate();
        #endif
        
        s_initialized = false;
    }

    PlatformInfo PlatformManager::getPlatformInfo() {
        return s_platformInfo;
    }

    PlatformType PlatformManager::getCurrentPlatform() {
        return s_platformInfo.type;
    }

    bool PlatformManager::createWindow(int width, int height, const std::string& title) {
        s_platformInfo.screenWidth = width;
        s_platformInfo.screenHeight = height;
        
        #if PLATFORM_ANDROID
            // Android window creation is handled by the activity
            return true;
        #elif PLATFORM_IOS
            // iOS window creation is handled by the view controller
            return true;
        #else
            if (!glfwInit()) {
                std::cerr << "Failed to initialize GLFW" << std::endl;
                return false;
            }
            
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            
            GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
            if (!window) {
                std::cerr << "Failed to create GLFW window" << std::endl;
                glfwTerminate();
                return false;
            }
            
            glfwMakeContextCurrent(window);
            return true;
        #endif
    }

    void PlatformManager::destroyWindow() {
        #if !PLATFORM_MOBILE
            glfwTerminate();
        #endif
    }

    bool PlatformManager::isWindowOpen() {
        #if PLATFORM_MOBILE
            return true; // Mobile apps stay open until explicitly closed
        #else
            return !glfwWindowShouldClose(glfwGetCurrentContext());
        #endif
    }

    void PlatformManager::swapBuffers() {
        #if PLATFORM_ANDROID
            eglSwapBuffers(eglGetCurrentDisplay(), eglGetCurrentSurface(EGL_DRAW));
        #elif PLATFORM_IOS
            // iOS buffer swap is handled automatically
        #else
            glfwSwapBuffers(glfwGetCurrentContext());
        #endif
    }

    void PlatformManager::pollEvents() {
        #if !PLATFORM_MOBILE
            glfwPollEvents();
        #endif
    }

    // Input callbacks (simplified for now)
    void PlatformManager::setTouchCallback(std::function<void(int, float, float, bool)> callback) {
        // Implementation would set up touch event handling
    }

    void PlatformManager::setKeyCallback(std::function<void(int, bool)> callback) {
        // Implementation would set up keyboard event handling
    }

    void PlatformManager::setAccelerometerCallback(std::function<void(float, float, float)> callback) {
        // Implementation would set up accelerometer handling
    }

    void PlatformManager::setGyroscopeCallback(std::function<void(float, float, float)> callback) {
        // Implementation would set up gyroscope handling
    }

    // File system
    std::string PlatformManager::getAssetPath(const std::string& filename) {
        #if PLATFORM_ANDROID
            return "assets/" + filename;
        #elif PLATFORM_IOS
            return [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:filename.c_str()] ofType:nil].UTF8String;
        #else
            return "assets/" + filename;
        #endif
    }

    std::string PlatformManager::getDataPath() {
        #if PLATFORM_ANDROID
            return "/data/data/com.racinggame/files/";
        #elif PLATFORM_IOS
            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
            return [paths[0] UTF8String] + std::string("/");
        #else
            return "./data/";
        #endif
    }

    std::string PlatformManager::getCachePath() {
        #if PLATFORM_ANDROID
            return "/data/data/com.racinggame/cache/";
        #elif PLATFORM_IOS
            NSArray* paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
            return [paths[0] UTF8String] + std::string("/");
        #else
            return "./cache/";
        #endif
    }

    bool PlatformManager::fileExists(const std::string& path) {
        #if PLATFORM_ANDROID
            AAsset* asset = AAssetManager_open(AAssetManager_fromJava(env, assetManager), path.c_str(), AASSET_MODE_UNKNOWN);
            if (asset) {
                AAsset_close(asset);
                return true;
            }
            return false;
        #else
            std::ifstream file(path);
            return file.good();
        #endif
    }

    std::vector<uint8_t> PlatformManager::readFile(const std::string& path) {
        std::vector<uint8_t> data;
        
        #if PLATFORM_ANDROID
            AAsset* asset = AAssetManager_open(AAssetManager_fromJava(env, assetManager), path.c_str(), AASSET_MODE_UNKNOWN);
            if (asset) {
                size_t length = AAsset_getLength(asset);
                data.resize(length);
                AAsset_read(asset, data.data(), length);
                AAsset_close(asset);
            }
        #else
            std::ifstream file(path, std::ios::binary);
            if (file) {
                file.seekg(0, std::ios::end);
                size_t length = file.tellg();
                file.seekg(0, std::ios::beg);
                data.resize(length);
                file.read(reinterpret_cast<char*>(data.data()), length);
            }
        #endif
        
        return data;
    }

    // Audio (simplified)
    bool PlatformManager::initializeAudio() {
        return true;
    }

    void PlatformManager::shutdownAudio() {
        // Audio cleanup
    }

    void PlatformManager::playSound(const std::string& filename) {
        // Sound playback implementation
    }

    void PlatformManager::playMusic(const std::string& filename) {
        // Music playback implementation
    }

    void PlatformManager::stopMusic() {
        // Stop music implementation
    }

    void PlatformManager::setVolume(float volume) {
        // Volume control implementation
    }

    // Utilities
    void PlatformManager::showToast(const std::string& message) {
        #if PLATFORM_ANDROID
            // Android toast implementation
        #elif PLATFORM_IOS
            // iOS alert implementation
        #endif
    }

    void PlatformManager::vibrate(int duration) {
        #if PLATFORM_ANDROID
            // Android vibration implementation
        #elif PLATFORM_IOS
            // iOS haptic feedback implementation
        #endif
    }

    void PlatformManager::setScreenOrientation(int orientation) {
        // Screen orientation implementation
    }

    void PlatformManager::setKeepScreenOn(bool keepOn) {
        // Keep screen on implementation
    }

    void PlatformManager::setTargetFPS(int fps) {
        // FPS limiting implementation
    }

    int PlatformManager::getCurrentFPS() {
        return 60; // Placeholder
    }

    float PlatformManager::getBatteryLevel() {
        return 1.0f; // Placeholder
    }

    bool PlatformManager::isCharging() {
        return false; // Placeholder
    }
}