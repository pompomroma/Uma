#pragma once
#include "PlatformBase.h"

#ifdef __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE

#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>

// Forward declarations for Objective-C types
#ifdef __OBJC__
@class UIWindow;
@class UIViewController;
@class EAGLContext;
@class CAEAGLLayer;
@class UITouch;
@class NSSet;
@class UIEvent;
#else
typedef void UIWindow;
typedef void UIViewController;
typedef void EAGLContext;
typedef void CAEAGLLayer;
typedef void UITouch;
typedef void NSSet;
typedef void UIEvent;
#endif

class iOSPlatform : public PlatformBase {
private:
    UIWindow* window;
    UIViewController* viewController;
    EAGLContext* glContext;
    CAEAGLLayer* eaglLayer;
    
    GLuint colorRenderBuffer;
    GLuint depthRenderBuffer;
    GLuint frameBuffer;
    
    bool isInitialized;
    bool isActive;
    
    // Touch tracking
    static const int MAX_TOUCHES = 10;
    void* activeTouchPointers[MAX_TOUCHES];
    
public:
    iOSPlatform();
    ~iOSPlatform() override;
    
    // Platform initialization
    bool initialize() override;
    void shutdown() override;
    
    // Window/View management
    bool createWindow(int width, int height, const std::string& title) override;
    void* getNativeWindowHandle() override;
    void setFullscreen(bool fullscreen) override;
    void getWindowSize(int& width, int& height) override;
    
    // Input handling
    void processEvents() override;
    bool shouldClose() override;
    void setCursorVisible(bool visible) override {} // Not applicable on iOS
    void setCursorLocked(bool locked) override {} // Not applicable on iOS
    
    // Mobile-specific
    void vibrate(int milliseconds) override;
    void setKeepScreenOn(bool keepOn) override;
    bool requestPermission(const std::string& permission) override;
    void showVirtualKeyboard(bool show) override;
    void setOrientationLock(DeviceOrientation::Type orientation) override;
    
    // Rendering
    void swapBuffers() override;
    void* getGLContext() override;
    bool supportsOpenGLES() const override { return true; }
    int getGLESVersion() const override { return 3; }
    
    // File system
    std::string getResourcePath() const override;
    std::string getSavePath() const override;
    std::string getCachePath() const override;
    
    // iOS-specific methods
    void handleTouchesBegan(NSSet* touches, UIEvent* event);
    void handleTouchesMoved(NSSet* touches, UIEvent* event);
    void handleTouchesEnded(NSSet* touches, UIEvent* event);
    void handleTouchesCancelled(NSSet* touches, UIEvent* event);
    
    void handleOrientationChange();
    void handleMemoryWarning();
    void handleApplicationWillResignActive();
    void handleApplicationDidBecomeActive();
    void handleApplicationWillTerminate();
    
    // Metal support (for future)
    bool supportsMetal() const;
    void* getMetalDevice();
    
private:
    bool createGLContext();
    void destroyGLContext();
    bool createRenderBuffers();
    void destroyRenderBuffers();
    
    void updateDeviceInfo();
    TouchPoint convertUITouch(UITouch* touch);
    int getTouchId(UITouch* touch);
    void registerTouch(UITouch* touch);
    void unregisterTouch(UITouch* touch);
};

// Global iOS app instance
extern iOSPlatform* g_iOSPlatform;

#endif // TARGET_OS_IPHONE
#endif // __APPLE__