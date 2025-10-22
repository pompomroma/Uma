#pragma once
#include "PlatformBase.h"
#include <GLFW/glfw3.h>

class DesktopPlatform : public PlatformBase {
private:
    GLFWwindow* window;
    bool isInitialized;
    bool cursorVisible;
    bool cursorLocked;
    
    // Mouse simulation for touch
    bool mouseAsTouch;
    int simulatedTouchId;
    
public:
    DesktopPlatform();
    ~DesktopPlatform() override;
    
    // Platform initialization
    bool initialize() override;
    void shutdown() override;
    
    // Window/View management
    bool createWindow(int width, int height, const std::string& title) override;
    void* getNativeWindowHandle() override { return window; }
    void setFullscreen(bool fullscreen) override;
    void getWindowSize(int& width, int& height) override;
    
    // Input handling
    void processEvents() override;
    bool shouldClose() override;
    void setCursorVisible(bool visible) override;
    void setCursorLocked(bool locked) override;
    
    // Mobile-specific (no-op on desktop)
    void vibrate(int milliseconds) override {}
    void setKeepScreenOn(bool keepOn) override {}
    bool requestPermission(const std::string& permission) override { return true; }
    void showVirtualKeyboard(bool show) override {}
    void setOrientationLock(DeviceOrientation::Type orientation) override {}
    
    // Rendering
    void swapBuffers() override;
    void* getGLContext() override;
    bool supportsOpenGLES() const override { return false; }
    int getGLESVersion() const override { return 0; }
    
    // File system
    std::string getResourcePath() const override;
    std::string getSavePath() const override;
    std::string getCachePath() const override;
    
    // Desktop-specific
    GLFWwindow* getGLFWWindow() const { return window; }
    void setMouseAsTouch(bool enable) { mouseAsTouch = enable; }
    
    // GLFW callbacks
    static void windowSizeCallback(GLFWwindow* window, int width, int height);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
private:
    void setupCallbacks();
    void simulateTouchFromMouse();
};