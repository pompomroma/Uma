#pragma once
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>
#include <functional>

// Mobile touch input manager for Android and iOS
class TouchInputManager {
public:
    enum class TouchPhase {
        Began,
        Moved,
        Stationary,
        Ended,
        Cancelled
    };

    struct Touch {
        int id;
        Vector2 position;
        Vector2 previousPosition;
        Vector2 deltaPosition;
        TouchPhase phase;
        float pressure;
        double timestamp;
    };

    // Virtual control elements
    struct VirtualJoystick {
        Vector2 centerPosition;
        Vector2 currentPosition;
        float outerRadius;
        float innerRadius;
        bool isActive;
        int touchId;
        Vector2 direction;
        float magnitude;
        bool isDynamic;  // For dynamic positioning
        float fadeAlpha; // For visual fading when inactive
    };

    struct VirtualButton {
        Vector2 position;
        float radius;
        bool isPressed;
        bool wasPressed;
        int touchId;
        std::string label;
    };

    enum class GestureType {
        Tap,
        DoubleTap,
        LongPress,
        Swipe,
        Pinch,
        Rotate
    };

    struct Gesture {
        GestureType type;
        Vector2 position;
        Vector2 deltaPosition;
        float scale;
        float rotation;
        double timestamp;
    };

private:
    std::unordered_map<int, Touch> activeTouches;
    std::vector<Gesture> currentGestures;
    
    // Virtual controls
    VirtualJoystick leftJoystick;   // Dynamic joystick for movement
    std::vector<VirtualButton> buttons;
    
    // Camera drag control
    bool isCameraDragging;
    int cameraDragTouchId;
    Vector2 cameraDragStartPos;
    Vector2 cameraDragCurrentPos;
    Vector2 cameraDragDelta;
    
    // Screen dimensions
    float screenWidth;
    float screenHeight;
    
    // Touch sensitivity
    float touchSensitivity;
    float joystickDeadzone;
    
    // Gesture detection
    double doubleTapTimeThreshold;
    double longPressTimeThreshold;
    float swipeDistanceThreshold;
    
    // Callbacks
    std::function<void(const Touch&)> onTouchBegan;
    std::function<void(const Touch&)> onTouchMoved;
    std::function<void(const Touch&)> onTouchEnded;
    std::function<void(const Gesture&)> onGesture;

public:
    TouchInputManager();
    ~TouchInputManager();
    
    // Initialization
    void initialize(float width, float height);
    void shutdown();
    
    // Update
    void update(float deltaTime);
    
    // Touch input
    void registerTouch(int touchId, float x, float y, TouchPhase phase, float pressure = 1.0f);
    void updateTouch(int touchId, float x, float y);
    void endTouch(int touchId);
    void cancelTouch(int touchId);
    
    // Touch queries
    bool isTouchActive(int touchId) const;
    Touch getTouch(int touchId) const;
    std::vector<Touch> getAllTouches() const;
    int getTouchCount() const;
    
    // Virtual joystick setup
    void setupDynamicJoystick(float outerRadius = 100.0f, float innerRadius = 40.0f);
    
    // Virtual button setup
    void addButton(const std::string& label, Vector2 position, float radius = 50.0f);
    void removeButton(const std::string& label);
    
    // Virtual control queries
    Vector2 getJoystickDirection() const;
    float getJoystickMagnitude() const;
    Vector2 getCameraDragDelta() const { return cameraDragDelta; }
    bool isCameraBeingDragged() const { return isCameraDragging; }
    bool isButtonPressed(const std::string& label) const;
    bool isButtonJustPressed(const std::string& label) const;
    bool isButtonJustReleased(const std::string& label) const;
    
    // Gesture detection
    std::vector<Gesture> getGestures() const { return currentGestures; }
    bool hasGesture(GestureType type) const;
    Gesture getGesture(GestureType type) const;
    
    // Settings
    void setScreenSize(float width, float height);
    void setTouchSensitivity(float sensitivity);
    void setJoystickDeadzone(float deadzone);
    
    // Callbacks
    void setTouchBeganCallback(std::function<void(const Touch&)> callback);
    void setTouchMovedCallback(std::function<void(const Touch&)> callback);
    void setTouchEndedCallback(std::function<void(const Touch&)> callback);
    void setGestureCallback(std::function<void(const Gesture&)> callback);
    
    // Rendering support (positions for UI overlay)
    const VirtualJoystick& getJoystickState() const { return leftJoystick; }
    const std::vector<VirtualButton>& getButtons() const { return buttons; }
    
private:
    void updateVirtualControls();
    void updateDynamicJoystick();
    void updateCameraDrag();
    void updateButtons();
    void detectGestures();
    void processTouchForControls(const Touch& touch);
    void processTouchForButtons(const Touch& touch);
    bool isTouchInCircle(Vector2 touchPos, Vector2 center, float radius) const;
    bool isTouchInLeftHalf(const Touch& touch) const;
    bool isTouchInRightHalf(const Touch& touch) const;
    float calculateDistance(Vector2 a, Vector2 b) const;
};
