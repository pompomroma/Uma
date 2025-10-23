#include <android/log.h>
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include "../src/Game.h"
#include <memory>

#define LOG_TAG "RacingGame3D"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

struct AndroidApp {
    std::unique_ptr<Game> game;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    bool isInitialized;
    bool isVisible;
};

static bool initEGL(AndroidApp* app, ANativeWindow* window) {
    app->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (app->display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return false;
    }
    
    if (!eglInitialize(app->display, nullptr, nullptr)) {
        LOGE("eglInitialize failed");
        return false;
    }
    
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
    };
    
    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(app->display, attribs, &config, 1, &numConfigs)) {
        LOGE("eglChooseConfig failed");
        return false;
    }
    
    EGLint format;
    eglGetConfigAttrib(app->display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(window, 0, 0, format);
    
    app->surface = eglCreateWindowSurface(app->display, config, window, nullptr);
    if (app->surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return false;
    }
    
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 3,
        EGL_NONE
    };
    
    app->context = eglCreateContext(app->display, config, EGL_NO_CONTEXT, contextAttribs);
    if (app->context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return false;
    }
    
    if (!eglMakeCurrent(app->display, app->surface, app->surface, app->context)) {
        LOGE("eglMakeCurrent failed");
        return false;
    }
    
    eglQuerySurface(app->display, app->surface, EGL_WIDTH, &app->width);
    eglQuerySurface(app->display, app->surface, EGL_HEIGHT, &app->height);
    
    LOGI("EGL initialized: %dx%d", app->width, app->height);
    return true;
}

static void terminateEGL(AndroidApp* app) {
    if (app->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(app->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        
        if (app->context != EGL_NO_CONTEXT) {
            eglDestroyContext(app->display, app->context);
            app->context = EGL_NO_CONTEXT;
        }
        
        if (app->surface != EGL_NO_SURFACE) {
            eglDestroySurface(app->display, app->surface);
            app->surface = EGL_NO_SURFACE;
        }
        
        eglTerminate(app->display);
        app->display = EGL_NO_DISPLAY;
    }
}

static bool initGame(AndroidApp* app) {
    if (!app->game) {
        app->game = std::make_unique<Game>();
    }
    
    if (!app->game->initialize(app->width, app->height, "Racing Game 3D")) {
        LOGE("Failed to initialize game");
        return false;
    }
    
    app->isInitialized = true;
    LOGI("Game initialized successfully");
    return true;
}

static int32_t handleInput(struct android_app* androidApp, AInputEvent* event) {
    AndroidApp* app = static_cast<AndroidApp*>(androidApp->userData);
    if (!app || !app->game) return 0;
    
    int32_t eventType = AInputEvent_getType(event);
    
    if (eventType == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t actionMasked = action & AMOTION_EVENT_ACTION_MASK;
        int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) 
                               >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int32_t pointerId = AMotionEvent_getPointerId(event, pointerIndex);
        
        float x = AMotionEvent_getX(event, pointerIndex);
        float y = AMotionEvent_getY(event, pointerIndex);
        float pressure = AMotionEvent_getPressure(event, pointerIndex);
        
        int phase = 0; // Default to began
        switch (actionMasked) {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
                phase = 0; // Began
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                phase = 1; // Moved
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
                phase = 3; // Ended
                break;
            case AMOTION_EVENT_ACTION_CANCEL:
                phase = 4; // Cancelled
                break;
        }
        
        app->game->handleTouchInput(pointerId, x, y, phase, pressure);
        return 1;
    }
    
    return 0;
}

static void handleAppCommand(struct android_app* androidApp, int32_t cmd) {
    AndroidApp* app = static_cast<AndroidApp*>(androidApp->userData);
    if (!app) return;
    
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW");
            if (androidApp->window != nullptr) {
                if (initEGL(app, androidApp->window)) {
                    if (!app->isInitialized) {
                        initGame(app);
                    }
                }
            }
            break;
            
        case APP_CMD_TERM_WINDOW:
            LOGI("APP_CMD_TERM_WINDOW");
            if (app->game) {
                app->game->shutdown();
            }
            terminateEGL(app);
            app->isInitialized = false;
            break;
            
        case APP_CMD_GAINED_FOCUS:
            LOGI("APP_CMD_GAINED_FOCUS");
            app->isVisible = true;
            if (app->game) {
                app->game->resume();
            }
            break;
            
        case APP_CMD_LOST_FOCUS:
            LOGI("APP_CMD_LOST_FOCUS");
            app->isVisible = false;
            if (app->game) {
                app->game->pause();
            }
            break;
            
        case APP_CMD_CONFIG_CHANGED:
            LOGI("APP_CMD_CONFIG_CHANGED");
            if (app->surface != EGL_NO_SURFACE) {
                int32_t width, height;
                eglQuerySurface(app->display, app->surface, EGL_WIDTH, &width);
                eglQuerySurface(app->display, app->surface, EGL_HEIGHT, &height);
                if (width != app->width || height != app->height) {
                    app->width = width;
                    app->height = height;
                    if (app->game) {
                        app->game->handleDeviceOrientation(
                            static_cast<float>(width), 
                            static_cast<float>(height)
                        );
                    }
                }
            }
            break;
            
        case APP_CMD_LOW_MEMORY:
            LOGI("APP_CMD_LOW_MEMORY");
            if (app->game) {
                app->game->setLowPowerMode(true);
            }
            break;
    }
}

void android_main(struct android_app* androidApp) {
    AndroidApp app = {};
    app.display = EGL_NO_DISPLAY;
    app.surface = EGL_NO_SURFACE;
    app.context = EGL_NO_CONTEXT;
    app.isInitialized = false;
    app.isVisible = false;
    
    androidApp->userData = &app;
    androidApp->onAppCmd = handleAppCommand;
    androidApp->onInputEvent = handleInput;
    
    LOGI("Starting Racing Game 3D for Android");
    
    // Main loop
    while (true) {
        int events;
        struct android_poll_source* source;
        
        // Poll events
        while (ALooper_pollAll(app.isVisible && app.isInitialized ? 0 : -1, 
                              nullptr, &events, (void**)&source) >= 0) {
            if (source != nullptr) {
                source->process(androidApp, source);
            }
            
            if (androidApp->destroyRequested != 0) {
                LOGI("Destroy requested");
                if (app.game) {
                    app.game->shutdown();
                }
                terminateEGL(&app);
                return;
            }
        }
        
        // Update and render game
        if (app.isVisible && app.isInitialized && app.game && app.game->getIsRunning()) {
            static auto lastTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
            lastTime = currentTime;
            
            // Cap delta time
            deltaTime = std::min(deltaTime, 0.033f); // Max 30 FPS minimum
            
            app.game->update(deltaTime);
            app.game->render();
            
            eglSwapBuffers(app.display, app.surface);
        }
    }
}
