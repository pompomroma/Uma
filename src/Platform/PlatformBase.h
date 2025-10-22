#pragma once
#include <string>
#include <functional>
#include <vector>

class PlatformBase {
public:
    enum class Platform {
        Desktop,
        Android,
        iOS
    };
    
    struct TouchPoint {
        int id;
        float x, y;
        float pressure;
        float size;
        enum State {
            Began,
            Moved,
            Stationary,
            Ended,
            Cancelled
        } state;
    };
    
    struct DeviceOrientation {
        enum Type {
            Portrait,
            PortraitUpsideDown,
            LandscapeLeft,
            LandscapeRight
        } orientation;
        bool isLocked;
    };
    
    struct DeviceInfo {
        Platform platform;
        std::string deviceModel;
        std::string osVersion;
        int screenWidth;
        int screenHeight;
        float screenDPI;
        float screenScale;
        bool hasMultitouch;
        bool hasGyroscope;
        bool hasAccelerometer;
        bool hasHapticFeedback;
        int maxTouchPoints;
        int memoryMB;
    };

protected:
    Platform currentPlatform;
    DeviceInfo deviceInfo;
    DeviceOrientation orientation;
    std::vector<TouchPoint> activeTouches;
    
    // Callbacks
    std::function<void(const TouchPoint&)> onTouchBegin;
    std::function<void(const TouchPoint&)> onTouchMove;
    std::function<void(const TouchPoint&)> onTouchEnd;
    std::function<void(const std::vector<TouchPoint>&)> onMultiTouch;
    std::function<void(DeviceOrientation)> onOrientationChange;
    std::function<void(float, float, float)> onAccelerometer;
    std::function<void(float, float, float)> onGyroscope;
    std::function<void()> onPause;
    std::function<void()> onResume;
    std::function<void()> onLowMemory;

public:
    PlatformBase();
    virtual ~PlatformBase();
    
    // Platform initialization
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    // Window/View management
    virtual bool createWindow(int width, int height, const std::string& title) = 0;
    virtual void* getNativeWindowHandle() = 0;
    virtual void setFullscreen(bool fullscreen) = 0;
    virtual void getWindowSize(int& width, int& height) = 0;
    
    // Input handling
    virtual void processEvents() = 0;
    virtual bool shouldClose() = 0;
    virtual void setCursorVisible(bool visible) = 0;
    virtual void setCursorLocked(bool locked) = 0;
    
    // Mobile-specific
    virtual void vibrate(int milliseconds) = 0;
    virtual void setKeepScreenOn(bool keepOn) = 0;
    virtual bool requestPermission(const std::string& permission) = 0;
    virtual void showVirtualKeyboard(bool show) = 0;
    virtual void setOrientationLock(DeviceOrientation::Type orientation) = 0;
    
    // Rendering
    virtual void swapBuffers() = 0;
    virtual void* getGLContext() = 0;
    virtual bool supportsOpenGLES() const = 0;
    virtual int getGLESVersion() const = 0;
    
    // File system
    virtual std::string getResourcePath() const = 0;
    virtual std::string getSavePath() const = 0;
    virtual std::string getCachePath() const = 0;
    
    // Platform info
    Platform getPlatform() const { return currentPlatform; }
    const DeviceInfo& getDeviceInfo() const { return deviceInfo; }
    DeviceOrientation getOrientation() const { return orientation; }
    const std::vector<TouchPoint>& getActiveTouches() const { return activeTouches; }
    
    // Callback setters
    void setTouchBeginCallback(std::function<void(const TouchPoint&)> cb) { onTouchBegin = cb; }
    void setTouchMoveCallback(std::function<void(const TouchPoint&)> cb) { onTouchMove = cb; }
    void setTouchEndCallback(std::function<void(const TouchPoint&)> cb) { onTouchEnd = cb; }
    void setMultiTouchCallback(std::function<void(const std::vector<TouchPoint>&)> cb) { onMultiTouch = cb; }
    void setOrientationChangeCallback(std::function<void(DeviceOrientation)> cb) { onOrientationChange = cb; }
    void setAccelerometerCallback(std::function<void(float, float, float)> cb) { onAccelerometer = cb; }
    void setGyroscopeCallback(std::function<void(float, float, float)> cb) { onGyroscope = cb; }
    void setPauseCallback(std::function<void()> cb) { onPause = cb; }
    void setResumeCallback(std::function<void()> cb) { onResume = cb; }
    void setLowMemoryCallback(std::function<void()> cb) { onLowMemory = cb; }
    
    // Touch helpers
    TouchPoint* getTouchById(int id);
    bool isTouchActive(int id) const;
    int getActiveTouchCount() const { return activeTouches.size(); }
    
    // Static factory
    static PlatformBase* createPlatform();
    
protected:
    void updateDeviceInfo();
    void handleTouchBegin(const TouchPoint& touch);
    void handleTouchMove(const TouchPoint& touch);
    void handleTouchEnd(const TouchPoint& touch);
    void handleOrientationChange(DeviceOrientation newOrientation);
};