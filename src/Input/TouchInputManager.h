#pragma once
#include "../Math/Vector2.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>

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
        bool isDynamic;  // For dynamic joystick that appears where touched
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
    VirtualJoystick leftJoystick;   // For movement/steering (dynamic)
    VirtualJoystick rightJoystick;  // For camera control (right half screen)
    std::vector<VirtualButton> buttons;
    
    // Camera touch tracking
    Vector2 lastCameraTouchPosition;
    Vector2 cameraTouchDelta;
    
    // Movement smoothing
    Vector2 smoothedMovementInput;
    float movementSmoothingFactor;
    
    // Touch areas
    Vector2 rightHalfStart;  // Right half screen for camera control
    Vector2 rightHalfEnd;
    Vector2 leftHalfStart;   // Left half screen for movement joystick
    Vector2 leftHalfEnd;
    
    // Screen dimensions
    float screenWidth;
    float screenHeight;
    
    // Touch sensitivity
    float touchSensitivity;
    float joystickDeadzone;
    float cameraSensitivity;
    
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
    void setupLeftJoystick(Vector2 center, float outerRadius = 100.0f, float innerRadius = 40.0f);
    void setupRightJoystick(Vector2 center, float outerRadius = 100.0f, float innerRadius = 40.0f);
    void setupDynamicJoystick(float outerRadius = 80.0f, float innerRadius = 30.0f);
    void setupScreenHalves();
    
    // Virtual button setup
    void addButton(const std::string& label, Vector2 position, float radius = 50.0f);
    void removeButton(const std::string& label);
    
    // Virtual control queries
    Vector2 getLeftJoystickDirection() const;
    float getLeftJoystickMagnitude() const;
    Vector2 getMovementInput() const;
    void setMovementSmoothing(float factor);  // Smoothed movement input
    Vector2 getRightJoystickDirection() const;
    float getRightJoystickMagnitude() const;
    Vector2 getCameraTouchDirection() const;  // For right half screen camera control
    float getCameraTouchMagnitude() const;
    Vector2 getCameraTouchDelta() const;  // Raw delta movement for camera
    float getCameraSensitivity() const { return cameraSensitivity; }
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
    void setCameraSensitivity(float sensitivity);
    
    // Callbacks
    void setTouchBeganCallback(std::function<void(const Touch&)> callback);
    void setTouchMovedCallback(std::function<void(const Touch&)> callback);
    void setTouchEndedCallback(std::function<void(const Touch&)> callback);
    void setGestureCallback(std::function<void(const Gesture&)> callback);
    
    // Rendering support (positions for UI overlay)
    const VirtualJoystick& getLeftJoystickState() const { return leftJoystick; }
    const VirtualJoystick& getRightJoystickState() const { return rightJoystick; }
    const std::vector<VirtualButton>& getButtons() const { return buttons; }
    
private:
    void updateVirtualControls();
    void updateJoystick(VirtualJoystick& joystick);
    void updateButtons();
    void detectGestures();
    void processTouchForJoystick(const Touch& touch, VirtualJoystick& joystick);
    void processTouchForButtons(const Touch& touch);
    bool isTouchInCircle(Vector2 touchPos, Vector2 center, float radius) const;
    bool isTouchInRightHalf(Vector2 touchPos) const;
    bool isTouchInLeftHalf(Vector2 touchPos) const;
    float calculateDistance(Vector2 a, Vector2 b) const;
};
