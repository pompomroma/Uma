#pragma once
#include "PlatformBase.h"

#ifdef __ANDROID__

#include <android/native_window.h>
#include <android/native_activity.h>
#include <android/asset_manager.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

class AndroidPlatform : public PlatformBase {
private:
    ANativeActivity* nativeActivity;
    ANativeWindow* nativeWindow;
    AAssetManager* assetManager;
    
    EGLDisplay eglDisplay;
    EGLSurface eglSurface;
    EGLContext eglContext;
    EGLConfig eglConfig;
    
    JNIEnv* jniEnv;
    jobject activityObject;
    
    bool isInitialized;
    bool hasFocus;
    bool isVisible;
    
    // Touch handling
    static const int MAX_TOUCHES = 10;
    int touchIds[MAX_TOUCHES];
    
public:
    AndroidPlatform();
    ~AndroidPlatform() override;
    
    // Platform initialization
    bool initialize() override;
    void shutdown() override;
    
    // Window/View management
    bool createWindow(int width, int height, const std::string& title) override;
    void* getNativeWindowHandle() override { return nativeWindow; }
    void setFullscreen(bool fullscreen) override;
    void getWindowSize(int& width, int& height) override;
    
    // Input handling
    void processEvents() override;
    bool shouldClose() override;
    void setCursorVisible(bool visible) override {} // Not applicable on Android
    void setCursorLocked(bool locked) override {} // Not applicable on Android
    
    // Mobile-specific
    void vibrate(int milliseconds) override;
    void setKeepScreenOn(bool keepOn) override;
    bool requestPermission(const std::string& permission) override;
    void showVirtualKeyboard(bool show) override;
    void setOrientationLock(DeviceOrientation::Type orientation) override;
    
    // Rendering
    void swapBuffers() override;
    void* getGLContext() override { return eglContext; }
    bool supportsOpenGLES() const override { return true; }
    int getGLESVersion() const override { return 3; }
    
    // File system
    std::string getResourcePath() const override;
    std::string getSavePath() const override;
    std::string getCachePath() const override;
    
    // Android-specific methods
    void setNativeActivity(ANativeActivity* activity);
    void setNativeWindow(ANativeWindow* window);
    void handleAppCommand(int32_t cmd);
    int32_t handleInputEvent(AInputEvent* event);
    void handleConfigChanged();
    
    // Asset management
    AAssetManager* getAssetManager() const { return assetManager; }
    std::vector<uint8_t> loadAsset(const std::string& filename);
    
private:
    bool initializeEGL();
    void terminateEGL();
    bool createEGLSurface();
    void destroyEGLSurface();
    
    void updateDeviceInfo();
    void handleTouchEvent(AInputEvent* event);
    void handleKeyEvent(AInputEvent* event);
    
    // JNI helpers
    void callJavaMethod(const char* methodName, const char* signature, ...);
    std::string getJavaString(const char* methodName);
    int getJavaInt(const char* methodName);
    float getJavaFloat(const char* methodName);
};

// Global Android app state
struct AndroidApp {
    AndroidPlatform* platform;
    ANativeActivity* activity;
    ANativeWindow* window;
    AInputQueue* inputQueue;
    AConfiguration* config;
    
    bool isRunning;
    bool hasFocus;
    bool isVisible;
    bool isPaused;
    
    void (*onAppCmd)(AndroidApp* app, int32_t cmd);
    int32_t (*onInputEvent)(AndroidApp* app, AInputEvent* event);
};

extern AndroidApp* g_androidApp;

// Android main entry point
void android_main(ANativeActivity* activity);

#endif // __ANDROID__