#pragma once

#ifdef MOBILE_PLATFORM

#include "../Math/Vector2.h"
#include <vector>
#include <functional>
#include <unordered_map>
#include <chrono>

class TouchInputManager {
public:
    // Touch point structure
    struct TouchPoint {
        int id;                    // Unique touch identifier
        Vector2 position;          // Current position
        Vector2 startPosition;     // Initial touch position
        Vector2 deltaPosition;     // Change since last frame
        Vector2 velocity;          // Touch velocity
        float pressure;            // Touch pressure (0.0 to 1.0)
        bool isActive;             // Is this touch currently active
        std::chrono::steady_clock::time_point startTime;  // When touch began
        std::chrono::steady_clock::time_point lastUpdateTime;  // Last update time
        
        TouchPoint() : id(-1), pressure(1.0f), isActive(false) {}
    };
    
    // Gesture types
    enum class GestureType {
        None,
        Tap,
        DoubleTap,
        LongPress,
        Swipe,
        Pinch,
        Pan,
        Rotate
    };
    
    // Gesture data
    struct Gesture {
        GestureType type;
        Vector2 position;          // Gesture center position
        Vector2 startPosition;     // Where gesture started
        Vector2 deltaPosition;     // Change in position
        float scale;               // For pinch gestures
        float rotation;            // For rotation gestures (radians)
        float velocity;            // Gesture velocity
        bool isComplete;           // Is gesture finished
        std::vector<int> touchIds; // Touch points involved in gesture
        
        Gesture() : type(GestureType::None), scale(1.0f), rotation(0.0f), 
                   velocity(0.0f), isComplete(false) {}
    };
    
    // Racing game specific actions
    enum class RacingAction {
        Accelerate,
        Brake,
        SteerLeft,
        SteerRight,
        Boost,
        Handbrake,
        CameraLook,
        CameraZoom,
        Pause,
        Reset,
        // Combat actions
        Attack1,
        Attack2,
        Shield,
        Teleport,
        Interact,
        StatMenu
    };

private:
    // Touch tracking
    std::vector<TouchPoint> activeTouches;
    std::vector<TouchPoint> previousTouches;
    int maxTouches;
    
    // Gesture recognition
    std::vector<Gesture> activeGestures;
    std::vector<Gesture> completedGestures;
    
    // Gesture thresholds
    float tapMaxDistance;          // Max movement for tap
    float tapMaxDuration;          // Max time for tap (seconds)
    float doubleTapMaxInterval;    // Max time between taps for double tap
    float longPressMinDuration;    // Min time for long press
    float swipeMinDistance;        // Min distance for swipe
    float swipeMaxDuration;        // Max time for swipe
    float pinchMinDistance;        // Min distance change for pinch
    
    // Screen properties
    Vector2 screenSize;
    float screenDensity;           // Pixels per inch
    
    // Input callbacks for racing actions
    std::function<void(float)> onAccelerate;
    std::function<void(float)> onBrake;
    std::function<void(float)> onSteer;
    std::function<void(bool)> onBoost;
    std::function<void(bool)> onHandbrake;
    std::function<void(float, float)> onCameraLook;
    std::function<void(float)> onCameraZoom;
    std::function<void()> onPause;
    std::function<void()> onReset;
    
    // Combat callbacks
    std::function<void()> onAttack1;
    std::function<void()> onAttack2;
    std::function<void()> onShield;
    std::function<void()> onTeleport;
    std::function<void()> onInteract;
    std::function<void()> onStatMenu;
    
    // Touch zones for racing controls
    struct TouchZone {
        Vector2 center;
        Vector2 size;
        RacingAction action;
        bool isActive;
        float currentValue;        // For analog controls (0.0 to 1.0)
        
        TouchZone() : isActive(false), currentValue(0.0f) {}
        TouchZone(const Vector2& c, const Vector2& s, RacingAction a) 
            : center(c), size(s), action(a), isActive(false), currentValue(0.0f) {}
    };
    
    std::vector<TouchZone> touchZones;
    
    // Virtual joystick for steering
    struct VirtualJoystick {
        Vector2 center;
        float outerRadius;
        float innerRadius;
        Vector2 knobPosition;
        bool isActive;
        int activeTouchId;
        
        VirtualJoystick() : outerRadius(50.0f), innerRadius(20.0f), 
                           isActive(false), activeTouchId(-1) {}
    };
    
    VirtualJoystick steeringJoystick;
    VirtualJoystick cameraJoystick;
    
    // Input processing
    bool isEnabled;
    bool showTouchZones;           // For debugging
    
public:
    TouchInputManager();
    ~TouchInputManager();
    
    // Initialization
    void initialize(const Vector2& screenSize, float screenDensity = 160.0f);
    void shutdown();
    
    // Screen management
    void setScreenSize(const Vector2& size);
    void setScreenDensity(float density);
    void handleScreenRotation();
    
    // Touch input processing
    void update(float deltaTime);
    void processTouchDown(int touchId, const Vector2& position, float pressure = 1.0f);
    void processTouchMove(int touchId, const Vector2& position, float pressure = 1.0f);
    void processTouchUp(int touchId, const Vector2& position);
    void processTouchCancel(int touchId);
    
    // Touch queries
    bool isTouchActive(int touchId) const;
    TouchPoint* getTouch(int touchId);
    const TouchPoint* getTouch(int touchId) const;
    std::vector<TouchPoint> getActiveTouches() const;
    int getActiveTouchCount() const;
    
    // Gesture recognition
    void updateGestures(float deltaTime);
    std::vector<Gesture> getActiveGestures() const;
    std::vector<Gesture> getCompletedGestures() const;
    void clearCompletedGestures();
    
    // Racing control setup
    void setupRacingControls();
    void setupTouchZones();
    void setupVirtualJoysticks();
    
    // Touch zone management
    void addTouchZone(const Vector2& center, const Vector2& size, RacingAction action);
    void removeTouchZone(RacingAction action);
    void updateTouchZones();
    TouchZone* getTouchZone(RacingAction action);
    
    // Virtual joystick management
    void updateVirtualJoysticks();
    Vector2 getSteeringInput() const;
    Vector2 getCameraInput() const;
    
    // Racing input values
    float getAccelerateInput() const;
    float getBrakeInput() const;
    float getSteerInput() const;
    bool getBoostInput() const;
    bool getHandbrakeInput() const;
    Vector2 getCameraLookInput() const;
    float getCameraZoomInput() const;
    
    // Callback registration
    void setAccelerateCallback(std::function<void(float)> callback);
    void setBrakeCallback(std::function<void(float)> callback);
    void setSteerCallback(std::function<void(float)> callback);
    void setBoostCallback(std::function<void(bool)> callback);
    void setHandbrakeCallback(std::function<void(bool)> callback);
    void setCameraLookCallback(std::function<void(float, float)> callback);
    void setCameraZoomCallback(std::function<void(float)> callback);
    void setPauseCallback(std::function<void()> callback);
    void setResetCallback(std::function<void()> callback);
    
    // Combat callback registration
    void setAttack1Callback(std::function<void()> callback);
    void setAttack2Callback(std::function<void()> callback);
    void setShieldCallback(std::function<void()> callback);
    void setTeleportCallback(std::function<void()> callback);
    void setInteractCallback(std::function<void()> callback);
    void setStatMenuCallback(std::function<void()> callback);
    
    // Settings
    void setEnabled(bool enabled);
    void setShowTouchZones(bool show);
    void setGestureThresholds(float tapDistance, float tapDuration, float longPressDuration);
    
    // Utility functions
    bool isPointInZone(const Vector2& point, const TouchZone& zone) const;
    bool isPointInCircle(const Vector2& point, const Vector2& center, float radius) const;
    float getDistance(const Vector2& a, const Vector2& b) const;
    float getAngle(const Vector2& a, const Vector2& b) const;
    Vector2 normalizeToScreen(const Vector2& position) const;
    Vector2 screenToNormalized(const Vector2& position) const;
    
    // Debug functions
    void renderTouchZones() const;
    void renderVirtualJoysticks() const;
    void renderActiveTouches() const;
    void logTouchInfo() const;
    
private:
    // Internal touch management
    TouchPoint* findTouch(int touchId);
    void addTouch(int touchId, const Vector2& position, float pressure);
    void updateTouch(int touchId, const Vector2& position, float pressure);
    void removeTouch(int touchId);
    void updateTouchVelocities(float deltaTime);
    
    // Gesture recognition helpers
    void recognizeTap(const TouchPoint& touch);
    void recognizeDoubleTap(const TouchPoint& touch);
    void recognizeLongPress(const TouchPoint& touch);
    void recognizeSwipe(const TouchPoint& touch);
    void recognizePinch();
    void recognizePan();
    void recognizeRotation();
    
    // Racing control processing
    void processRacingControls();
    void processVirtualJoystick(VirtualJoystick& joystick, const TouchPoint& touch);
    void updateTouchZoneValues();
    void triggerRacingCallbacks();
    
    // Utility helpers
    float calculateTouchPressure(const Vector2& position, const TouchZone& zone) const;
    Vector2 clampToCircle(const Vector2& position, const Vector2& center, float radius) const;
    float smoothStep(float edge0, float edge1, float x) const;
};

#endif // MOBILE_PLATFORM