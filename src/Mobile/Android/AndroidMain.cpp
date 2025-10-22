#ifdef ANDROID_PLATFORM

#include <jni.h>
#include <android/log.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <memory>
#include <chrono>

#include "AndroidRenderer.h"
#include "AndroidInput.h"
#include "../MobileMain.cpp"

#define LOG_TAG "RacingGame3D"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

// Android-specific game state
struct AndroidGameState {
    ANativeWindow* window;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    bool isInitialized;
    bool isRunning;
    std::chrono::steady_clock::time_point lastFrameTime;
    
    AndroidGameState() : window(nullptr), display(EGL_NO_DISPLAY), 
                        surface(EGL_NO_SURFACE), context(EGL_NO_CONTEXT),
                        width(0), height(0), isInitialized(false), isRunning(false) {}
};

static AndroidGameState g_androidState;

// EGL initialization
bool initializeEGL() {
    LOGI("Initializing EGL...");
    
    // Get EGL display
    g_androidState.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_androidState.display == EGL_NO_DISPLAY) {
        LOGE("Failed to get EGL display");
        return false;
    }
    
    // Initialize EGL
    EGLint majorVersion, minorVersion;
    if (!eglInitialize(g_androidState.display, &majorVersion, &minorVersion)) {
        LOGE("Failed to initialize EGL");
        return false;
    }
    
    LOGI("EGL version: %d.%d", majorVersion, minorVersion);
    
    // Configure EGL
    const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(g_androidState.display, configAttribs, &config, 1, &numConfigs)) {
        LOGE("Failed to choose EGL config");
        return false;
    }
    
    // Create EGL context
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    g_androidState.context = eglCreateContext(g_androidState.display, config, EGL_NO_CONTEXT, contextAttribs);
    if (g_androidState.context == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context");
        return false;
    }
    
    // Create EGL surface
    g_androidState.surface = eglCreateWindowSurface(g_androidState.display, config, g_androidState.window, nullptr);
    if (g_androidState.surface == EGL_NO_SURFACE) {
        LOGE("Failed to create EGL surface");
        return false;
    }
    
    // Make context current
    if (!eglMakeCurrent(g_androidState.display, g_androidState.surface, g_androidState.surface, g_androidState.context)) {
        LOGE("Failed to make EGL context current");
        return false;
    }
    
    // Get surface dimensions
    eglQuerySurface(g_androidState.display, g_androidState.surface, EGL_WIDTH, &g_androidState.width);
    eglQuerySurface(g_androidState.display, g_androidState.surface, EGL_HEIGHT, &g_androidState.height);
    
    LOGI("EGL initialized successfully: %dx%d", g_androidState.width, g_androidState.height);
    return true;
}

void shutdownEGL() {
    LOGI("Shutting down EGL...");
    
    if (g_androidState.display != EGL_NO_DISPLAY) {
        eglMakeCurrent(g_androidState.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (g_androidState.context != EGL_NO_CONTEXT) {
            eglDestroyContext(g_androidState.display, g_androidState.context);
            g_androidState.context = EGL_NO_CONTEXT;
        }
        
        if (g_androidState.surface != EGL_NO_SURFACE) {
            eglDestroySurface(g_androidState.display, g_androidState.surface);
            g_androidState.surface = EGL_NO_SURFACE;
        }
        
        eglTerminate(g_androidState.display);
        g_androidState.display = EGL_NO_DISPLAY;
    }
    
    LOGI("EGL shutdown complete");
}

void renderFrame() {
    if (!g_androidState.isInitialized || !g_androidState.isRunning) {
        return;
    }
    
    // Calculate delta time
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = 0.016f; // Default to 60 FPS
    
    if (g_androidState.lastFrameTime.time_since_epoch().count() > 0) {
        deltaTime = std::chrono::duration<float>(currentTime - g_androidState.lastFrameTime).count();
    }
    g_androidState.lastFrameTime = currentTime;
    
    // Clamp delta time to prevent large jumps
    deltaTime = std::min(deltaTime, 0.033f); // Max 30 FPS
    
    // Update and render game
    mobile_game_update(deltaTime);
    mobile_game_render();
    
    // Swap buffers
    eglSwapBuffers(g_androidState.display, g_androidState.surface);
}

// JNI interface functions
extern "C" {

JNIEXPORT jboolean JNICALL
Java_com_racinggame3d_GameActivity_nativeInitialize(JNIEnv *env, jobject thiz, jobject surface) {
    LOGI("nativeInitialize called");
    
    // Get native window from surface
    g_androidState.window = ANativeWindow_fromSurface(env, surface);
    if (!g_androidState.window) {
        LOGE("Failed to get native window from surface");
        return JNI_FALSE;
    }
    
    // Initialize EGL
    if (!initializeEGL()) {
        LOGE("Failed to initialize EGL");
        ANativeWindow_release(g_androidState.window);
        g_androidState.window = nullptr;
        return JNI_FALSE;
    }
    
    // Initialize mobile game
    float screenDensity = 160.0f; // Default Android density
    if (!mobile_game_initialize(g_androidState.width, g_androidState.height, screenDensity)) {
        LOGE("Failed to initialize mobile game");
        shutdownEGL();
        ANativeWindow_release(g_androidState.window);
        g_androidState.window = nullptr;
        return JNI_FALSE;
    }
    
    g_androidState.isInitialized = true;
    g_androidState.isRunning = true;
    g_androidState.lastFrameTime = std::chrono::steady_clock::now();
    
    LOGI("Native initialization complete");
    return JNI_TRUE;
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeShutdown(JNIEnv *env, jobject thiz) {
    LOGI("nativeShutdown called");
    
    g_androidState.isRunning = false;
    
    if (g_androidState.isInitialized) {
        mobile_game_shutdown();
        g_androidState.isInitialized = false;
    }
    
    shutdownEGL();
    
    if (g_androidState.window) {
        ANativeWindow_release(g_androidState.window);
        g_androidState.window = nullptr;
    }
    
    LOGI("Native shutdown complete");
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeRender(JNIEnv *env, jobject thiz) {
    renderFrame();
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativePause(JNIEnv *env, jobject thiz) {
    LOGI("nativePause called");
    g_androidState.isRunning = false;
    mobile_game_pause();
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeResume(JNIEnv *env, jobject thiz) {
    LOGI("nativeResume called");
    g_androidState.isRunning = true;
    g_androidState.lastFrameTime = std::chrono::steady_clock::now();
    mobile_game_resume();
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeSurfaceChanged(JNIEnv *env, jobject thiz, jint width, jint height) {
    LOGI("nativeSurfaceChanged: %dx%d", width, height);
    
    g_androidState.width = width;
    g_androidState.height = height;
    
    if (g_androidState.isInitialized) {
        mobile_game_screen_size_changed(width, height);
    }
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeTouchDown(JNIEnv *env, jobject thiz, jint touchId, jfloat x, jfloat y, jfloat pressure) {
    if (g_androidState.isInitialized && g_androidState.isRunning) {
        mobile_game_touch_down(touchId, x, y, pressure);
    }
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeTouchMove(JNIEnv *env, jobject thiz, jint touchId, jfloat x, jfloat y, jfloat pressure) {
    if (g_androidState.isInitialized && g_androidState.isRunning) {
        mobile_game_touch_move(touchId, x, y, pressure);
    }
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeTouchUp(JNIEnv *env, jobject thiz, jint touchId, jfloat x, jfloat y) {
    if (g_androidState.isInitialized && g_androidState.isRunning) {
        mobile_game_touch_up(touchId, x, y);
    }
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeSetPerformanceLevel(JNIEnv *env, jobject thiz, jint level) {
    if (g_androidState.isInitialized) {
        mobile_game_set_performance_level(level);
    }
}

JNIEXPORT void JNICALL
Java_com_racinggame3d_GameActivity_nativeLowMemory(JNIEnv *env, jobject thiz) {
    LOGI("nativeLowMemory called");
    if (g_androidState.isInitialized) {
        mobile_game_low_memory();
    }
}

} // extern "C"

#endif // ANDROID_PLATFORM