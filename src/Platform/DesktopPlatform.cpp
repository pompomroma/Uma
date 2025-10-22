#include "DesktopPlatform.h"
#include <iostream>
#include <filesystem>

DesktopPlatform::DesktopPlatform() 
    : window(nullptr)
    , isInitialized(false)
    , cursorVisible(true)
    , cursorLocked(false)
    , mouseAsTouch(false)
    , simulatedTouchId(0) {
    
    currentPlatform = Platform::Desktop;
}

DesktopPlatform::~DesktopPlatform() {
    shutdown();
}

bool DesktopPlatform::initialize() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Set OpenGL version hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    isInitialized = true;
    updateDeviceInfo();
    return true;
}

void DesktopPlatform::shutdown() {
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    if (isInitialized) {
        glfwTerminate();
        isInitialized = false;
    }
}

bool DesktopPlatform::createWindow(int width, int height, const std::string& title) {
    if (!isInitialized) {
        if (!initialize()) {
            return false;
        }
    }
    
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return false;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    setupCallbacks();
    
    deviceInfo.screenWidth = width;
    deviceInfo.screenHeight = height;
    
    return true;
}

void DesktopPlatform::setFullscreen(bool fullscreen) {
    if (!window) return;
    
    if (fullscreen) {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    } else {
        glfwSetWindowMonitor(window, nullptr, 100, 100, deviceInfo.screenWidth, deviceInfo.screenHeight, 0);
    }
}

void DesktopPlatform::getWindowSize(int& width, int& height) {
    if (window) {
        glfwGetWindowSize(window, &width, &height);
    } else {
        width = deviceInfo.screenWidth;
        height = deviceInfo.screenHeight;
    }
}

void DesktopPlatform::processEvents() {
    glfwPollEvents();
    
    if (mouseAsTouch) {
        simulateTouchFromMouse();
    }
}

bool DesktopPlatform::shouldClose() {
    return window ? glfwWindowShouldClose(window) : false;
}

void DesktopPlatform::setCursorVisible(bool visible) {
    if (!window) return;
    cursorVisible = visible;
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void DesktopPlatform::setCursorLocked(bool locked) {
    if (!window) return;
    cursorLocked = locked;
    glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void DesktopPlatform::swapBuffers() {
    if (window) {
        glfwSwapBuffers(window);
    }
}

void* DesktopPlatform::getGLContext() {
    return window ? glfwGetCurrentContext() : nullptr;
}

std::string DesktopPlatform::getResourcePath() const {
    return std::filesystem::current_path().string() + "/resources/";
}

std::string DesktopPlatform::getSavePath() const {
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    return home + "/.racinggame/saves/";
}

std::string DesktopPlatform::getCachePath() const {
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : ".";
    return home + "/.racinggame/cache/";
}

void DesktopPlatform::setupCallbacks() {
    glfwSetWindowSizeCallback(window, windowSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void DesktopPlatform::windowSizeCallback(GLFWwindow* window, int width, int height) {
    DesktopPlatform* platform = static_cast<DesktopPlatform*>(glfwGetWindowUserPointer(window));
    if (platform) {
        platform->deviceInfo.screenWidth = width;
        platform->deviceInfo.screenHeight = height;
    }
}

void DesktopPlatform::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handle keyboard input
}

void DesktopPlatform::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    DesktopPlatform* platform = static_cast<DesktopPlatform*>(glfwGetWindowUserPointer(window));
    if (platform && platform->mouseAsTouch) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        TouchPoint touch;
        touch.id = platform->simulatedTouchId;
        touch.x = (float)xpos;
        touch.y = (float)ypos;
        touch.pressure = 1.0f;
        touch.size = 1.0f;
        
        if (action == GLFW_PRESS) {
            touch.state = TouchPoint::Began;
            platform->handleTouchBegin(touch);
        } else if (action == GLFW_RELEASE) {
            touch.state = TouchPoint::Ended;
            platform->handleTouchEnd(touch);
        }
    }
}

void DesktopPlatform::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    DesktopPlatform* platform = static_cast<DesktopPlatform*>(glfwGetWindowUserPointer(window));
    if (platform && platform->mouseAsTouch) {
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        if (state == GLFW_PRESS) {
            TouchPoint touch;
            touch.id = platform->simulatedTouchId;
            touch.x = (float)xpos;
            touch.y = (float)ypos;
            touch.pressure = 1.0f;
            touch.size = 1.0f;
            touch.state = TouchPoint::Moved;
            platform->handleTouchMove(touch);
        }
    }
}

void DesktopPlatform::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Handle scroll input
}

void DesktopPlatform::simulateTouchFromMouse() {
    if (!window) return;
    
    // This is handled in the callbacks above
}

void DesktopPlatform::updateDeviceInfo() {
    deviceInfo.platform = Platform::Desktop;
    deviceInfo.deviceModel = "Desktop PC";
    deviceInfo.osVersion = "Unknown";
    deviceInfo.screenDPI = 96.0f;
    deviceInfo.screenScale = 1.0f;
    deviceInfo.hasMultitouch = false;
    deviceInfo.hasGyroscope = false;
    deviceInfo.hasAccelerometer = false;
    deviceInfo.hasHapticFeedback = false;
    deviceInfo.maxTouchPoints = 1;
    deviceInfo.memoryMB = 8192; // Default assumption
}