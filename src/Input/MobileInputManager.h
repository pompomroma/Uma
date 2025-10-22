#pragma once
#include "../Platform/Platform.h"
#include "../Platform/MobileInput.h"
#include "../Math/Vector2.h"
#include <unordered_map>
#include <functional>
#include <vector>

class MobileInputManager {
public:
    enum class Action {
        // Racing controls
        Accelerate,
        Brake,
        SteerLeft,
        SteerRight,
        Boost,
        Handbrake,
        CameraLook,
        CameraZoom,
        
        // Combat controls
        Attack1,
        Attack2,
        Shield,
        Teleport,
        Interact,
        
        // UI controls
        Pause,
        Menu,
        Back,
        Select,
        
        // Gestures
        Tap,
        DoubleTap,
        LongPress,
        SwipeLeft,
        SwipeRight,
        SwipeUp,
        SwipeDown,
        PinchIn,
        PinchOut,
        RotateLeft,
        RotateRight,
        
        Count
    };

    struct ActionBinding {
        Action action;
        MobileInput::VirtualControls::ControlType controlType;
        int controlId;
        float threshold;
        bool isEnabled;
    };

    struct TouchState {
        Vector2 position;
        Vector2 delta;
        bool isPressed;
        bool isJustPressed;
        bool isJustReleased;
        float pressure;
    };

    struct GestureState {
        MobileInput::TouchManager::Gesture::Type type;
        Vector2 startPosition;
        Vector2 endPosition;
        Vector2 delta;
        float scale;
        float rotation;
        float duration;
        bool isActive;
    };

private:
    std::vector<ActionBinding> m_actionBindings;
    std::unordered_map<Action, bool> m_actionStates;
    std::unordered_map<Action, bool> m_previousActionStates;
    std::unordered_map<Action, std::function<void()>> m_actionCallbacks;
    
    TouchState m_touchState;
    GestureState m_gestureState;
    
    // Mobile-specific settings
    float m_controlSensitivity;
    float m_controlDeadZone;
    float m_controlScale;
    float m_controlOpacity;
    std::string m_currentLayout;
    
    // Virtual controls
    int m_joystickId;
    int m_accelerateButtonId;
    int m_brakeButtonId;
    int m_boostButtonId;
    int m_handbrakeButtonId;
    int m_attack1ButtonId;
    int m_attack2ButtonId;
    int m_shieldButtonId;
    int m_teleportButtonId;
    int m_pauseButtonId;
    int m_menuButtonId;
    
    bool m_initialized;

public:
    MobileInputManager();
    ~MobileInputManager();
    
    // Initialization
    bool initialize();
    void shutdown();
    void update(float deltaTime);
    
    // Action management
    void bindAction(Action action, MobileInput::VirtualControls::ControlType controlType, int controlId, float threshold = 0.1f);
    void unbindAction(Action action);
    void setActionEnabled(Action action, bool enabled);
    
    // Action queries
    bool isActionPressed(Action action) const;
    bool isActionJustPressed(Action action) const;
    bool isActionJustReleased(Action action) const;
    float getActionValue(Action action) const;
    
    // Touch queries
    bool isTouching() const;
    int getTouchCount() const;
    Vector2 getTouchPosition(int index = 0) const;
    Vector2 getTouchDelta(int index = 0) const;
    bool isTouchPressed(int index = 0) const;
    bool isTouchJustPressed(int index = 0) const;
    bool isTouchJustReleased(int index = 0) const;
    float getTouchPressure(int index = 0) const;
    
    // Gesture queries
    bool isGestureActive() const;
    GestureState getGestureState() const;
    bool isTap() const;
    bool isDoubleTap() const;
    bool isLongPress() const;
    bool isPanning() const;
    bool isPinching() const;
    bool isRotating() const;
    bool isSwipe() const;
    Vector2 getPanDelta() const;
    float getPinchScale() const;
    float getRotationAngle() const;
    Vector2 getSwipeDirection() const;
    
    // Racing-specific input
    float getAccelerateInput() const;
    float getBrakeInput() const;
    float getSteerInput() const;
    bool getBoostInput() const;
    bool getHandbrakeInput() const;
    Vector2 getCameraLookInput() const;
    float getCameraZoomInput() const;
    
    // Combat-specific input
    bool getAttack1Input() const;
    bool getAttack2Input() const;
    bool getShieldInput() const;
    bool getTeleportInput() const;
    bool getInteractInput() const;
    
    // Callback registration
    void setActionCallback(Action action, std::function<void()> callback);
    
    // Mobile control setup
    void setupRacingControls();
    void setupCombatControls();
    void setupMenuControls();
    void setupGestureControls();
    
    // Configuration
    void setControlSensitivity(float sensitivity);
    void setControlDeadZone(float deadZone);
    void setControlScale(float scale);
    void setControlOpacity(float opacity);
    
    // Layout management
    void setControlLayout(const std::string& layout);
    void saveControlLayout(const std::string& layout);
    void loadControlLayout(const std::string& layout);
    void resetToDefaultLayout();
    
    // Virtual control management
    void setJoystickVisible(bool visible);
    void setJoystickPosition(const Vector2& position);
    void setJoystickRadius(float radius);
    Vector2 getJoystickInput() const;
    bool isJoystickActive() const;
    
    void setButtonVisible(Action action, bool visible);
    void setButtonPosition(Action action, const Vector2& position);
    void setButtonSize(Action action, const Vector2& size);
    void setButtonLabel(Action action, const std::string& label);
    
    // Rendering
    void renderControls();
    
    // Event handling
    void onTouchDown(int id, float x, float y, float pressure);
    void onTouchMove(int id, float x, float y, float pressure);
    void onTouchUp(int id, float x, float y);
    void onTouchCancel(int id);
    
    // Utility
    void clearInputState();
    void resetToDefaults();

private:
    void processActions();
    void updateActionStates();
    void updateTouchState();
    void updateGestureState();
    bool evaluateAction(const ActionBinding& binding) const;
    float getActionValue(const ActionBinding& binding) const;
    
    // Control creation helpers
    void createRacingControls();
    void createCombatControls();
    void createMenuControls();
    void createGestureControls();
    
    // Layout helpers
    void applyLayout(const std::string& layout);
    void saveCurrentLayout();
    void loadSavedLayout();
    
    // Default layouts
    void setDefaultRacingLayout();
    void setDefaultCombatLayout();
    void setDefaultMenuLayout();
    void setDefaultGestureLayout();
};