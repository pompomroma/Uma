#pragma once
#include "../Math/Vector2.h"
#include "../Platform/PlatformBase.h"
#include <vector>
#include <functional>
#include <chrono>

class TouchInputManager {
public:
    enum class GestureType {
        None,
        Tap,
        DoubleTap,
        LongPress,
        Swipe,
        Pinch,
        Rotate,
        Pan
    };
    
    struct Gesture {
        GestureType type;
        Vector2 startPosition;
        Vector2 currentPosition;
        Vector2 delta;
        float scale;      // For pinch
        float rotation;   // For rotate
        float velocity;
        float distance;
        std::chrono::steady_clock::time_point startTime;
        int touchCount;
        bool isActive;
    };
    
    struct TouchControl {
        enum Type {
            Button,
            Joystick,
            Slider,
            DPad
        } type;
        
        Vector2 position;
        Vector2 size;
        float value;  // For slider
        Vector2 axis; // For joystick/dpad
        bool isPressed;
        bool isVisible;
        std::string label;
        std::function<void()> onPress;
        std::function<void()> onRelease;
        std::function<void(float)> onValueChange;
        std::function<void(Vector2)> onAxisChange;
    };
    
private:
    std::vector<PlatformBase::TouchPoint> currentTouches;
    std::vector<TouchControl> virtualControls;
    Gesture currentGesture;
    
    // Touch zones for different controls
    struct TouchZone {
        Vector2 min, max;
        std::function<void(const PlatformBase::TouchPoint&)> handler;
    };
    std::vector<TouchZone> touchZones;
    
    // Gesture recognition
    std::chrono::steady_clock::time_point lastTapTime;
    Vector2 lastTapPosition;
    bool isRecognizingGesture;
    float doubleTapThreshold;
    float longPressThreshold;
    float swipeThreshold;
    float pinchThreshold;
    
    // Virtual joystick state
    struct VirtualJoystick {
        Vector2 center;
        float radius;
        Vector2 currentPosition;
        Vector2 axis;
        bool isActive;
        int touchId;
    };
    VirtualJoystick leftJoystick;   // Movement
    VirtualJoystick rightJoystick;  // Camera
    
    // Virtual buttons
    struct VirtualButton {
        Vector2 position;
        float radius;
        bool isPressed;
        int touchId;
        std::string label;
        std::function<void(bool)> callback;
    };
    std::vector<VirtualButton> virtualButtons;
    
    // Accelerometer/Gyroscope
    Vector2 tiltInput;
    bool useTiltControls;
    float tiltSensitivity;
    
    // Callbacks
    std::function<void(GestureType, const Gesture&)> onGesture;
    std::function<void(Vector2)> onMovementInput;
    std::function<void(Vector2)> onCameraInput;
    std::function<void(const std::string&, bool)> onButtonInput;
    
    // Screen layout
    enum class ControlLayout {
        Racing,     // Tilt/touch steering, buttons for gas/brake
        Combat,     // Dual joysticks + action buttons
        Menu        // Simple touch navigation
    };
    ControlLayout currentLayout;
    
public:
    TouchInputManager();
    ~TouchInputManager();
    
    // Initialization
    void initialize(int screenWidth, int screenHeight);
    void shutdown();
    
    // Touch handling
    void handleTouchBegin(const PlatformBase::TouchPoint& touch);
    void handleTouchMove(const PlatformBase::TouchPoint& touch);
    void handleTouchEnd(const PlatformBase::TouchPoint& touch);
    void handleMultiTouch(const std::vector<PlatformBase::TouchPoint>& touches);
    
    // Gesture recognition
    void updateGestureRecognition(float deltaTime);
    bool isGestureActive(GestureType type) const;
    const Gesture& getCurrentGesture() const { return currentGesture; }
    
    // Virtual controls
    void setControlLayout(ControlLayout layout);
    void addVirtualButton(const std::string& label, Vector2 position, float radius, std::function<void(bool)> callback);
    void addVirtualJoystick(bool isLeft, Vector2 center, float radius);
    void updateVirtualControls();
    void renderVirtualControls(); // For debug visualization
    
    // Control queries
    Vector2 getMovementInput() const;
    Vector2 getCameraInput() const;
    bool isButtonPressed(const std::string& label) const;
    float getAccelerateInput() const;
    float getBrakeInput() const;
    float getSteerInput() const;
    
    // Accelerometer/Gyroscope
    void setTiltControlsEnabled(bool enabled);
    void handleAccelerometer(float x, float y, float z);
    void handleGyroscope(float x, float y, float z);
    Vector2 getTiltInput() const { return tiltInput; }
    
    // Settings
    void setJoystickDeadzone(float deadzone);
    void setTiltSensitivity(float sensitivity);
    void setDoubleTapThreshold(float threshold) { doubleTapThreshold = threshold; }
    void setLongPressThreshold(float threshold) { longPressThreshold = threshold; }
    void setSwipeThreshold(float threshold) { swipeThreshold = threshold; }
    
    // Callbacks
    void setGestureCallback(std::function<void(GestureType, const Gesture&)> cb) { onGesture = cb; }
    void setMovementCallback(std::function<void(Vector2)> cb) { onMovementInput = cb; }
    void setCameraCallback(std::function<void(Vector2)> cb) { onCameraInput = cb; }
    void setButtonCallback(std::function<void(const std::string&, bool)> cb) { onButtonInput = cb; }
    
    // Utility
    void clearTouches();
    void reset();
    bool isAnyTouchActive() const { return !currentTouches.empty(); }
    int getActiveTouchCount() const { return currentTouches.size(); }
    Vector2 screenToNormalized(Vector2 screenPos) const;
    
private:
    void setupRacingControls();
    void setupCombatControls();
    void setupMenuControls();
    
    void processTouch(const PlatformBase::TouchPoint& touch);
    void updateJoystick(VirtualJoystick& joystick, const PlatformBase::TouchPoint& touch);
    void updateButton(VirtualButton& button, const PlatformBase::TouchPoint& touch);
    
    void detectTap(const PlatformBase::TouchPoint& touch);
    void detectDoubleTap(const PlatformBase::TouchPoint& touch);
    void detectLongPress();
    void detectSwipe(const PlatformBase::TouchPoint& touch);
    void detectPinch(const std::vector<PlatformBase::TouchPoint>& touches);
    void detectRotate(const std::vector<PlatformBase::TouchPoint>& touches);
    
    float getJoystickValue(const VirtualJoystick& joystick, bool horizontal) const;
    bool isTouchInButton(const PlatformBase::TouchPoint& touch, const VirtualButton& button) const;
    bool isTouchInJoystick(const PlatformBase::TouchPoint& touch, const VirtualJoystick& joystick) const;
    
    int screenWidth, screenHeight;
    float joystickDeadzone;
};